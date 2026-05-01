/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * (c) ZeroTier, Inc.
 * https://www.zerotier.com/
 */

#include "CertificateOfOwnership.hpp"

#include "Identity.hpp"
#include "Network.hpp"
#include "Node.hpp"
#include "RuntimeEnvironment.hpp"
#include "Switch.hpp"
#include "Topology.hpp"

namespace ZeroTier {

int CertificateOfOwnership::verify(const RuntimeEnvironment* RR, void* tPtr) const
{
	if ((! _signedBy) || (_signedBy != Network::controllerFor(_networkId))) {
		return -1;
	}
	const Identity id(RR->topology->getIdentity(tPtr, _signedBy));
	if (! id) {
		RR->sw->requestWhois(tPtr, RR->node->now(), _signedBy);
		return 1;
	}
	try {
		Buffer<(sizeof(CertificateOfOwnership) + 64)> tmp;
		this->serialize(tmp, true);
		return (id.verify(tmp.data(), tmp.size(), _signature) ? 0 : -1);
	}
	catch (...) {
		return -1;
	}
}

bool CertificateOfOwnership::_owns(const CertificateOfOwnership::Thing& t, const void* v, unsigned int l) const
{
	for (unsigned int i = 0, j = _thingCount; i < j; ++i) {
		if (_thingTypes[i] == (uint8_t)t) {
			unsigned int k = 0;
			while (k < l) {
				if (reinterpret_cast<const uint8_t*>(v)[k] != _thingValues[i][k]) {
					break;
				}
				++k;
			}
			if (k == l) {
				return true;
			}
		}
	}
	return false;
}

}	// namespace ZeroTier
