/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * (c) ZeroTier, Inc.
 * https://www.zerotier.com/
 */

#ifndef WIN_FW_HELPER_H_
#define WIN_FW_HELPER_H_

#include "../node/InetAddress.hpp"

#include <cstdint>
#include <vector>

namespace ZeroTier {

class WinFWHelper {
  public:
	static void newICMPRule(const InetAddress& ip, uint64_t nwid);
	static void removeICMPRule(const InetAddress& ip, uint64_t nwid);
	static void removeICMPRules(uint64_t nwid);
	static void removeICMPRules();

  private:
	static void _run(std::string cmd);
	static void newICMPv4Rule(std::string address, uint64_t nwid);
	static void newICMPv6Rule(std::string address, uint64_t nwid);
	static void removeICMPv4Rule(std::string address, uint64_t nwid);
	static void removeICMPv6Rule(std::string address, uint64_t nwid);
	static void removeICMPv4Rules(uint64_t nwid);
	static void removeICMPv6Rules(uint64_t nwid);
};

}	// namespace ZeroTier

#endif
