/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * (c) ZeroTier, Inc.
 * https://www.zerotier.com/
 */

#ifndef ZT_MACETHERNETTAPAGENT_H
#define ZT_MACETHERNETTAPAGENT_H

#define ZT_MACETHERNETTAPAGENT_EXIT_CODE_SUCCESS		  0
#define ZT_MACETHERNETTAPAGENT_EXIT_CODE_INVALID_REQUEST  -1
#define ZT_MACETHERNETTAPAGENT_EXIT_CODE_UNABLE_TO_CREATE -2
#define ZT_MACETHERNETTAPAGENT_EXIT_CODE_READ_ERROR		  -3

#define ZT_MACETHERNETTAPAGENT_STDIN_CMD_PACKET	  0
#define ZT_MACETHERNETTAPAGENT_STDIN_CMD_IFCONFIG 1
#define ZT_MACETHERNETTAPAGENT_STDIN_CMD_EXIT	  2

#define ZT_MACETHERNETTAPAGENT_DEFAULT_SYSTEM_PATH "/Library/Application Support/ZeroTier/One/MacEthernetTapAgent"

#endif
