#pragma once
// Stub for ifaddrs.h -- ESP32 port, Binder.hpp is excluded from build
// but Phy.hpp drags it in via include. Provide minimal struct to satisfy parser.

#include <sys/socket.h>

struct ifaddrs {
    struct ifaddrs  *ifa_next;
    char            *ifa_name;
    unsigned int     ifa_flags;
    struct sockaddr *ifa_addr;
    struct sockaddr *ifa_netmask;
    struct sockaddr *ifa_broadaddr;
    void            *ifa_data;
};

static inline int getifaddrs(struct ifaddrs **ifap) { if(ifap) *ifap = nullptr; return 0; }
static inline void freeifaddrs(struct ifaddrs *ifa) {}
