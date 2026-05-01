/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * (c) ZeroTier, Inc.
 * https://www.zerotier.com/
 */

#ifndef ZT_DNS_HPP
#define ZT_DNS_HPP
#include "../include/ZeroTierOne.h"
#include "Buffer.hpp"
#include "InetAddress.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

namespace ZeroTier {

/**
 * DNS data serialization methods
 */
class DNS {
  public:
	template <unsigned int C> static inline void serializeDNS(Buffer<C>& b, const ZT_VirtualNetworkDNS* dns)
	{
		b.append(dns->domain, 128);
		for (unsigned int j = 0; j < ZT_MAX_DNS_SERVERS; ++j) {
			InetAddress tmp(dns->server_addr[j]);
			tmp.serialize(b);
		}
	}

	template <unsigned int C> static inline void deserializeDNS(const Buffer<C>& b, unsigned int& p, ZT_VirtualNetworkDNS* dns)
	{
		char* d = (char*)b.data() + p;
		memset(dns, 0, sizeof(ZT_VirtualNetworkDNS));
		memcpy(dns->domain, d, 128);
		dns->domain[127] = 0;
		p += 128;
		for (unsigned int j = 0; j < ZT_MAX_DNS_SERVERS; ++j) {
			p += reinterpret_cast<InetAddress*>(&(dns->server_addr[j]))->deserialize(b, p);
		}
	}
};

}	// namespace ZeroTier

#endif	 // ZT_DNS_HPP
