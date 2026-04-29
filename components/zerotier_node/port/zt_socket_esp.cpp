// ESP32 ZeroTier socket wrapper — lwIP UDP på port 9993
#include <string.h>
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "lwip/sockets.h"
#include "lwip/netdb.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "ZeroTierOne.h"

static const char *TAG = "zt_socket";

// Globalt socket och node-referens
static int zt_sock = -1;
static ZT_Node *zt_node_ref = nullptr;
static void *zt_uptr_ref = nullptr;

// Skapar och binder UDP-socket på port 9993
extern "C" int zt_socket_init(void) {
    zt_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (zt_sock < 0) {
        ESP_LOGE(TAG, "socket() failed");
        return -1;
    }

    struct sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(9993);

    if (bind(zt_sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        ESP_LOGE(TAG, "bind() failed");
        close(zt_sock);
        zt_sock = -1;
        return -1;
    }

    // Sätt 100ms timeout så receive-loopen inte blockerar för evigt
    struct timeval tv = { .tv_sec = 0, .tv_usec = 100000 };
    setsockopt(zt_sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

    ESP_LOGI(TAG, "UDP socket bunden till port 9993");
    return 0;
}

// FreeRTOS-task som tar emot paket och skickar till ZT-noden
static void zt_recv_task(void *arg) {
    uint8_t buf[2048];
    struct sockaddr_storage from;
    socklen_t from_len;

    while (1) {
        from_len = sizeof(from);
        int n = recvfrom(zt_sock, buf, sizeof(buf), 0,
                         (struct sockaddr *)&from, &from_len);
        if (n > 0 && zt_node_ref) {
            int64_t now = (int64_t)(esp_timer_get_time() / 1000ULL);
            ZT_Node_processWirePacket(zt_node_ref, nullptr, now,
                -1, (struct sockaddr_storage *)&from,
                buf, (unsigned int)n, &now);
        }
        vTaskDelay(1); // yield
    }
}

// Startar receive-task — anropas efter ZT_Node_new
extern "C" void zt_socket_start_recv(ZT_Node *node, void *uptr) {
    zt_node_ref = node;
    zt_uptr_ref = uptr;
    xTaskCreate(zt_recv_task, "zt_recv", 4096, nullptr, 5, nullptr);
    ESP_LOGI(TAG, "zt_recv_task startad");
}

// Callback: ZeroTier vill skicka ett UDP-paket
extern "C" int zt_wire_send_impl(ZT_Node *node, void *uptr, void *tptr,
    int64_t localSocket,
    const struct sockaddr_storage *addr,
    const void *data, unsigned int len,
    unsigned int ttl)
{
    if (zt_sock < 0) return 0;
    socklen_t addrlen = (addr->ss_family == AF_INET6)
        ? sizeof(struct sockaddr_in6)
        : sizeof(struct sockaddr_in);
    sendto(zt_sock, data, len, 0, (const struct sockaddr *)addr, addrlen);
    return 0;
}
