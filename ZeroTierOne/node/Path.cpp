/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * (c) ZeroTier, Inc.
 * https://www.zerotier.com/
 */

#include "Path.hpp"

#include "Node.hpp"
#include "RuntimeEnvironment.hpp"

namespace ZeroTier {

bool Path::send(const RuntimeEnvironment* RR, void* tPtr, const void* data, unsigned int len, int64_t now)
{
	if (RR->node->putPacket(tPtr, _localSocket, _addr, data, len)) {
		_lastOut = now;
		return true;
	}
	return false;
}

}	// namespace ZeroTier
