/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * (c) ZeroTier, Inc.
 * https://www.zerotier.com/
 */

#pragma once

#if defined(_WIN32) || defined(_WIN64)

#include "../../node/Mutex.hpp"
#include "../../osdep/Thread.hpp"
#include "../../service/OneService.hpp"
#include "ServiceBase.h"

#include <stdio.h>
#include <string>

// Uncomment to make debugging Windows services suck slightly less hard.
// #define ZT_DEBUG_SERVICE "C:\\ZeroTierOneServiceDebugLog.txt"

#ifdef ZT_DEBUG_SERVICE
extern FILE* SVCDBGfile;
extern ZeroTier::Mutex SVCDBGfile_m;
#define ZT_SVCDBG(f, ...)                                                                                                                                                                                                                      \
	{                                                                                                                                                                                                                                          \
		SVCDBGfile_m.lock();                                                                                                                                                                                                                   \
		fprintf(SVCDBGfile, f, ##__VA_ARGS__);                                                                                                                                                                                                 \
		fflush(SVCDBGfile);                                                                                                                                                                                                                    \
		SVCDBGfile_m.unlock();                                                                                                                                                                                                                 \
	}
#else
#define ZT_SVCDBG(f, ...)                                                                                                                                                                                                                      \
	{                                                                                                                                                                                                                                          \
	}
#endif

#define ZT_SERVICE_NAME			"ZeroTierOneService"
#define ZT_SERVICE_DISPLAY_NAME "ZeroTier One"
#define ZT_SERVICE_START_TYPE	SERVICE_AUTO_START
#define ZT_SERVICE_DEPENDENCIES ""
// #define ZT_SERVICE_ACCOUNT "NT AUTHORITY\\LocalService"
#define ZT_SERVICE_ACCOUNT	NULL
#define ZT_SERVICE_PASSWORD NULL

class ZeroTierOneService : public CServiceBase {
  public:
	ZeroTierOneService();
	virtual ~ZeroTierOneService(void);

	/**
	 * Thread main method; do not call elsewhere
	 */
	void threadMain() throw();

  protected:
	virtual void OnStart(DWORD dwArgc, PSTR* pszArgv);
	virtual void OnStop();
	virtual void OnShutdown();

  private:
	std::string _path;
	ZeroTier::OneService* volatile _service;
	ZeroTier::Mutex _lock;
	ZeroTier::Thread _thread;
};

#endif
