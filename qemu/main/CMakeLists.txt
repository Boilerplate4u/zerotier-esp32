idf_component_register(
    SRCS "main.cpp"
    INCLUDE_DIRS "."
    REQUIRES zerotier_node esp_eth esp_netif nvs_flash esp_timer esp_event
)
