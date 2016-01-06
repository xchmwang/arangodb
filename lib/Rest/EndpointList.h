////////////////////////////////////////////////////////////////////////////////
/// DISCLAIMER
///
/// Copyright 2014-2016 ArangoDB GmbH, Cologne, Germany
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

#ifndef LIB_REST_ENDPOINT_LIST_H
#define LIB_REST_ENDPOINT_LIST_H 1

#include "Basics/Common.h"

#include "Rest/Endpoint.h"


namespace triagens {
namespace rest {

class EndpointList {
 public:
  
 public:
  ////////////////////////////////////////////////////////////////////////////////
  /// @brief creates an endpoint list
  ////////////////////////////////////////////////////////////////////////////////

  EndpointList();

  ////////////////////////////////////////////////////////////////////////////////
  /// @brief destroys an endpoint list
  ////////////////////////////////////////////////////////////////////////////////

  ~EndpointList();

  
 public:
  ////////////////////////////////////////////////////////////////////////////////
  /// @brief returns whether the list is empty
  ////////////////////////////////////////////////////////////////////////////////

  bool empty() const { return _endpoints.empty(); }

  ////////////////////////////////////////////////////////////////////////////////
  /// @brief add a new endpoint
  ////////////////////////////////////////////////////////////////////////////////

  bool add(std::string const&, std::vector<std::string> const&, int, bool,
           Endpoint** = nullptr);

  ////////////////////////////////////////////////////////////////////////////////
  /// @brief remove a specific endpoint
  ////////////////////////////////////////////////////////////////////////////////

  bool remove(std::string const&, Endpoint**);

  ////////////////////////////////////////////////////////////////////////////////
  /// @brief return all databases for an endpoint
  ////////////////////////////////////////////////////////////////////////////////

  std::vector<std::string> const& getMapping(std::string const&) const;

  ////////////////////////////////////////////////////////////////////////////////
  /// @brief return all endpoints
  ////////////////////////////////////////////////////////////////////////////////

  std::map<std::string, std::vector<std::string>> getAll() const;

  ////////////////////////////////////////////////////////////////////////////////
  /// @brief return all endpoints with a certain prefix
  ////////////////////////////////////////////////////////////////////////////////

  std::map<std::string, Endpoint*> getByPrefix(std::string const&) const;

  ////////////////////////////////////////////////////////////////////////////////
  /// @brief return all endpoints with a certain encryption type
  ////////////////////////////////////////////////////////////////////////////////

  std::map<std::string, Endpoint*> getByPrefix(Endpoint::EncryptionType) const;

  ////////////////////////////////////////////////////////////////////////////////
  /// @brief return if there is an endpoint with a certain encryption type
  ////////////////////////////////////////////////////////////////////////////////

  bool has(Endpoint::EncryptionType) const;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief dump all endpoints used
  ////////////////////////////////////////////////////////////////////////////////

  void dump() const;

  ////////////////////////////////////////////////////////////////////////////////
  /// @brief return an encryption name
  ////////////////////////////////////////////////////////////////////////////////

  static std::string getEncryptionName(Endpoint::EncryptionType);

  
 private:
  ////////////////////////////////////////////////////////////////////////////////
  /// @brief list of endpoints
  ////////////////////////////////////////////////////////////////////////////////

  std::map<std::string, std::pair<Endpoint*, std::vector<std::string>>>
      _endpoints;
};
}
}

#endif


