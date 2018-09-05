////////////////////////////////////////////////////////////////////////////////
/// DISCLAIMER
///
/// Copyright 2014-2018 ArangoDB GmbH, Cologne, Germany
/// Copyright 2004-2014 triAGENS GmbH, Cologne, Germany
///
/// Licensed under the Apache License, Version 2.0 (the "License");
/// you may not use this file except in compliance with the License.
/// You may obtain a copy of the License at
///
///     http://www.apache.org/licenses/LICENSE-2.0
///
/// Unless required by applicable law or agreed to in writing, software
/// distributed under the License is distributed on an "AS IS" BASIS,
/// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
/// See the License for the specific language governing permissions and
/// limitations under the License.
///
/// Copyright holder is ArangoDB GmbH, Cologne, Germany
///
/// @author Jan Steemann
////////////////////////////////////////////////////////////////////////////////

#include "DBServerAgencySync.h"

#include "Basics/MutexLocker.h"
#include "Cluster/ClusterFeature.h"
#include "Cluster/ClusterInfo.h"
#include "Cluster/FollowerInfo.h"
#include "Cluster/HeartbeatThread.h"
#include "Cluster/Maintenance.h"
#include "Cluster/MaintenanceFeature.h"
#include "Cluster/MaintenanceStrings.h"
#include "Cluster/ServerState.h"
#include "Logger/Logger.h"
#include "RestServer/DatabaseFeature.h"
#include "RestServer/SystemDatabaseFeature.h"
#include "Utils/DatabaseGuard.h"
#include "VocBase/vocbase.h"
#include "VocBase/LogicalCollection.h"
#include "VocBase/Methods/Databases.h"

using namespace arangodb;
using namespace arangodb::application_features;
using namespace arangodb::methods;
using namespace arangodb::rest;

DBServerAgencySync::DBServerAgencySync(HeartbeatThread* heartbeat)
    : _heartbeat(heartbeat) {}

void DBServerAgencySync::work() {
  LOG_TOPIC(TRACE, Logger::CLUSTER) << "starting plan update handler";

  _heartbeat->setReady();

  DBServerAgencySyncResult result = execute();
  _heartbeat->dispatchedJobResult(result);
}

Result getLocalCollections(VPackBuilder& collections) {

  using namespace arangodb::basics;
  Result result;
  DatabaseFeature* dbfeature = nullptr;

  try {
    dbfeature = ApplicationServer::getFeature<DatabaseFeature>("Database");
  } catch (...) {}

  if (dbfeature == nullptr) {
    LOG_TOPIC(ERR, Logger::HEARTBEAT) << "Failed to get feature database";
    return Result(TRI_ERROR_INTERNAL, "Failed to get feature database");
  }

  collections.clear();
  VPackObjectBuilder c(&collections);
  for (auto const& database : Databases::list()) {

    try {
      DatabaseGuard guard(database);
      auto vocbase = &guard.database();

      collections.add(VPackValue(database));
      VPackObjectBuilder db(&collections);
      auto cols = vocbase->collections(false);

      for (auto const& collection : cols) {
        collections.add(VPackValue(collection->name()));
        VPackObjectBuilder col(&collections);
        collection->toVelocyPack(collections,true,false);
        collections.add(
          "theLeader", VPackValue(collection->followers()->getLeader()));
      }
    } catch (std::exception const& e) {
      return Result(
        TRI_ERROR_INTERNAL,
        std::string("Failed to guard database ") +  database + ": " + e.what());
    }

  }

  return Result();

}

DBServerAgencySyncResult DBServerAgencySync::execute() {
  // default to system database

  TRI_ASSERT(AgencyCommManager::isEnabled());
  AgencyComm comm;

  using namespace std::chrono;
  using clock = std::chrono::steady_clock;

  LOG_TOPIC(DEBUG, Logger::MAINTENANCE) << "DBServerAgencySync::execute starting";

  auto* sysDbFeature = application_features::ApplicationServer::lookupFeature<
    SystemDatabaseFeature
  >();
  MaintenanceFeature* mfeature =
    ApplicationServer::getFeature<MaintenanceFeature>("Maintenance");
  arangodb::SystemDatabaseFeature::ptr vocbase =
      sysDbFeature ? sysDbFeature->use() : nullptr;
  DBServerAgencySyncResult result;

  if (vocbase == nullptr) {
    LOG_TOPIC(DEBUG, Logger::MAINTENANCE)
      << "DBServerAgencySync::execute no vocbase";
    return result;
  }

  Result tmp;
  VPackBuilder rb;
  auto clusterInfo = ClusterInfo::instance();
  auto plan = clusterInfo->getPlan();
  auto serverId = arangodb::ServerState::instance()->getId();

  VPackBuilder local;
  Result glc = getLocalCollections(local);
  if (!glc.ok()) {
    // FIXMEMAINTENANCE: if this fails here, then result is empty, is this
    // intended? I also notice that there is another Result object "tmp"
    // that is going to eat bad results in few lines later. Again, is
    // that the correct action? If so, how about supporting comments in
    // the code for both.
    return result;
  }

  auto start = clock::now();
  try {
    // in previous life handlePlanChange

    VPackObjectBuilder o(&rb);

    LOG_TOPIC(DEBUG, Logger::MAINTENANCE) << "DBServerAgencySync::phaseOne";
    tmp = arangodb::maintenance::phaseOne(
      plan->slice(), local.slice(), serverId, *mfeature, rb);
    LOG_TOPIC(DEBUG, Logger::MAINTENANCE) << "DBServerAgencySync::phaseOne done";

    LOG_TOPIC(DEBUG, Logger::MAINTENANCE) << "DBServerAgencySync::phaseTwo";
    glc = getLocalCollections(local);
    // We intentionally refetch local collections here, such that phase 2
    // can already see potential changes introduced by phase 1. The two
    // phases are sufficiently independent that this is OK.
    LOG_TOPIC(TRACE, Logger::MAINTENANCE) << "DBServerAgencySync::phaseTwo - local state: " << local.toJson();
    if (!glc.ok()) {
      return result;
    }

    auto current = clusterInfo->getCurrent();
    LOG_TOPIC(TRACE, Logger::MAINTENANCE) << "DBServerAgencySync::phaseTwo - current state: " << current->toJson();

    tmp = arangodb::maintenance::phaseTwo(
      plan->slice(), current->slice(), local.slice(), serverId, *mfeature, rb);

    LOG_TOPIC(DEBUG, Logger::MAINTENANCE) << "DBServerAgencySync::phaseTwo done";

  } catch (std::exception const& e) {
    LOG_TOPIC(ERR, Logger::MAINTENANCE)
      << "Failed to handle plan change: " << e.what();
  }

  if (rb.isClosed()) {
    // FIXMEMAINTENANCE: when would rb not be closed? and if "catch"
    // just happened, would you want to be doing this anyway?

    auto report = rb.slice();
    if (report.isObject()) {

      std::vector<std::string> agency = {maintenance::PHASE_TWO, "agency"};
      if (report.hasKey(agency) && report.get(agency).isObject()) {

        auto phaseTwo = report.get(agency);
        LOG_TOPIC(DEBUG, Logger::MAINTENANCE)
          << "DBServerAgencySync reporting to Current: " << phaseTwo.toJson();

        // Report to current
        if (!phaseTwo.isEmptyObject()) {

          std::vector<AgencyOperation> operations;
          for (auto const& ao : VPackObjectIterator(phaseTwo)) {
            auto const key = ao.key.copyString();
            auto const op = ao.value.get("op").copyString();
            if (op == "set") {
              auto const value = ao.value.get("payload");
              operations.push_back(
                AgencyOperation(key, AgencyValueOperationType::SET, value));
            } else if (op == "delete") {
              operations.push_back(
                AgencyOperation(key, AgencySimpleOperationType::DELETE_OP));
            }
          }
          operations.push_back(
            AgencyOperation(
              "Current/Version", AgencySimpleOperationType::INCREMENT_OP));
          AgencyWriteTransaction currentTransaction(operations);
          AgencyCommResult r = comm.sendTransactionWithFailover(currentTransaction);
          if (!r.successful()) {
            LOG_TOPIC(ERR, Logger::MAINTENANCE) << "Error reporting to agency";
          } else {
            LOG_TOPIC(DEBUG, Logger::MAINTENANCE) << "Invalidating current in ClusterInfo";
            clusterInfo->invalidateCurrent();
          }
        }
      }

      // FIXMEMAINTENANCE: If comm.sendTransactionWithFailover()
      // fails, the result is ok() based upon phaseTwo()'s execution?
      result = DBServerAgencySyncResult(
        tmp.ok(),
        report.hasKey("Plan") ?
        report.get("Plan").get("Version").getNumber<uint64_t>() : 0,
        report.hasKey("Current") ?
        report.get("Current").get("Version").getNumber<uint64_t>() : 0);

    }
  }
  
  auto took = duration<double>(clock::now() - start).count();
  if (took > 30.0) {
    LOG_TOPIC(WARN, Logger::MAINTENANCE) << "DBServerAgencySync::execute "
      "took " << took << " s to execute handlePlanChange";
  }

  return result;
}
