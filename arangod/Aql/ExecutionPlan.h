////////////////////////////////////////////////////////////////////////////////
/// @brief Infrastructure for ExecutionPlans
///
/// @file arangod/Aql/ExecutionPlan.h
///
/// DISCLAIMER
///
/// Copyright 2010-2014 triagens GmbH, Cologne, Germany
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
/// Copyright holder is triAGENS GmbH, Cologne, Germany
///
/// @author Max Neunhoeffer
/// @author Copyright 2014, triagens GmbH, Cologne, Germany
////////////////////////////////////////////////////////////////////////////////

#ifndef ARANGODB_AQL_EXECUTION_PLAN_H
#define ARANGODB_AQL_EXECUTION_PLAN_H 1

#include <Basics/Common.h>

#include <BasicsC/json.h>
#include <Basics/JsonHelper.h>
#include <VocBase/voc-types.h>
#include <VocBase/vocbase.h>

namespace triagens {
  namespace aql {

////////////////////////////////////////////////////////////////////////////////
/// @brief class ExecutionPlan, abstract base class
////////////////////////////////////////////////////////////////////////////////

    class ExecutionPlan {

////////////////////////////////////////////////////////////////////////////////
/// @brief node type
////////////////////////////////////////////////////////////////////////////////

      public:

        enum NodeType {
          ILLEGAL,
          ENUMERATE_COLLECTION,
          INDEX_RANGE,
          STATIC_LIST,
          FILTER,
          LIMIT,
          INTERSECTION,
          PROJECTION,
          CALCULATION,
          SORT,
          AGGREGATE_ON_SORTED,
          AGGREGATE_ON_UNSORTED,
          LOOKUP_JOIN,
          MERGE_JOIN,
          LOOKUP_INDEX_UNIQUE,
          LOOKUP_INDEX_RANGE,
          LOOKUP_FULL_COLLECTION,
          CONCATENATION,
          MERGE,
          REMOTE,
          INSERT,
          REMOVE,
          REPLACE,
          UPDATE,
          ROOT
        };

// -----------------------------------------------------------------------------
// --SECTION--                                                    public methods
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @brief default constructor
////////////////////////////////////////////////////////////////////////////////

        ExecutionPlan () {
        }

////////////////////////////////////////////////////////////////////////////////
/// @brief constructor with one dependency
////////////////////////////////////////////////////////////////////////////////

        ExecutionPlan (ExecutionPlan* ep) {
          _dependencies.push_back(ep);
        }

////////////////////////////////////////////////////////////////////////////////
/// @brief destructor, free dependencies
////////////////////////////////////////////////////////////////////////////////

        virtual ~ExecutionPlan () { 
          for (auto i = _dependencies.begin(); i != _dependencies.end(); ++i) {
            delete *i;
          }
        }

////////////////////////////////////////////////////////////////////////////////
/// @brief return the type of the node
////////////////////////////////////////////////////////////////////////////////

        virtual NodeType getType () {
          return ILLEGAL;
        }

////////////////////////////////////////////////////////////////////////////////
/// @brief return the type of the node as a string
////////////////////////////////////////////////////////////////////////////////

        virtual std::string getTypeString () {
          return std::string("ExecutionPlan (abstract)");
        }

////////////////////////////////////////////////////////////////////////////////
/// @brief add a dependency
////////////////////////////////////////////////////////////////////////////////

        void addDependency (ExecutionPlan* ep) {
          _dependencies.push_back(ep);
        }

////////////////////////////////////////////////////////////////////////////////
/// @brief get all dependencies
////////////////////////////////////////////////////////////////////////////////

        vector<ExecutionPlan*> getDependencies () {
          return _dependencies;
        }

////////////////////////////////////////////////////////////////////////////////
/// @brief access the pos-th dependency
////////////////////////////////////////////////////////////////////////////////

        ExecutionPlan* operator[] (size_t pos) {
          if (pos > _dependencies.size()) {
            return nullptr;
          }
          else {
            return _dependencies.at(pos);
          }
        }

////////////////////////////////////////////////////////////////////////////////
/// @brief clone execution plan recursively, this makes the class abstract
////////////////////////////////////////////////////////////////////////////////

        virtual ExecutionPlan* clone () { // = 0;   make this abstract later
          return this;
        }

////////////////////////////////////////////////////////////////////////////////
/// @brief export to JSON, returns an AUTOFREE Json object
////////////////////////////////////////////////////////////////////////////////

        virtual triagens::basics::Json toJson (
                         TRI_memory_zone_t* zone = TRI_UNKNOWN_MEM_ZONE);

////////////////////////////////////////////////////////////////////////////////
/// @brief convert to a string, basically for debugging purposes
////////////////////////////////////////////////////////////////////////////////

        virtual void appendAsString (std::string& st, int indent = 0);

// -----------------------------------------------------------------------------
// --SECTION--                                                 private variables
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @brief our dependent nodes
////////////////////////////////////////////////////////////////////////////////

      private:

        std::vector<ExecutionPlan*> _dependencies;

    };

// -----------------------------------------------------------------------------
// --SECTION--                                     class EnumerateCollectionPlan
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @brief class EnumerateCollectionPlan, derived from ExecutionPlan
////////////////////////////////////////////////////////////////////////////////

    class EnumerateCollectionPlan : public ExecutionPlan {
      
////////////////////////////////////////////////////////////////////////////////
/// @brief constructor with a vocbase and a collection name
////////////////////////////////////////////////////////////////////////////////

      public:

        EnumerateCollectionPlan (TRI_vocbase_t* vocbase, std::string collname) 
          : ExecutionPlan(), _vocbase(vocbase), _collname(collname) {
        }

////////////////////////////////////////////////////////////////////////////////
/// @brief return the type of the node
////////////////////////////////////////////////////////////////////////////////

        virtual NodeType getType () {
          return ENUMERATE_COLLECTION;
        }

////////////////////////////////////////////////////////////////////////////////
/// @brief return the type of the node as a string
////////////////////////////////////////////////////////////////////////////////

        virtual std::string getTypeString () {
          return std::string("EnumerateCollectionPlan");
        }

////////////////////////////////////////////////////////////////////////////////
/// @brief export to JSON
////////////////////////////////////////////////////////////////////////////////

        virtual triagens::basics::Json toJson (
               TRI_memory_zone_t* zone = TRI_UNKNOWN_MEM_ZONE);

// -----------------------------------------------------------------------------
// --SECTION--                                                 private variables
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @brief we need to know the database and the collection
////////////////////////////////////////////////////////////////////////////////

      private:

        TRI_vocbase_t* _vocbase;
        std::string _collname;

    };

// -----------------------------------------------------------------------------
// --SECTION--                                     class EnumerateCollectionPlan
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @brief class LimitPlan, derived from ExecutionPlan
////////////////////////////////////////////////////////////////////////////////

    class LimitPlan : public ExecutionPlan {
      
////////////////////////////////////////////////////////////////////////////////
/// @brief constructors for various arguments, always with offset and limit
////////////////////////////////////////////////////////////////////////////////

      public:

        LimitPlan (size_t o, size_t l) 
          : ExecutionPlan(), _offset(o), _limit(l) {
        }

        LimitPlan (ExecutionPlan* ep, size_t o, size_t l) 
          : ExecutionPlan(ep), _offset(o), _limit(l) {
        }

        LimitPlan (size_t l) 
          : ExecutionPlan(), _offset(0), _limit(l) {
        }

        LimitPlan (ExecutionPlan* ep, size_t l) 
          : ExecutionPlan(ep), _offset(0), _limit(l) {
        }

////////////////////////////////////////////////////////////////////////////////
/// @brief return the type of the node
////////////////////////////////////////////////////////////////////////////////

        virtual NodeType getType () {
          return LIMIT;
        }

////////////////////////////////////////////////////////////////////////////////
/// @brief return the type of the node as a string
////////////////////////////////////////////////////////////////////////////////

        virtual std::string getTypeString () {
          return std::string("LimitPlan");
        }

////////////////////////////////////////////////////////////////////////////////
/// @brief export to JSON
////////////////////////////////////////////////////////////////////////////////

        virtual triagens::basics::Json toJson (
               TRI_memory_zone_t* zone = TRI_UNKNOWN_MEM_ZONE);

// -----------------------------------------------------------------------------
// --SECTION--                                                 private variables
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @brief we need to know the offset and limit
////////////////////////////////////////////////////////////////////////////////

      private:

        size_t _offset;
        size_t _limit;

    };
  }   // namespace triagens::aql
}  // namespace triagens

#endif

// Local Variables:
// mode: outline-minor
// outline-regexp: "^\\(/// @brief\\|/// {@inheritDoc}\\|/// @addtogroup\\|// --SECTION--\\|/// @\\}\\)"
// End:


