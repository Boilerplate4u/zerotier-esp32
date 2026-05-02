# ZeroTier client for ESP32 / ESPHome (Home Assistant)

> Experimental open source effort to create a working ZeroTier client for ESP32 devices, with first-class integration for ESPHome and Home Assistant.

## Goal and purpose

The objective is to enable secure, gateway-free remote ESPHome deployments. Currently, users must rely on external gateways (routers, Raspberry Pis, or HA add-ons) to bridge ZeroTier networks. This project removes that dependency by making the ESP32 device itself a native ZeroTier node.

This is ideal for:
*   Remote cabins, boats, and RVs.
*   Agricultural monitoring and remote sensors.
*   Installations without public IPs or port-forwarding capabilities.

## Current status

This project is experimental and under active development.

**Milestones reached:**
- [x] ESP-IDF/QEMU test environment operational.
- [x] TAP networking with root-server response handling.
- [x] Functional UDP send path using `sendto()`.
- [x] Persistent ZeroTier identity stored in NVS.
- [x] Successful `ZT_Node_new` and local `ZT_Node_join`.

**Current blocker:**
The node initializes and joins locally but fails to reach `ONLINE` status or register in ZeroTier Central.

## Technical challenges

Porting ZeroTier One to the ESP32 involves overcoming constraints not present in standard ZeroTier client environments:

*   **Memory & Stack:** Managing the ZeroTier stack within limited RAM and avoiding heap fragmentation.
*   **Networking:** Aligning ZeroTier's logic with lwIP behavior and FreeRTOS task scheduling.
*   **Resources:** Balancing crypto/entropy requirements with limited CPU cycles and flash wear.
*   **Stability:** Ensuring long-running reliability on a resource-constrained microcontroller.
