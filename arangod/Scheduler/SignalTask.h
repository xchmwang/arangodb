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
/// @author Dr. Frank Celler
/// @author Achim Brandt
////////////////////////////////////////////////////////////////////////////////

#ifndef ARANGOD_SCHEDULER_SIGNAL_TASK_H
#define ARANGOD_SCHEDULER_SIGNAL_TASK_H 1

#include "Basics/Common.h"

#include "Scheduler/Task.h"

#include "Basics/Mutex.h"

namespace triagens {
namespace rest {

////////////////////////////////////////////////////////////////////////////////
/// @ingroup Scheduler
/// @brief task used to handle signals
////////////////////////////////////////////////////////////////////////////////

class SignalTask : virtual public Task {
 public:
  ////////////////////////////////////////////////////////////////////////////////
  /// @brief maximal number of signals per task
  ////////////////////////////////////////////////////////////////////////////////

  static size_t const MAX_SIGNALS = 10;

 public:
  ////////////////////////////////////////////////////////////////////////////////
  /// @brief constructs a new task
  ////////////////////////////////////////////////////////////////////////////////

  SignalTask();

 public:
  ////////////////////////////////////////////////////////////////////////////////
  /// @brief adds a signal which will be handled
  ///
  /// Note that this method can be called on a signal task, which has already
  /// been set up.
  ////////////////////////////////////////////////////////////////////////////////

  bool addSignal(int signal);

 protected:
  ////////////////////////////////////////////////////////////////////////////////
  /// @brief called when the signal is received
  ////////////////////////////////////////////////////////////////////////////////

  virtual bool handleSignal() = 0;

 protected:
  ////////////////////////////////////////////////////////////////////////////////
  /// destructor
  ////////////////////////////////////////////////////////////////////////////////

  ~SignalTask();

 protected:
  ////////////////////////////////////////////////////////////////////////////////
  /// {@inheritDoc}
  ////////////////////////////////////////////////////////////////////////////////

  bool setup(Scheduler*, EventLoop) override;

  ////////////////////////////////////////////////////////////////////////////////
  /// {@inheritDoc}
  ////////////////////////////////////////////////////////////////////////////////

  void cleanup() override;

  ////////////////////////////////////////////////////////////////////////////////
  /// {@inheritDoc}
  ////////////////////////////////////////////////////////////////////////////////

  bool handleEvent(EventToken, EventType) override;

  ////////////////////////////////////////////////////////////////////////////////
  /// {@inheritDoc}
  ////////////////////////////////////////////////////////////////////////////////

  bool needsMainEventLoop() const override;

 protected:
  ////////////////////////////////////////////////////////////////////////////////
  /// @brief signal event
  ////////////////////////////////////////////////////////////////////////////////

  EventToken _watchers[MAX_SIGNALS];

 private:
  std::set<int> _signals;
  basics::Mutex _changeLock;
};
}
}

#endif

