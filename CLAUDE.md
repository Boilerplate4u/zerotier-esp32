# CLAUDE.md - ZeroTier ESP32 Port

> You are running inside container `zt-dev`. All commands are direct shell commands, executed as root. Project root: `/project`. QEMU project: `/qemu-project/`.

## Analysis Before Action
Before making any code changes:
1. Read all relevant source files and headers
2. State your understanding of the root cause
3. Explain what you expect the fix to achieve
4. Only then proceed with changes

---

## Session Start - Always Run First

```bash
export PATH=/opt/esp/tools/cmake/3.24.0/bin:$PATH
. /opt/esp/idf/export.sh
```

Verify QEMU project:
```bash
ls /qemu-project/sdkconfig.defaults 2>/dev/null || echo "QEMU project missing - must be recreated"
```

---

## Project Overview

Port of ZeroTier `node/` core to ESP32 as an ESP-IDF component.

| Target | Purpose | Status |
|--------|---------|--------|
| ESP32-S3 + PSRAM | Production hardware | Compiles and links; PSRAM linker fragment not yet verified |
| ESP32 + QEMU | Development/testing | QEMU boots, Ethernet OK, `ZT_Node_new` returns rc=100 (malloc failure) |

---

## Build Commands

### ESP32-S3 Production

```bash
cd /project && idf.py build
```

### ESP32 QEMU

```bash
export EXTRA_CPPFLAGS='-DZT_MAX_NETWORK_RULES=32 -DZT_MAX_NETWORK_CAPABILITIES=1 \
  -DZT_MAX_NETWORK_TAGS=1 -DZT_MAX_CERTIFICATES_OF_OWNERSHIP=1 \
  -DZT_MAX_CAPABILITY_RULES=1 -DZT_RX_QUEUE_SIZE=4 \
  -DZT_TX_QUEUE_SIZE=4 -DZT_DEFAULT_PHYSMTU=1280 -DZT_MAX_PACKET_FRAGMENTS=2 \
  -DZT_MAX_PEER_NETWORK_PATHS=4 -DZT_LAST_IDENTITY_VERIFICATION_SIZE=256 \
  -DZT_IDENTITY_GEN_MEMORY_OVERRIDE=65536 \
  -DZT_NETWORKCONFIG_DICT_CAPACITY_OVERRIDE=4096'
cd /qemu-project && idf.py build
```

> NOTE: Always do a clean build (`rm -rf build`) after changing EXTRA_CPPFLAGS. Incremental
> builds will only relink and the new defines will not take effect in already-compiled objects.

### Run QEMU

```bash
# Build 4 MB flash image
python3 -c "
flash_size=4*1024*1024
open('/tmp/qemu_flash.bin','wb').write(b'\xff'*flash_size)
def w(s,o): d=open(s,'rb').read(); f=open('/tmp/qemu_flash.bin','r+b'); f.seek(o); f.write(d); f.close()
w('/qemu-project/build/bootloader/bootloader.bin',0x1000)
w('/qemu-project/build/partition_table/partition-table.bin',0x8000)
w('/qemu-project/build/zt_esp32_qemu.bin',0x10000)
"

export PATH=/root/.espressif/tools/qemu-xtensa/esp_develop_8.2.0_20240122/qemu/bin:$PATH
qemu-system-xtensa -nographic -machine esp32 \
  -drive file=/tmp/qemu_flash.bin,if=mtd,format=raw \
  -nic tap,ifname=tap0,script=no,downscript=no,model=open_eth -no-reboot
```

---

## Development Tools

### clangd (clangd-lsp plugin active)

`compile_commands.json` is a symlink at `/qemu-project/compile_commands.json` pointing to
`/qemu-project/build/compile_commands.json`. A `.clangd` config file exists at
`/qemu-project/.clangd`.

**Important:** `compile_commands.json` reflects the last full build. EXTRA_CPPFLAGS must be
set during the build for clangd to see the correct reduced defines. After a clean build with
EXTRA_CPPFLAGS, clangd will report accurate struct sizes.

To regenerate `compile_commands.json` with correct defines:
```bash
export PATH=/opt/esp/tools/cmake/3.24.0/bin:$PATH
. /opt/esp/idf/export.sh 2>/dev/null
export EXTRA_CPPFLAGS='...'   # full set as above
cd /qemu-project && rm -rf build && idf.py build
# compile_commands.json symlink updates automatically
```

To ask clangd for actual struct sizes with current defines:
```
Use clangd to determine sizeof(Node), sizeof(Switch), sizeof(Bond)
with the current EXTRA_CPPFLAGS defines applied. Report each field
that contributes more than 1 KB. If clangd results differ from
source analysis, trust clangd.
```

### Size analysis without building

```bash
# After a completed build:
idf.py size-components 2>/dev/null | grep -E 'zerotier|Total'
```

---

## Directory Structure

```
/project/
+-- CMakeLists.txt
+-- sdkconfig.defaults          <- ESP32-S3 + PSRAM configuration
+-- main/
|   +-- CMakeLists.txt
|   +-- main.cpp                <- Phase 2: ZT_Node_new + NVS + UDP socket
+-- components/
|   +-- zerotier_node/
|       +-- CMakeLists.txt
|       +-- zerotier_spiram.lf  <- Linker fragment: ZT BSS to PSRAM (unverified)
|       +-- port/
|           +-- stubs/          <- prometheus, nlohmann/json, ifaddrs.h, sys/un.h
|           +-- zt_mutex.cpp / zt_utils_esp.cpp / zt_metrics_esp.cpp
|           +-- zt_bond_esp.cpp / zt_trace_esp.cpp / zt_storage_esp.cpp
|           +-- zt_socket_esp.cpp / zt_tap_esp.cpp
+-- ZeroTierOne/                <- Patched ZeroTier source tree

/qemu-project/
+-- CMakeLists.txt              <- EXTRA_COMPONENT_DIRS -> /project/components
+-- sdkconfig.defaults          <- esp32 + OpenEth + C++ exceptions
+-- main/main.cpp               <- OpenEth init + ZT_Node_new + heap diagnostics
+-- .clangd                     <- Points clangd to /qemu-project/build
+-- compile_commands.json       <- Symlink to build/compile_commands.json
```

---

## Patches Applied to ZeroTierOne

| File | Change | Status |
|------|--------|--------|
| `node/Mutex.hpp` | ESP_PLATFORM block: FreeRTOS SemaphoreHandle_t replaces pthreads | Applied |
| `node/NetworkConfig.hpp` | ZT_NETWORKCONFIG_DICT_CAPACITY_OVERRIDE support | Applied |
| `node/Utils.cpp` | `randomBuf[65536]` -> `randomBuf[256]` (saves 64 KB BSS) | Applied |
| `node/Utils.cpp` | `#ifndef ESP_PLATFORM` guard around POSIX getSecureRandom | Applied |
| `port/zt_utils_esp.cpp` | Implements `Utils::getSecureRandom` via `esp_fill_random()` | Applied |
| `include/ZeroTierOne.h` | `#ifndef` guards on ZT_MAX_NETWORK_RULES/CAPABILITIES/TAGS/CERTIFICATES/CAPABILITY_RULES | Applied |
| `include/ZeroTierOne.h` | `#ifndef` guard on ZT_MAX_PEER_NETWORK_PATHS | Applied |
| `node/Node.hpp` | `#ifndef ZT_LAST_IDENTITY_VERIFICATION_SIZE` + modulo in rateGateIdentityVerification | Applied |
| `node/Constants.hpp` | ZT_RX_QUEUE_SIZE, ZT_TX_QUEUE_SIZE, ZT_MAX_PACKET_FRAGMENTS already had `#ifndef` guards | Pre-existing |

**Pending patch:**
```c
// Constants.hpp ~line 290 - add before existing ZT_DEFAULT_PHYSMTU definition
#ifndef ZT_DEFAULT_PHYSMTU
#define ZT_DEFAULT_PHYSMTU 1500
#endif
```

---

## Known Issues

### 1. QEMU rc=100 - RESOLVED

**Symptom:** `ZT_Node_new` returns `rc=100` (`ZT_RESULT_FATAL_ERROR_OUT_OF_MEMORY`)

**Root cause:** Multiple oversized heap allocations exceeded the largest free block (114 KB).

| Field / Allocation | Location | Default size | Fixed size |
|--------------------|----------|-------------|------------|
| `_lastIdentityVerification[16384]` | `Node.hpp` | 128 KB | 2 KB (`ZT_LAST_IDENTITY_VERIFICATION_SIZE=256`) |
| `_paths[64]` × NominatedPath in Bond | `Bond.hpp` | ~109 KB | ~7 KB (`ZT_MAX_PEER_NETWORK_PATHS=4`) |
| `_rxQueue[32]` × RXQueueEntry | `Switch.hpp` | ~617 KB | ~20 KB (`ZT_RX_QUEUE_SIZE=4`, `ZT_MAX_PACKET_FRAGMENTS=2`) |
| `Identity::generate()` PoW buffer | `Identity.cpp` | 2 MB heap alloc | 64 KB (`ZT_IDENTITY_GEN_MEMORY_OVERRIDE=65536`) |

All fixes are in EXTRA_CPPFLAGS — no source patches required beyond the `#ifndef` guards
already applied to `Node.hpp` and `ZeroTierOne.h`.

### 6. zt_task Stack Overflow - RESOLVED

**Symptom:** `LoadStorePIFAddrError` at PC in FreeRTOS idle task watchdog
(`find_entry_from_task_handle_and_check_all_reset`, task_wdt.c:150).
EXCVADDR=0x680b643b — corrupted linked-list `next` pointer in the watchdog subscriber list.

**Root cause:** `zt_task` was created with 8192 bytes of stack. The `ZT_Node_new` call chain
(`new Node()` → placement-new of Switch/Bond/Topology/etc. → `Identity::generate()` PoW loop)
overflowed the stack and corrupted adjacent FreeRTOS heap structures, including the task
watchdog entry list. The idle task then faulted dereferencing the corrupted pointer.

**Fix:** Increase `zt_task` stack to 32768 bytes in `main/main.cpp`:
```cpp
xTaskCreate(zt_task, "zt_task", 32768, NULL, 5, NULL);
```

### 2. DRAM BSS Overflow - ESP32-S3 Production

**Symptom:** Linker error: `dram0.bss overflowed by ~213 KB`

**Root cause:** `static const NetworkConfig NIL_NC` in `NetworkConfig::fromDictionary()` = 436 KB BSS.

**Fix:** Linker fragment `zerotier_spiram.lf` maps ZeroTier BSS to PSRAM via `extern_ram`.
Requires `CONFIG_SPIRAM_ALLOW_BSS_SEG_EXTERNAL_MEMORY=y`.

Correct `zerotier_spiram.lf`:
```
[mapping:zerotier_node]
archive: libzerotier_node.a
entries:
    if SPIRAM_ALLOW_BSS_SEG_EXTERNAL_MEMORY = y:
        * (extram_bss);
            bss -> extern_ram ALIGN(4) ALIGN(4, post) SURROUND(zt_bss),
            common -> extern_ram ALIGN(4) ALIGN(4, post) SURROUND(zt_common)
```

`LINK_FRAGMENTS` must be inside `idf_component_register()` after `PRIV_REQUIRES`:
```cmake
idf_component_register(
    ...
    PRIV_REQUIRES
        esp_system
    LINK_FRAGMENTS ${CMAKE_CURRENT_SOURCE_DIR}/zerotier_spiram.lf
)
```

### 3. C++ Exceptions Required

`CONFIG_COMPILER_CXX_EXCEPTIONS=y` must be set in `sdkconfig.defaults`. Without it,
`__cxa_allocate_exception` is replaced by a stub that calls `abort()`.

### 4. EXTRA_CPPFLAGS vs add_compile_definitions - Mutual Exclusion

Never use both simultaneously. Both propagate to all components -> duplicate macro
definitions -> `-Werror` failures.

### 5. Diagnostics and OOM Debugging on ESP32

For hard-to-find out-of-memory failures, override `operator new` globally
to log or assert on large allocations. This immediately identifies which
allocation exceeds the heap limit without manual struct analysis:

```cpp
void* operator new(size_t size) {
    if (size > 200 * 1024)
        ESP_LOGE("heap", "Large alloc: %u bytes", (unsigned)size);
    void* p = std::malloc(size);
    if (!p) throw std::bad_alloc();
    return p;
}
```

Add this to a debug-only source file in `components/zerotier_node/port/`.
Remove or guard with `#ifdef DEBUG` before production builds.

Note: ZT_IDENTITY_GEN_MEMORY defaults to 2 MB (proof-of-work scratchpad
for Sybil attack prevention). On ESP32 this must be overridden via
`-DZT_IDENTITY_GEN_MEMORY_OVERRIDE=65536` in EXTRA_CPPFLAGS.

---

## Environment Variables

```bash
# Required every session
export PATH=/opt/esp/tools/cmake/3.24.0/bin:$PATH
. /opt/esp/idf/export.sh

# Required for QEMU builds
export EXTRA_CPPFLAGS='-DZT_MAX_NETWORK_RULES=32 -DZT_MAX_NETWORK_CAPABILITIES=1 \
  -DZT_MAX_NETWORK_TAGS=1 -DZT_MAX_CERTIFICATES_OF_OWNERSHIP=1 \
  -DZT_MAX_CAPABILITY_RULES=1 -DZT_RX_QUEUE_SIZE=4 \
  -DZT_TX_QUEUE_SIZE=4 -DZT_DEFAULT_PHYSMTU=1280 -DZT_MAX_PACKET_FRAGMENTS=2 \
  -DZT_MAX_PEER_NETWORK_PATHS=4 -DZT_LAST_IDENTITY_VERIFICATION_SIZE=256 \
  -DZT_IDENTITY_GEN_MEMORY_OVERRIDE=65536 \
  -DZT_NETWORKCONFIG_DICT_CAPACITY_OVERRIDE=4096'

# Required to run QEMU
export PATH=/root/.espressif/tools/qemu-xtensa/esp_develop_8.2.0_20240122/qemu/bin:$PATH
```

---

## sdkconfig.defaults

### ESP32-S3 Production (`/project/sdkconfig.defaults`)

```
CONFIG_IDF_TARGET="esp32s3"
CONFIG_ESP32S3_DEFAULT_CPU_FREQ_240=y
CONFIG_FREERTOS_HZ=1000
CONFIG_LWIP_IPV6=y
CONFIG_PARTITION_TABLE_SINGLE_APP=y
CONFIG_FREERTOS_PLACE_FUNCTIONS_INTO_FLASH=y
CONFIG_SPIRAM=y
CONFIG_SPIRAM_USE_MALLOC=y
CONFIG_SPIRAM_MEMTEST=y
CONFIG_SPIRAM_SPEED_80M=y
CONFIG_VFS_SUPPORT_TERMIOS=y
CONFIG_SPIRAM_ALLOW_BSS_SEG_EXTERNAL_MEMORY=y
CONFIG_SPIRAM_FETCH_INSTRUCTIONS=y
CONFIG_SPIRAM_RODATA=y
```

### QEMU (`/qemu-project/sdkconfig.defaults`)

```
CONFIG_IDF_TARGET="esp32"
CONFIG_ESP32_DEFAULT_CPU_FREQ_240=y
CONFIG_FREERTOS_HZ=1000
CONFIG_ETH_ENABLED=y
CONFIG_ETH_USE_OPENETH=y
CONFIG_ESP_WIFI_ENABLED=n
CONFIG_BT_ENABLED=n
CONFIG_BLUEDROID_ENABLED=n
CONFIG_MBEDTLS_HARDWARE_AES=n
CONFIG_MBEDTLS_HARDWARE_SHA=n
CONFIG_MBEDTLS_HARDWARE_MPI=n
CONFIG_LWIP_IPV6=y
CONFIG_COMPILER_CXX_EXCEPTIONS=y
```

---

## Test Procedure

### ESP32-S3 build

```bash
cd /project
idf.py build 2>&1 | tail -5
# Expected: "zt_esp32.bin binary size 0x... bytes. Bin file ends ... free."
# Failure: "dram0.bss overflowed" -> linker fragment not working
```

### QEMU build and execution

```bash
# Set EXTRA_CPPFLAGS first, then:
cd /qemu-project && rm -rf build && idf.py build
# Run QEMU (see Run QEMU section above)

# Expected output:
# I (xxxx) zt_qemu: IP: 10.0.2.15
# I (xxxx) zt_qemu: ZT_Node_new rc=0 node=...
# I (xxxx) zt_qemu: ZT event: 6
```

---

## Next Steps

**Priority 1 - Fix QEMU rc=100**
1. Use clangd (or a compile-time static_assert) to measure `sizeof(Node)` with all
   EXTRA_CPPFLAGS applied. Present result as a table before any code changes.
2. Identify remaining fields that push sizeof(Node) above 114 KB.
3. Add `#ifndef` guards and corresponding EXTRA_CPPFLAGS entries for each.
4. Clean build and run QEMU -- verify `ZT_Node_new rc=0`.

**Priority 2 - ZT_Node_join**
Once rc=0: call `ZT_Node_join(node, NETWORK_ID)` and verify node appears in ZeroTier Central.

**Priority 3 - Verify PSRAM Linker Fragment (ESP32-S3)**
Always ask before build test production. Check `LINK_FRAGMENTS` is inside `idf_component_register()` after `PRIV_REQUIRES`.
