#pragma once
#include <sys/socket.h>
#ifndef AF_UNIX
#define AF_UNIX  1
#endif
#ifndef PF_UNIX
#define PF_UNIX  AF_UNIX
#endif
#define UNIX_PATH_MAX 108
struct sockaddr_un {
    sa_family_t sun_family;
    char        sun_path[UNIX_PATH_MAX];
};
