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

#ifndef LIB_BASICS_SPIN_LOCKER_H
#define LIB_BASICS_SPIN_LOCKER_H 1

#include "Basics/Common.h"
#include "Basics/SpinLock.h"


////////////////////////////////////////////////////////////////////////////////
/// @brief construct locker with file and line information
///
/// Ones needs to use macros twice to get a unique variable based on the line
/// number.
////////////////////////////////////////////////////////////////////////////////

#define SPIN_LOCKER_VAR_A(a) _spin_lock_variable_##a
#define SPIN_LOCKER_VAR_B(a) SPIN_LOCKER_VAR_A(a)

#ifdef TRI_SHOW_LOCK_TIME

#define SPIN_LOCKER(b)                                                   \
  triagens::basics::SpinLocker SPIN_LOCKER_VAR_B(__LINE__)(&b, __FILE__, \
                                                           __LINE__)

#else

#define SPIN_LOCKER(b) \
  triagens::basics::SpinLocker SPIN_LOCKER_VAR_B(__LINE__)(&b)

#endif


namespace triagens {
namespace basics {

////////////////////////////////////////////////////////////////////////////////
/// @brief spin locker
///
/// A SpinLocker locks a spinlock during its lifetime und unlocks the spinlock
/// when it is destroyed.
////////////////////////////////////////////////////////////////////////////////

class SpinLocker {
  SpinLocker(SpinLocker const&);
  SpinLocker& operator=(SpinLocker const&);

  
 public:
////////////////////////////////////////////////////////////////////////////////
/// @brief aquires a lock
///
/// The constructor aquires a lock, the destructors releases the lock.
////////////////////////////////////////////////////////////////////////////////

#ifdef TRI_SHOW_LOCK_TIME

  SpinLocker(SpinLock*, char const* file, int line);

#else

  explicit SpinLocker(SpinLock*);

#endif

  ////////////////////////////////////////////////////////////////////////////////
  /// @brief releases the lock
  ////////////////////////////////////////////////////////////////////////////////

  ~SpinLocker();

  
 private:
  ////////////////////////////////////////////////////////////////////////////////
  /// @brief the mutex
  ////////////////////////////////////////////////////////////////////////////////

  SpinLock* _lock;

#ifdef TRI_SHOW_LOCK_TIME

  ////////////////////////////////////////////////////////////////////////////////
  /// @brief file
  ////////////////////////////////////////////////////////////////////////////////

  char const* _file;

  ////////////////////////////////////////////////////////////////////////////////
  /// @brief line number
  ////////////////////////////////////////////////////////////////////////////////

  int _line;

  ////////////////////////////////////////////////////////////////////////////////
  /// @brief lock time
  ////////////////////////////////////////////////////////////////////////////////

  double _time;

#endif
};
}
}

#endif


