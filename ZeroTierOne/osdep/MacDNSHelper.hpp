/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * (c) ZeroTier, Inc.
 * https://www.zerotier.com/
 */

#ifndef MAC_DNS_HELPER
#define MAC_DNS_HELPER

#include "../node/InetAddress.hpp"
#include "../node/MAC.hpp"

#include <vector>

namespace ZeroTier {

class MacDNSHelper {
  public:
	static void setDNS(uint64_t nwid, const char* domain, const std::vector<InetAddress>& servers);
	static void removeDNS(uint64_t nwid);
	static bool addIps4(uint64_t nwid, const MAC mac, const char* dev, const std::vector<InetAddress>& addrs);
	static bool addIps6(uint64_t nwid, const MAC mac, const char* dev, const std::vector<InetAddress>& addrs);
	static bool removeIps4(uint64_t nwid);
	static bool removeIps6(uint64_t nwid);
};

}	// namespace ZeroTier

#endif
