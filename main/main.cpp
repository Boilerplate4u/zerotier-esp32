#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "ZeroTierOne.h"

static const char *TAG = "zt_main";

// Declarations from port files
extern "C" int  zt_state_get_impl(ZT_Node*, void*, void*, enum ZT_StateObjectType, const uint64_t*, void*, unsigned int);
extern "C" void zt_state_put_impl(ZT_Node*, void*, void*, enum ZT_StateObjectType, const uint64_t*, const void*, int);
extern "C" int  zt_wire_send_impl(ZT_Node*, void*, void*, int64_t, const struct sockaddr_storage*, const void*, unsigned int, unsigned int);
extern "C" void zt_virtual_frame_impl(ZT_Node*, void*, void*, uint64_t, void**, uint64_t, uint64_t, unsigned int, unsigned int, const void*, unsigned int);
extern "C" int  zt_virtual_config_impl(ZT_Node*, void*, void*, uint64_t, void**, enum ZT_VirtualNetworkConfigOperation, const ZT_VirtualNetworkConfig*);

extern "C" int  zt_socket_init(void);
extern "C" void zt_socket_start_recv(ZT_Node *node, void *uptr);

static int zt_path_check(ZT_Node *node, void *uptr, void *tptr,
    uint64_t ztaddr, int64_t localSocket,
    const struct sockaddr_storage *addr) { return 1; }

static int zt_path_lookup(ZT_Node *node, void *uptr, void *tptr,
    uint64_t ztaddr, int family,
    struct sockaddr_storage *result) { return 0; }

static void zt_event(ZT_Node *node, void *uptr, void *tptr,
    enum ZT_Event event, const void *data)
{
    ESP_LOGI(TAG, "ZT event: %d", (int)event);
    if (event == ZT_EVENT_ONLINE) {
        ESP_LOGI(TAG, "ZeroTier node är online!");
    }
}

static void zt_main_task(void *arg)
{
    // Initialize NVS
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        nvs_flash_erase();
        nvs_flash_init();
    }

    // Measure DIRAM before ZT_Node_new
    size_t diram_before = heap_caps_get_free_size(MALLOC_CAP_INTERNAL);
    size_t psram_before = heap_caps_get_free_size(MALLOC_CAP_SPIRAM);
    ESP_LOGI(TAG, "DIRAM ledigt före ZT_Node_new: %u bytes", (unsigned)diram_before);
    ESP_LOGI(TAG, "PSRAM ledigt före ZT_Node_new: %u bytes", (unsigned)psram_before);

    // Initialize UDP socket
    if (zt_socket_init() != 0) {
        ESP_LOGE(TAG, "zt_socket_init misslyckades");
        vTaskDelete(nullptr);
        return;
    }

    // Set up callbacks
    ZT_Node_Callbacks cb;
    memset(&cb, 0, sizeof(cb));
    cb.version                      = 0;
    cb.statePutFunction             = zt_state_put_impl;
    cb.stateGetFunction             = zt_state_get_impl;
    cb.wirePacketSendFunction       = zt_wire_send_impl;
    cb.virtualNetworkFrameFunction  = zt_virtual_frame_impl;
    cb.virtualNetworkConfigFunction = zt_virtual_config_impl;
    cb.eventCallback                = zt_event;
    cb.pathCheckFunction            = zt_path_check;
    cb.pathLookupFunction           = zt_path_lookup;

    // Create ZeroTier node
    ZT_Node *node = nullptr;
    int64_t now = (int64_t)(esp_timer_get_time() / 1000ULL);
    ZT_ResultCode rc = ZT_Node_new(&node, nullptr, nullptr, nullptr, &cb, now);

    size_t diram_after = heap_caps_get_free_size(MALLOC_CAP_INTERNAL);
    size_t psram_after = heap_caps_get_free_size(MALLOC_CAP_SPIRAM);
    ESP_LOGI(TAG, "ZT_Node_new rc=%d node=%p", (int)rc, (void*)node);
    ESP_LOGI(TAG, "DIRAM förbrukat av ZT_Node_new: %d bytes", (int)(diram_before - diram_after));
    ESP_LOGI(TAG, "PSRAM förbrukat av ZT_Node_new: %d bytes", (int)(psram_before - psram_after));

    if (rc != ZT_RESULT_OK || !node) {
        ESP_LOGE(TAG, "ZT_Node_new misslyckades");
        vTaskDelete(nullptr);
        return;
    }

    // Start receive task
    zt_socket_start_recv(node, nullptr);

    // Main loop: run ZT_Node_processBackgroundTasks every 100ms
    while (1) {
        now = (int64_t)(esp_timer_get_time() / 1000ULL);
        int64_t next_run = now + 100;
        ZT_Node_processBackgroundTasks(node, nullptr, now, &next_run);
        int64_t delay_ms = next_run - (int64_t)(esp_timer_get_time() / 1000ULL);
        if (delay_ms < 1) delay_ms = 1;
        if (delay_ms > 100) delay_ms = 100;
        vTaskDelay(pdMS_TO_TICKS(delay_ms));
    }
}

extern "C" void app_main(void)
{
    ESP_LOGI(TAG, "ZeroTier ESP32-port - fas 2");
    xTaskCreate(zt_main_task, "zt_main", 8192, nullptr, 5, nullptr);
}
