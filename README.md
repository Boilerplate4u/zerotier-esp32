# ZeroTier client for ESP32 / ESPHome

> Experimental open source effort to create a working ZeroTier client for ESP32 devices, with first-class integration for ESPHome and Home Assistant.

## Goal and purpose

This project aims to create a working ZeroTier client implementation for ESP32 devices, with first-class integration for ESPHome and Home Assistant.

The purpose is to make secure, gateway-free remote ESPHome deployments possible. Today, there is no working ZeroTier client for ESP32. Users who want ESPHome devices reachable over ZeroTier must rely on an external gateway such as a router, Raspberry Pi, Home Assistant add-on, or another always-on Linux device. This project aims to remove that requirement by making the ESP32 device itself a ZeroTier node.

This would be useful for cabins, boats, RVs, sheds, remote sensors, agricultural monitoring, and other installations where users cannot rely on port forwarding, public IP addresses, or a separate VPN gateway.

## Current status

This project is experimental and under active development.

The prototype already has:

- a working ESP-IDF/QEMU test setup
- TAP networking with root-server responses
- a real UDP send path using `sendto()`
- NVS-backed persistent ZeroTier identity across runs
- successful `ZT_Node_new`
- successful local `ZT_Node_join`

The current blocker is the later join flow: the node can initialize and join locally, but it does not yet reach `ONLINE` and does not yet appear in ZeroTier Central.

## Technical challenge

ZeroTier One is normally designed for much larger systems such as laptops, servers, virtual machines, and containers.

ESP32 devices are highly constrained by comparison. Important constraints include:

- limited RAM
- limited flash storage
- limited CPU resources
- FreeRTOS task scheduling
- lwIP networking behavior
- crypto and entropy availability
- persistent storage limitations
- stack usage and heap fragmentation
- long-running reliability requirements

