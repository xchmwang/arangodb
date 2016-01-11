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

#ifndef ARANGOD_UTILS_V8_TRANSACTION_CONTEXT_H
#define ARANGOD_UTILS_V8_TRANSACTION_CONTEXT_H 1

#include "Basics/Common.h"
#include "Utils/TransactionContext.h"

#include <velocypack/Options.h>
#include <velocypack/velocypack-aliases.h>

struct TRI_transaction_s;

namespace triagens {
namespace arango {

class V8TransactionContext final : public TransactionContext {
  
  
 public:
  ////////////////////////////////////////////////////////////////////////////////
  /// @brief create the context
  ////////////////////////////////////////////////////////////////////////////////

  explicit V8TransactionContext(bool);

  ////////////////////////////////////////////////////////////////////////////////
  /// @brief destroy the context
  ////////////////////////////////////////////////////////////////////////////////

  ~V8TransactionContext();

  
 public:
  ////////////////////////////////////////////////////////////////////////////////
  /// @brief return the resolver
  ////////////////////////////////////////////////////////////////////////////////

  CollectionNameResolver const* getResolver() const override;

  ////////////////////////////////////////////////////////////////////////////////
  /// @brief return the vpack options
  ////////////////////////////////////////////////////////////////////////////////

  VPackOptions const* getVPackOptions() const override;

  ////////////////////////////////////////////////////////////////////////////////
  /// @brief get parent transaction (if any)
  ////////////////////////////////////////////////////////////////////////////////

  struct TRI_transaction_s* getParentTransaction() const override;

  ////////////////////////////////////////////////////////////////////////////////
  /// @brief register the transaction in the context
  ////////////////////////////////////////////////////////////////////////////////

  int registerTransaction(struct TRI_transaction_s* trx) override;

  ////////////////////////////////////////////////////////////////////////////////
  /// @brief unregister the transaction from the context
  ////////////////////////////////////////////////////////////////////////////////

  int unregisterTransaction() override;

  ////////////////////////////////////////////////////////////////////////////////
  /// @brief whether or not the transaction is embeddable
  ////////////////////////////////////////////////////////////////////////////////

  bool isEmbeddable() const override;

  ////////////////////////////////////////////////////////////////////////////////
  /// @brief make this transaction context a global context
  ////////////////////////////////////////////////////////////////////////////////

  void makeGlobal();

  ////////////////////////////////////////////////////////////////////////////////
  /// @brief delete the resolver from the context
  ////////////////////////////////////////////////////////////////////////////////

  void deleteResolver();

  ////////////////////////////////////////////////////////////////////////////////
  /// @brief whether or not the context contains a resolver
  ////////////////////////////////////////////////////////////////////////////////

  inline bool hasResolver() const { return _resolver != nullptr; }

  ////////////////////////////////////////////////////////////////////////////////
  /// @brief set a resolver in the context
  ////////////////////////////////////////////////////////////////////////////////

  inline void setResolver(CollectionNameResolver const* resolver) {
    TRI_ASSERT(_resolver == nullptr);
    TRI_ASSERT(resolver != nullptr);
    _resolver = resolver;
  }

  ////////////////////////////////////////////////////////////////////////////////
  /// @brief check whether the transaction is embedded
  ////////////////////////////////////////////////////////////////////////////////

  static bool IsEmbedded();

  
 private:
  ////////////////////////////////////////////////////////////////////////////////
  /// @brief the v8 thread-local "global" transaction context
  ////////////////////////////////////////////////////////////////////////////////

  V8TransactionContext* _sharedTransactionContext;

  ////////////////////////////////////////////////////////////////////////////////
  /// @brief the collection name resolver
  ////////////////////////////////////////////////////////////////////////////////

  CollectionNameResolver const* _resolver;

  ////////////////////////////////////////////////////////////////////////////////
  /// @brief the options for vpack
  ////////////////////////////////////////////////////////////////////////////////

  VPackOptions _options;

  ////////////////////////////////////////////////////////////////////////////////
  /// @brief the currently ongoing transaction
  ////////////////////////////////////////////////////////////////////////////////

  struct TRI_transaction_s* _currentTransaction;

  ////////////////////////////////////////////////////////////////////////////////
  /// @brief whether or not we are responsible for the resolver
  ////////////////////////////////////////////////////////////////////////////////

  bool _ownResolver;

  ////////////////////////////////////////////////////////////////////////////////
  /// @brief whether or not we are responsible for the vpack options
  ////////////////////////////////////////////////////////////////////////////////

  bool _ownOptions;

  ////////////////////////////////////////////////////////////////////////////////
  /// @brief whether or not further transactions can be embedded
  ////////////////////////////////////////////////////////////////////////////////

  bool const _embeddable;
};
}
}

#endif


