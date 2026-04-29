// ESP-IDF-specifika implementationer som ersatter POSIX-beroenden i Utils.cpp
#include "esp_random.h"
#include <stdint.h>
#include <stddef.h>

// ZeroTier anropar getSecureRandom() fran Utils.cpp
// ESP-IDF erbjuder esp_fill_random() som anvander hardware RNG
extern "C" void zt_esp_fill_random(void *buf, size_t len)
{
    esp_fill_random(buf, len);
}
