idf_component_register(
    SRCS "main.cpp"
    INCLUDE_DIRS "."
    REQUIRES
        esp_netif
        esp_eth
        esp_event
        esp_timer
        nvs_flash
    PRIV_REQUIRES
        zerotier_node
)
