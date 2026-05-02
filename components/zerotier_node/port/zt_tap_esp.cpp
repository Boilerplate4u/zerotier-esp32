// ESP32 ZeroTier TAP wrapper — Ethernet frames via esp_netif
// Phase 2: minimal implementation that logs frames, no actual netif yet
#include <stdio.h>
#include "esp_log.h"
#include "ZeroTierOne.h"

static const char *TAG = "zt_tap";

extern "C" void zt_virtual_frame_impl(
    ZT_Node *node, void *uptr, void *tptr,
    uint64_t nwid, void **nuptr,
    uint64_t sourceMac, uint64_t destMac,
    unsigned int etherType, unsigned int vlanId,
    const void *frameData, unsigned int frameLen)
{
    // Phase 2: log incoming frames - phase 3 connects to esp_netif
    ESP_LOGD(TAG, "frame nwid=%llx src=%llx dst=%llx type=0x%04x len=%u",
        nwid, sourceMac, destMac, etherType, frameLen);
}

extern "C" int zt_virtual_config_impl(
    ZT_Node *node, void *uptr, void *tptr,
    uint64_t nwid, void **nuptr,
    enum ZT_VirtualNetworkConfigOperation op,
    const ZT_VirtualNetworkConfig *conf)
{
    ESP_LOGI(TAG, "network config op=%d nwid=%llx", (int)op, nwid);
    return 0;
}
