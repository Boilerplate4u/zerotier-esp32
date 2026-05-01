/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * (c) ZeroTier, Inc.
 * https://www.zerotier.com/
 */

#ifndef ZT_ATOMICCOUNTER_HPP
#define ZT_ATOMICCOUNTER_HPP

#ifndef __GNUC__
#include <atomic>
#endif

namespace ZeroTier {

/**
 * Simple atomic counter supporting increment and decrement
 */
class AtomicCounter {
  public:
	AtomicCounter()
	{
		_v = 0;
	}

	inline int load() const
	{
#ifdef __GNUC__
		return __sync_or_and_fetch(const_cast<int*>(&_v), 0);
#else
		return _v.load();
#endif
	}

	inline int operator++()
	{
#ifdef __GNUC__
		return __sync_add_and_fetch(&_v, 1);
#else
		return ++_v;
#endif
	}

	inline int operator--()
	{
#ifdef __GNUC__
		return __sync_sub_and_fetch(&_v, 1);
#else
		return --_v;
#endif
	}

  private:
	AtomicCounter(const AtomicCounter&)
	{
	}
	const AtomicCounter& operator=(const AtomicCounter&)
	{
		return *this;
	}

#ifdef __GNUC__
	int _v;
#else
	std::atomic_int _v;
#endif
};

}	// namespace ZeroTier

#endif
