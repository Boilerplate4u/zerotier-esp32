cmake_minimum_required(VERSION 3.16)
set(EXTRA_COMPONENT_DIRS
    /project/components
    /project/qemu/components
)
include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(zt_esp32_qemu)
