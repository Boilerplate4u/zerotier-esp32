#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_eth.h"
#include "esp_eth_mac_openeth.h"
#include "esp_eth_phy.h"
#include "esp_event.h"
#include "esp_timer.h"
#include "esp_heap_caps.h"
#include "nvs_flash.h"
#include "lwip/sockets.h"
#include "lwip/netdb.h"

#include "ZeroTierOne.h"

static const char *TAG = "zt_qemu";
static EventGroupHandle_t s_eth_event_group;
#define ETH_GOT_IP_BIT BIT0

static int      s_udp_sock = -1;
static ZT_Node *s_node     = nullptr;

static StackType_t  s_rx_stack[32768];
static StaticTask_t s_rx_tcb;

static void on_got_ip(void *arg, esp_event_base_t base, int32_t id, void *data)
{
    ip_event_got_ip_t *ev = (ip_event_got_ip_t *)data;
    ESP_LOGI(TAG, "IP: " IPSTR, IP2STR(&ev->ip_info.ip));
    xEventGroupSetBits(s_eth_event_group, ETH_GOT_IP_BIT);
}

// ── ZeroTier callbacks ────────────────────────────────────────────────────────

static void zt_event_cb(ZT_Node *node, void *uptr, void *tptr,
                        enum ZT_Event event, const void *meta)
{
    ESP_LOGI(TAG, "ZT event: %d", (int)event);
    if (event == ZT_EVENT_ONLINE) {
        ZT_ResultCode rc = ZT_Node_join(node, 0xdb64858fed0b2566ULL, nullptr, nullptr);
        ESP_LOGI(TAG, "ZT_Node_join 0xdb64858fed0b2566 rc=%d", (int)rc);
        if (!heap_caps_check_integrity_all(false)) {
            ESP_LOGE(TAG, "HEAP CORRUPT after ZT_Node_join");
            heap_caps_check_integrity_all(true);
        }
    }
}

static void zt_state_put(ZT_Node *node, void *uptr, void *tptr,
                         enum ZT_StateObjectType type, const uint64_t id[2],
                         const void *data, int len)
{
    if (len < 0) return;

    if (type != ZT_STATE_OBJECT_IDENTITY_SECRET &&
        type != ZT_STATE_OBJECT_IDENTITY_PUBLIC  &&
        type != ZT_STATE_OBJECT_PLANET           &&
        type != ZT_STATE_OBJECT_PEER) {
        ESP_LOGD(TAG, "state_put type=%d: not persisted", (int)type);
        return;
    }

    char key[16];
    switch (type) {
        case ZT_STATE_OBJECT_IDENTITY_SECRET: snprintf(key, sizeof(key), "id_sec"); break;
        case ZT_STATE_OBJECT_IDENTITY_PUBLIC:  snprintf(key, sizeof(key), "id_pub"); break;
        case ZT_STATE_OBJECT_PLANET:           snprintf(key, sizeof(key), "planet"); break;
        case ZT_STATE_OBJECT_PEER:             snprintf(key, sizeof(key), "p_%08llx", (unsigned long long)(id[0] & 0xFFFFFFFFULL)); break;
        default: return;
    }

    nvs_handle_t h;
    esp_err_t err = nvs_open("zerotier", NVS_READWRITE, &h);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "state_put nvs_open failed: %s", esp_err_to_name(err));
        return;
    }
    err = nvs_set_blob(h, key, data, (size_t)len);
    if (err == ESP_OK) err = nvs_commit(h);
    nvs_close(h);

    if (err != ESP_OK)
        ESP_LOGE(TAG, "state_put type=%d nvs error: %s", (int)type, esp_err_to_name(err));
    else
        ESP_LOGI(TAG, "state_put type=%d key=%s len=%d: saved", (int)type, key, len);
}

static int zt_state_get(ZT_Node *node, void *uptr, void *tptr,
                        enum ZT_StateObjectType type, const uint64_t id[2],
                        void *data, unsigned int maxlen)
{
    if (type != ZT_STATE_OBJECT_IDENTITY_SECRET &&
        type != ZT_STATE_OBJECT_IDENTITY_PUBLIC  &&
        type != ZT_STATE_OBJECT_PLANET           &&
        type != ZT_STATE_OBJECT_PEER) {
        return -1;
    }

    char key[16];
    switch (type) {
        case ZT_STATE_OBJECT_IDENTITY_SECRET: snprintf(key, sizeof(key), "id_sec"); break;
        case ZT_STATE_OBJECT_IDENTITY_PUBLIC:  snprintf(key, sizeof(key), "id_pub"); break;
        case ZT_STATE_OBJECT_PLANET:           snprintf(key, sizeof(key), "planet"); break;
        case ZT_STATE_OBJECT_PEER:             snprintf(key, sizeof(key), "p_%08llx", (unsigned long long)(id[0] & 0xFFFFFFFFULL)); break;
        default: return -1;
    }

    nvs_handle_t h;
    esp_err_t err = nvs_open("zerotier", NVS_READONLY, &h);
    if (err == ESP_ERR_NVS_NOT_FOUND) return -1;
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "state_get nvs_open failed: %s", esp_err_to_name(err));
        return -1;
    }

    size_t sz = maxlen;
    err = nvs_get_blob(h, key, data, &sz);
    nvs_close(h);

    if (err == ESP_ERR_NVS_NOT_FOUND) return -1;
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "state_get type=%d nvs error: %s", (int)type, esp_err_to_name(err));
        return -1;
    }

    ESP_LOGI(TAG, "state_get type=%d key=%s: found %u bytes", (int)type, key, (unsigned)sz);
    return (int)sz;
}

static int zt_wire_send(ZT_Node *node, void *uptr, void *tptr,
                        int64_t localSocket,
                        const struct sockaddr_storage *addr,
                        const void *data, unsigned int len, unsigned int ttl)
{
    if (s_udp_sock < 0) return -1;
    if (addr->ss_family == AF_INET) {
        ssize_t sent = sendto(s_udp_sock, data, len, 0,
                              (const struct sockaddr *)addr,
                              sizeof(struct sockaddr_in));
        char _astr[INET_ADDRSTRLEN];
        const struct sockaddr_in *_s = (const struct sockaddr_in *)addr;
        inet_ntop(AF_INET, &_s->sin_addr, _astr, sizeof(_astr));
        ESP_LOGI(TAG, "wire_send -> %s:%u len=%u sent=%d", _astr, ntohs(_s->sin_port), len, (int)sent);
        return (sent == (ssize_t)len) ? 0 : -1;
    }
    return 0;  // AF_INET6: QEMU SLIRP is IPv4 only, drop silently
}

static void zt_vn_frame(ZT_Node *node, void *uptr, void *tptr,
                        uint64_t nwid, void **nuptr,
                        uint64_t srcMac, uint64_t destMac,
                        unsigned int etherType, unsigned int vlanId,
                        const void *frameData, unsigned int frameLen)
{
}

static int zt_vn_config(ZT_Node *node, void *uptr, void *tptr,
                        uint64_t nwid, void **nuptr,
                        enum ZT_VirtualNetworkConfigOperation op,
                        const ZT_VirtualNetworkConfig *nwconf)
{
    return 0;
}

// ── ZeroTier receive task ─────────────────────────────────────────────────────

static void zt_rx_task(void *pv)
{
    uint8_t buf[1500];
    struct sockaddr_storage from;
    socklen_t fromlen;
    volatile int64_t nextTask = 0;

    for (;;) {
        fromlen = sizeof(from);
        int n = recvfrom(s_udp_sock, buf, sizeof(buf), 0,
                         (struct sockaddr *)&from, &fromlen);
        { char _astr[INET_ADDRSTRLEN]; const struct sockaddr_in *_s = (const struct sockaddr_in *)&from;
          inet_ntop(AF_INET, &_s->sin_addr, _astr, sizeof(_astr));
          ESP_LOGI(TAG, "wire_recv <- %s:%u len=%d", _astr, ntohs(_s->sin_port), n); }
        if (n <= 0) continue;
        if (s_node == nullptr) continue;

        int64_t now = esp_timer_get_time() / 1000LL;
        ZT_Node_processWirePacket(s_node, nullptr, now, -1,
                                  &from, buf, (unsigned int)n, &nextTask);
        if (!heap_caps_check_integrity_all(false)) {
            ESP_LOGE(TAG, "HEAP CORRUPT after processWirePacket");
            heap_caps_check_integrity_all(true);
        }
        ESP_LOGI(TAG, "zt_rx_task stack HWM: %u bytes",
                 uxTaskGetStackHighWaterMark(NULL));
    }
}

// ── ZeroTier task ─────────────────────────────────────────────────────────────

static void zt_task(void *pv)
{
    xEventGroupWaitBits(s_eth_event_group, ETH_GOT_IP_BIT,
                        pdFALSE, pdTRUE, portMAX_DELAY);

    ESP_LOGI(TAG, "Free heap: %lu  largest block: %lu",
             (unsigned long)esp_get_free_heap_size(),
             (unsigned long)heap_caps_get_largest_free_block(MALLOC_CAP_8BIT));

    // a) Create and bind UDP socket on port 9993
    s_udp_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (s_udp_sock < 0) {
        ESP_LOGE(TAG, "UDP socket create failed errno=%d", errno);
        vTaskDelete(nullptr);
        return;
    }
    struct sockaddr_in local = {};
    local.sin_family      = AF_INET;
    local.sin_addr.s_addr = htonl(INADDR_ANY);
    local.sin_port        = htons(9993);
    if (bind(s_udp_sock, (struct sockaddr *)&local, sizeof(local)) < 0) {
        ESP_LOGE(TAG, "UDP bind failed errno=%d", errno);
        close(s_udp_sock);
        s_udp_sock = -1;
        vTaskDelete(nullptr);
        return;
    }
    ESP_LOGI(TAG, "UDP socket bound on :9993");

    // b) ZT_Node_new
    ZT_Node_Callbacks cb;
    memset(&cb, 0, sizeof(cb));
    cb.version                      = 0;
    cb.eventCallback                = zt_event_cb;
    cb.statePutFunction             = zt_state_put;
    cb.stateGetFunction             = zt_state_get;
    cb.wirePacketSendFunction       = zt_wire_send;
    cb.virtualNetworkFrameFunction  = zt_vn_frame;
    cb.virtualNetworkConfigFunction = zt_vn_config;

    ZT_Node_Config cfg;
    memset(&cfg, 0, sizeof(cfg));

    int64_t now = esp_timer_get_time() / 1000LL;
    ZT_ResultCode rc = ZT_Node_new(&s_node, &cfg, nullptr, nullptr, &cb, now);
    ESP_LOGI(TAG, "ZT_Node_new rc=%d node=%p", (int)rc, (void *)s_node);

    if (rc != ZT_RESULT_OK) {
        ESP_LOGE(TAG, "ZT_Node_new failed — aborting ZT task");
        vTaskDelete(nullptr);
        return;
    }

    // c) Start rx task after ZT_Node_new so its heap allocation comes first
    xTaskCreateStatic(zt_rx_task, "zt_rx", 32768, nullptr, 5,
                      s_rx_stack, &s_rx_tcb);

    // d) Background loop — nextTask=0 so first iteration runs immediately
    volatile int64_t nextTask = 0;
    for (;;) {
        now = esp_timer_get_time() / 1000LL;
        ZT_Node_processBackgroundTasks(s_node, nullptr, now, &nextTask);
        if (!heap_caps_check_integrity_all(false)) {
            ESP_LOGE(TAG, "HEAP CORRUPT after processBackgroundTasks");
            heap_caps_check_integrity_all(true);
        }

        int64_t sleep = nextTask - now;
        if (sleep < 10) sleep = 10;
        ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS((uint32_t)sleep));
    }
}

// ── Ethernet init ─────────────────────────────────────────────────────────────

extern "C" void app_main(void)
{
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);

    s_eth_event_group = xEventGroupCreate();

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    esp_netif_config_t netif_cfg = ESP_NETIF_DEFAULT_ETH();
    esp_netif_t *eth_netif = esp_netif_new(&netif_cfg);

    eth_mac_config_t mac_cfg = ETH_MAC_DEFAULT_CONFIG();
    eth_phy_config_t phy_cfg = ETH_PHY_DEFAULT_CONFIG();
    phy_cfg.autonego_timeout_ms = 100;
    phy_cfg.reset_gpio_num      = -1;

    esp_eth_mac_t *mac = esp_eth_mac_new_openeth(&mac_cfg);
    esp_eth_phy_t *phy = esp_eth_phy_new_dp83848(&phy_cfg);

    esp_eth_config_t eth_cfg = ETH_DEFAULT_CONFIG(mac, phy);
    esp_eth_handle_t eth_handle = NULL;
    ESP_ERROR_CHECK(esp_eth_driver_install(&eth_cfg, &eth_handle));
    ESP_ERROR_CHECK(esp_netif_attach(eth_netif,
                                     esp_eth_new_netif_glue(eth_handle)));

    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_ETH_GOT_IP,
                                               on_got_ip, NULL));
    ESP_ERROR_CHECK(esp_eth_start(eth_handle));

    xTaskCreate(zt_task, "zt_task", 65536, nullptr, 5, nullptr);
}
