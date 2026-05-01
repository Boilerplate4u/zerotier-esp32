#include "OSUtils.hpp"

#include <cstdarg>
#include <cstdio>

namespace ZeroTier {

unsigned int OSUtils::ztsnprintf(char* buf, unsigned int len, const char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    int r = vsnprintf(buf, len, fmt, ap);
    va_end(ap);
    if (r < 0 || (unsigned int)r >= len)
        buf[len - 1] = '\0';
    return (r > 0) ? (unsigned int)r : 0;
}

std::string OSUtils::networkIDStr(const uint64_t nwid)
{
    char buf[17];
    snprintf(buf, sizeof(buf), "%016llx", (unsigned long long)nwid);
    return std::string(buf);
}

std::string OSUtils::nodeIDStr(const uint64_t nid)
{
    char buf[11];
    snprintf(buf, sizeof(buf), "%010llx", (unsigned long long)nid);
    return std::string(buf);
}

} // namespace ZeroTier
