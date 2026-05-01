/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * (c) ZeroTier, Inc.
 * https://www.zerotier.com/
 */

#ifndef ZT_BSDETHERNETTAP_HPP
#define ZT_BSDETHERNETTAP_HPP

#include "../node/Constants.hpp"
#include "../node/MAC.hpp"
#include "../node/MulticastGroup.hpp"
#include "EthernetTap.hpp"
#include "Thread.hpp"

#include <stdexcept>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <thread>
#include <vector>

namespace ZeroTier {

class BSDEthernetTap : public EthernetTap {
  public:
	BSDEthernetTap(
		const char* homePath,
		unsigned int concurrency,
		bool pinning,
		const MAC& mac,
		unsigned int mtu,
		unsigned int metric,
		uint64_t nwid,
		const char* friendlyName,
		void (*handler)(void*, void*, uint64_t, const MAC&, const MAC&, unsigned int, unsigned int, const void*, unsigned int),
		void* arg);

	virtual ~BSDEthernetTap();

	virtual void setEnabled(bool en);
	virtual bool enabled() const;
	virtual bool addIp(const InetAddress& ip);
	virtual bool removeIp(const InetAddress& ip);
	virtual std::vector<InetAddress> ips() const;
	virtual void put(const MAC& from, const MAC& to, unsigned int etherType, const void* data, unsigned int len);
	virtual std::string deviceName() const;
	virtual void setFriendlyName(const char* friendlyName);
	virtual void scanMulticastGroups(std::vector<MulticastGroup>& added, std::vector<MulticastGroup>& removed);
	virtual void setMtu(unsigned int mtu);
	virtual void setDns(const char* domain, const std::vector<InetAddress>& servers)
	{
	}

	void threadMain() throw();

  private:
	void (*_handler)(void*, void*, uint64_t, const MAC&, const MAC&, unsigned int, unsigned int, const void*, unsigned int);
	void* _arg;
	unsigned int _concurrency;
	bool _pinning;
	uint64_t _nwid;
	Thread _thread;
	std::string _dev;
	std::vector<MulticastGroup> _multicastGroups;
	unsigned int _mtu;
	unsigned int _metric;
	int _fd;
	int _shutdownSignalPipe[2];
	volatile bool _enabled;
	mutable std::vector<InetAddress> _ifaddrs;
	mutable uint64_t _lastIfAddrsUpdate;
	std::vector<std::thread> _rxThreads;
};

}	// namespace ZeroTier

#endif
