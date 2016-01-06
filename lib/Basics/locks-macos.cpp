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
////////////////////////////////////////////////////////////////////////////////

#include "locks.h"


#ifdef TRI_HAVE_MACOS_SPIN


////////////////////////////////////////////////////////////////////////////////
/// @brief initializes a new spin-lock
////////////////////////////////////////////////////////////////////////////////

#ifndef TRI_FAKE_SPIN_LOCKS
void TRI_InitSpin(TRI_spin_t* spinLock) { *spinLock = 0; }
#else
void locks_macos_cpp_MusntBeEmpty(void) {}
#endif

////////////////////////////////////////////////////////////////////////////////
/// @brief destroys a spin-lock
////////////////////////////////////////////////////////////////////////////////

#ifndef TRI_FAKE_SPIN_LOCKS
void TRI_DestroySpin(TRI_spin_t* spinLock) {}
#endif


////////////////////////////////////////////////////////////////////////////////
/// @brief locks spin-lock
////////////////////////////////////////////////////////////////////////////////

#ifndef TRI_FAKE_SPIN_LOCKS
void TRI_LockSpin(TRI_spin_t* spinLock) { OSSpinLockLock(spinLock); }
#endif

////////////////////////////////////////////////////////////////////////////////
/// @brief unlocks spin-lock
////////////////////////////////////////////////////////////////////////////////

#ifndef TRI_FAKE_SPIN_LOCKS
void TRI_UnlockSpin(TRI_spin_t* spinLock) { OSSpinLockUnlock(spinLock); }
#endif

#endif


