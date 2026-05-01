/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * (c) ZeroTier, Inc.
 * https://www.zerotier.com/
 */

#ifndef ZT_NEIGHBORDISCOVERY_HPP
#define ZT_NEIGHBORDISCOVERY_HPP

#include "../node/Hashtable.hpp"
#include "../node/InetAddress.hpp"
#include "../node/MAC.hpp"

#define ZT_ND_QUERY_INTERVAL 2000

#define ZT_ND_QUERY_MAX_TTL 5000

#define ZT_ND_EXPIRE 600000

namespace ZeroTier {

class NeighborDiscovery {
  public:
	NeighborDiscovery();

	/**
	 * Set a local IP entry that we should respond to Neighbor Requests withPrefix64k
	 *
	 * @param mac Our local MAC address
	 * @param ip Our IPv6 address
	 */
	void addLocal(const sockaddr_storage& address, const MAC& mac);

	/**
	 * Delete a local IP entry or cached Neighbor entry
	 *
	 * @param address IPv6 address to remove
	 */
	void remove(const sockaddr_storage& address);

	sockaddr_storage processIncomingND(const uint8_t* nd, unsigned int len, const sockaddr_storage& localIp, uint8_t* response, unsigned int& responseLen, MAC& responseDest);

	MAC query(const MAC& localMac, const sockaddr_storage& localIp, const sockaddr_storage& targetIp, uint8_t* query, unsigned int& queryLen, MAC& queryDest);

  private:
	struct _NDEntry {
		_NDEntry() : lastQuerySent(0), lastResponseReceived(0), mac(), local(false)
		{
		}
		uint64_t lastQuerySent;
		uint64_t lastResponseReceived;
		MAC mac;
		bool local;
	};

	Hashtable<InetAddress, _NDEntry> _cache;
	uint64_t _lastCleaned;
};

}	// namespace ZeroTier

#endif
