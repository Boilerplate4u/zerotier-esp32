cmake_minimum_required(VERSION 3.16)

file(GLOB ZT_ALL_SRCS /project/ZeroTierOne/node/*.cpp)
list(FILTER ZT_ALL_SRCS EXCLUDE REGEX ".*(AES_aesni|AES_armcrypto|Bond|Metrics|Trace)\.cpp$")
list(APPEND ZT_ALL_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/port/zt_mutex.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/port/zt_utils_esp.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/port/zt_metrics_esp.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/port/zt_bond_esp.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/port/zt_trace_esp.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/port/zt_storage_esp.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/port/zt_socket_esp.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/port/zt_tap_esp.cpp
)

idf_component_register(
    SRCS ${ZT_ALL_SRCS}
    INCLUDE_DIRS
        /project/ZeroTierOne/node
        port
        port/stubs
        /project/ZeroTierOne/include
        include
        include
    REQUIRES
        freertos
        lwip
        nvs_flash
        esp_timer
    PRIV_REQUIRES
        esp_system
)

target_compile_options(${COMPONENT_LIB} PRIVATE
    -fno-rtti
    -fexceptions
    -Wno-unused-variable
    -Wno-unused-function
    -DZT_NO_BOND
    -DZT_NO_METRICS
    -DZT_NO_PEER_METRICS
    -DZT_NO_TYPE_PUNNING
    -DZT_NO_UNALIGNED_ACCESS
    -include thread
    -DZT_NO_TRACE
    -Wno-error=return-type
    -Wno-error=maybe-uninitialized
    -Wno-error=stringop-overflow
    -Wno-error=array-bounds
    -Wno-error=restrict
    -Wno-volatile
    -Wno-deprecated
)
