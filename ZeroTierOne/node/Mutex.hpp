/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * (c) ZeroTier, Inc.
 * https://www.zerotier.com/
 */

#ifndef ZT_MUTEX_HPP
#define ZT_MUTEX_HPP

#include "Constants.hpp"

#if defined(ESP_PLATFORM)

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

namespace ZeroTier {

class Mutex {
  public:
	Mutex()  { _mh = xSemaphoreCreateMutex(); }
	~Mutex() { vSemaphoreDelete(_mh); }

	inline void lock()   const { xSemaphoreTake(const_cast<Mutex*>(this)->_mh, portMAX_DELAY); }
	inline void unlock() const { xSemaphoreGive(const_cast<Mutex*>(this)->_mh); }

	class Lock {
	  public:
		Lock(Mutex &m)       : _m(&m)                    { m.lock(); }
		Lock(const Mutex &m) : _m(const_cast<Mutex*>(&m)) { _m->lock(); }
		~Lock() { _m->unlock(); }
	  private:
		Mutex* const _m;
	};

  private:
	Mutex(const Mutex&) {}
	const Mutex& operator=(const Mutex&) { return *this; }
	SemaphoreHandle_t _mh;
};

} // namespace ZeroTier

#elif defined(__UNIX_LIKE__)

#include <pthread.h>
#include <stdlib.h>

namespace ZeroTier {

class Mutex {
  public:
	Mutex()  { pthread_mutex_init(&_mh, (const pthread_mutexattr_t*)0); }
	~Mutex() { pthread_mutex_destroy(&_mh); }

	inline void lock()   const { pthread_mutex_lock(&((const_cast<Mutex*>(this))->_mh)); }
	inline void unlock() const { pthread_mutex_unlock(&((const_cast<Mutex*>(this))->_mh)); }

	class Lock {
	  public:
		Lock(Mutex& m) : _m(&m) { m.lock(); }
		Lock(const Mutex& m) : _m(const_cast<Mutex*>(&m)) { _m->lock(); }
		~Lock() { _m->unlock(); }
	  private:
		Mutex* const _m;
	};

  private:
	Mutex(const Mutex&) {}
	const Mutex& operator=(const Mutex&) { return *this; }
	pthread_mutex_t _mh;
};

} // namespace ZeroTier

#elif defined(__WINDOWS__)

#include <stdlib.h>
#include <windows.h>

namespace ZeroTier {

class Mutex {
  public:
	Mutex()  { InitializeCriticalSection(&_cs); }
	~Mutex() { DeleteCriticalSection(&_cs); }

	inline void lock()         { EnterCriticalSection(&_cs); }
	inline void unlock()       { LeaveCriticalSection(&_cs); }
	inline void lock()   const { (const_cast<Mutex*>(this))->lock(); }
	inline void unlock() const { (const_cast<Mutex*>(this))->unlock(); }

	class Lock {
	  public:
		Lock(Mutex& m) : _m(&m) { m.lock(); }
		Lock(const Mutex& m) : _m(const_cast<Mutex*>(&m)) { _m->lock(); }
		~Lock() { _m->unlock(); }
	  private:
		Mutex* const _m;
	};

  private:
	Mutex(const Mutex&) {}
	const Mutex& operator=(const Mutex&) { return *this; }
	CRITICAL_SECTION _cs;
};

} // namespace ZeroTier

#endif

#endif
