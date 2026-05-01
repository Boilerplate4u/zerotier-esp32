/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * (c) ZeroTier, Inc.
 * https://www.zerotier.com/
 */

/**
 * @file
 *
 * Debug macros
 */

#ifndef ZT_DEBUG_H
#define ZT_DEBUG_H

#if defined(__linux__) || defined(__APPLE__)
#include <pthread.h>
#include <sys/syscall.h>
#include <unistd.h>
#endif

#include <string.h>

#define ZT_MSG_INFO true
#define ZT_COLOR	true

// Debug output colors
#if defined(__APPLE__)
#include "TargetConditionals.h"
#endif
#if defined(ZT_COLOR) && ! defined(_WIN32) && ! defined(__ANDROID__) && ! defined(TARGET_OS_IPHONE) && ! defined(TARGET_IPHONE_SIMULATOR) && ! defined(__APP_FRAMEWORK__)
#define ZT_RED	 "\x1B[31m"
#define ZT_GRN	 "\x1B[32m"
#define ZT_YEL	 "\x1B[33m"
#define ZT_BLU	 "\x1B[34m"
#define ZT_MAG	 "\x1B[35m"
#define ZT_CYN	 "\x1B[36m"
#define ZT_WHT	 "\x1B[37m"
#define ZT_RESET "\x1B[0m"
#else
#define ZT_RED
#define ZT_GRN
#define ZT_YEL
#define ZT_BLU
#define ZT_MAG
#define ZT_CYN
#define ZT_WHT
#define ZT_RESET
#endif

#define ZT_FILENAME (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)   // short

#ifdef __linux__
#define ZT_THREAD_ID (long)0   // syscall(SYS_gettid)
#endif
#ifdef __APPLE__
#define ZT_THREAD_ID (long)0   // (long)gettid()
#endif
#ifdef __FreeBSD__
#define ZT_THREAD_ID (long)0   // (long)gettid()
#endif
#ifdef _WIN32
#define ZT_THREAD_ID (long)0   //
#endif
#if defined(__JNI_LIB__)
#include <jni.h>
#endif
#if defined(__ANDROID__)
#include <android/log.h>
#define ZT_LOG_TAG "ZTSDK"
#endif
#if defined(ZT_DEBUG_TRACE)
#if ZT_MSG_INFO == true
#if defined(__ANDROID__)
#define DEBUG_INFO(fmt, args...) ((void)__android_log_print(ANDROID_LOG_VERBOSE, ZT_LOG_TAG, "INFO : %17s:%5d:%20s: " fmt "\n", ZT_FILENAME, __LINE__, __FUNCTION__, ##args))
#endif
#if defined(_WIN32)
#define DEBUG_INFO(fmt, ...) fprintf(stderr, ZT_GRN "INFO [%ld]: %17s:%5d:%25s: " fmt "\n" ZT_RESET, ZT_THREAD_ID, ZT_FILENAME, __LINE__, __FUNCTION__, __VA_ARGS__)
#endif
#if defined(__linux__) or defined(__APPLE__) or defined(__FreeBSD__)
#define DEBUG_INFO(fmt, args...) fprintf(stderr, ZT_GRN "INFO [%ld]: %17s:%5d:%25s: " fmt "\n" ZT_RESET, ZT_THREAD_ID, ZT_FILENAME, __LINE__, __FUNCTION__, ##args)
#endif
#else
#define DEBUG_INFO(fmt, args...)
#endif
#else	// blank
#if defined(_WIN32)
#define DEBUG_INFO(...)
#else
#define DEBUG_INFO(fmt, args...)
#endif
#endif

#endif	 // _H
