/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * (c) ZeroTier, Inc.
 * https://www.zerotier.com/
 */

#ifndef ZT_POLY1305_HPP
#define ZT_POLY1305_HPP

namespace ZeroTier {

#define ZT_POLY1305_KEY_LEN 32
#define ZT_POLY1305_MAC_LEN 16

/**
 * Poly1305 one-time authentication code
 *
 * This takes a one-time-use 32-byte key and generates a 16-byte message
 * authentication code. The key must never be re-used for a different
 * message.
 *
 * In Packet this is done by using the first 32 bytes of the stream cipher
 * keystream as a one-time-use key. These 32 bytes are then discarded and
 * the packet is encrypted with the next N bytes.
 */
class Poly1305 {
  public:
	/**
	 * Compute a one-time authentication code
	 *
	 * @param auth Buffer to receive code -- MUST be 16 bytes in length
	 * @param data Data to authenticate
	 * @param len Length of data to authenticate in bytes
	 * @param key 32-byte one-time use key to authenticate data (must not be reused)
	 */
	static void compute(void* auth, const void* data, unsigned int len, const void* key);
};

}	// namespace ZeroTier

#endif
