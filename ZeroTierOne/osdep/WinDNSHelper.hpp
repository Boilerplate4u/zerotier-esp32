/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * (c) ZeroTier, Inc.
 * https://www.zerotier.com/
 */

#ifndef WIN_DNS_HELPER_H_
#define WIN_DNS_HELPER_H_

#include "../node/InetAddress.hpp"

#include <cstdint>
#include <vector>

namespace ZeroTier {

class WinDNSHelper {
  public:
	static void setDNS(uint64_t nwid, const char* domain, const std::vector<InetAddress>& servers);
	static void removeDNS(uint64_t nwid);

  private:
	static std::pair<bool, std::string> hasDNSConfig(uint64_t nwid);
};

}	// namespace ZeroTier

#endif
