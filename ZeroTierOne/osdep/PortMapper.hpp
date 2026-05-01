/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * (c) ZeroTier, Inc.
 * https://www.zerotier.com/
 */

#ifdef ZT_USE_MINIUPNPC

#ifndef ZT_PORTMAPPER_HPP
#define ZT_PORTMAPPER_HPP

#include "../node/Constants.hpp"
#include "../node/InetAddress.hpp"
#include "../node/Mutex.hpp"
#include "Thread.hpp"

#include <vector>

/**
 * How frequently should we refresh our UPNP/NAT-PnP/whatever state?
 */
#define ZT_PORTMAPPER_REFRESH_DELAY 300000

namespace ZeroTier {

class PortMapperImpl;

/**
 * UPnP/NAT-PnP port mapping "daemon"
 */
class PortMapper {
	friend class PortMapperImpl;

  public:
	/**
	 * Create and start port mapper service
	 *
	 * @param localUdpPortToMap Port we want visible to the outside world
	 * @param name Unique name of this endpoint (based on ZeroTier address)
	 */
	PortMapper(int localUdpPortToMap, const char* uniqueName);

	~PortMapper();

	/**
	 * @return All current external mappings for our port
	 */
	std::vector<InetAddress> get() const;

  private:
	PortMapperImpl* _impl;
};

}	// namespace ZeroTier

#endif

#endif	 // ZT_USE_MINIUPNPC
