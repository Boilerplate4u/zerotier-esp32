#include "Utils.hpp"
#include "esp_random.h"

namespace ZeroTier {

void Utils::getSecureRandom(void* buf, unsigned int bytes)
{
    esp_fill_random(buf, bytes);
}

} // namespace ZeroTier
