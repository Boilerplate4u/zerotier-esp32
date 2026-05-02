// ESP32 ZeroTier storage wrapper — NVS + embedded test identity for QEMU
#include <string.h>
#include <stdio.h>
#include "nvs_flash.h"
#include "nvs.h"
#include "esp_log.h"
#include "ZeroTierOne.h"

static const char *TAG = "zt_storage";
static const char *NVS_NS = "zerotier";

// Pregenerated test identity for QEMU (generated 2026-04-28)
// Note: this identity is public and should only be used for testing
static const char *ZT_TEST_SECRET = "df429581f6:0:4951208caea03ceea21857eaae33a5d45e0f38cd30df9f64de8ea4d1b2ecb547e272188d5f1f7588e3698ecfaf7ac20751d0f184138c0487fb79ac56a5175c09:c908b106c6b0b77df781072e068c450555bed04a3a185b40b44c8c2e4124e6e6510b3d1876753a7a65c18f0ff04abac5592d8b75bb92ccc43bb55429190d5d3e";
static const char *ZT_TEST_PUBLIC = "df429581f6:0:4951208caea03ceea21857eaae33a5d45e0f38cd30df9f64de8ea4d1b2ecb547e272188d5f1f7588e3698ecfaf7ac20751d0f184138c0487fb79ac56a5175c09";
static bool s_identity_loaded = false;

static void make_key(enum ZT_StateObjectType type, const uint64_t id[2], char *out, size_t len) {
    snprintf(out, len, "zt%d_%08x", (int)type, (unsigned)(id ? id[0] : 0));
}

extern "C" void zt_state_put_impl(ZT_Node *node, void *uptr, void *tptr,
    enum ZT_StateObjectType type, const uint64_t id[2],
    const void *data, int len)
{
    if (len < 0) {
        nvs_handle_t h;
        if (nvs_open(NVS_NS, NVS_READWRITE, &h) == ESP_OK) {
            char key[16];
            make_key(type, id, key, sizeof(key));
            nvs_erase_key(h, key);
            nvs_commit(h);
            nvs_close(h);
        }
        return;
    }
    nvs_handle_t h;
    if (nvs_open(NVS_NS, NVS_READWRITE, &h) != ESP_OK) return;
    char key[16];
    make_key(type, id, key, sizeof(key));
    nvs_set_blob(h, key, data, (size_t)len);
    nvs_commit(h);
    nvs_close(h);
}

extern "C" int zt_state_get_impl(ZT_Node *node, void *uptr, void *tptr,
    enum ZT_StateObjectType type, const uint64_t id[2],
    void *data, unsigned int maxlen)
{
    // Try NVS first
    nvs_handle_t h;
    if (nvs_open(NVS_NS, NVS_READONLY, &h) == ESP_OK) {
        char key[16];
        make_key(type, id, key, sizeof(key));
        size_t actual = maxlen;
        esp_err_t err = nvs_get_blob(h, key, data, &actual);
        nvs_close(h);
        if (err == ESP_OK) return (int)actual;
    }

    // Fallback: embedded test identity for QEMU
    if (type == ZT_STATE_OBJECT_IDENTITY_SECRET) {
        size_t len = strlen(ZT_TEST_SECRET);
        if (len < maxlen) {
            memcpy(data, ZT_TEST_SECRET, len);
            ESP_LOGI(TAG, "Använder inbäddad testidentitet");
            return (int)len;
        }
    }
    if (type == ZT_STATE_OBJECT_IDENTITY_PUBLIC) {
        size_t len = strlen(ZT_TEST_PUBLIC);
        if (len < maxlen) {
            memcpy(data, ZT_TEST_PUBLIC, len);
            return (int)len;
        }
    }
    return -1;
}
