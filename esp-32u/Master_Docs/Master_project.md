# ESP-32U — Master Project Documentation

## Project Overview

### Purpose
IoT starter project for an ESP32 development board providing both WiFi (station mode) and Bluetooth Low Energy (BLE GATT server) capabilities. Serves as a foundation for building connected devices that can communicate over WiFi and BLE simultaneously.

### Technical Goals
- WiFi station-mode connectivity with automatic reconnection
- BLE GATT server with read/write/notify characteristic
- Modular architecture separating WiFi and BLE concerns
- Serial monitoring for diagnostics

### In-Scope
- WiFi STA connection and reconnect logic
- BLE server with one service and one characteristic
- Serial status reporting
- PlatformIO-based build system

### Out-of-Scope
- OTA firmware updates
- Web server / REST API
- MQTT integration
- Deep sleep / power management
- Sensor integration

---

## Architecture

### High-Level Design

```
┌─────────────────────────────────────────┐
│              ESP32 (main.cpp)           │
│                                         │
│  ┌─────────────┐   ┌─────────────────┐ │
│  │ wifi_manager│   │  ble_manager    │ │
│  │  (STA mode) │   │ (GATT server)  │ │
│  └──────┬──────┘   └───────┬────────┘ │
│         │                   │          │
└─────────┼───────────────────┼──────────┘
          │                   │
     WiFi AP            BLE Client
   (router)          (phone / laptop)
```

### Components and Responsibilities

| Component      | Responsibility                                      |
|----------------|-----------------------------------------------------|
| `main.cpp`     | Entry point, initialization, main loop              |
| `wifi_manager` | WiFi STA connect, reconnect, status                 |
| `ble_manager`  | BLE device init, GATT server, characteristic R/W/N  |
| `config.h`     | WiFi SSID/password, BLE device name                 |

### Data Flow
1. `setup()` → `wifi_init()` → connects to configured AP
2. `setup()` → `ble_init()` → starts GATT server and advertising
3. `loop()` → `wifi_check_connection()` → reconnects if link dropped
4. `loop()` → `ble_get_received_value()` → reads data written by BLE clients
5. `loop()` → prints periodic status to Serial

---

## Modules / Features

### wifi_manager
- Connects to WiFi in blocking mode with configurable timeout
- Stores SSID/password for reconnection attempts
- Non-blocking reconnect check (5-second interval) for use in `loop()`
- Files: `lib/wifi_manager/wifi_manager.h`, `lib/wifi_manager/wifi_manager.cpp`

### ble_manager
- Initializes BLE stack with device name
- Creates GATT server with one service (UUID: `4fafc201-1fb5-459e-8fcc-c5c9c331914b`)
- One read/write/notify characteristic (UUID: `beb5483e-36e1-4688-b7f5-ea07361b26a8`)
- Tracks client connection state
- Restarts advertising on client disconnect
- Files: `lib/ble_manager/ble_manager.h`, `lib/ble_manager/ble_manager.cpp`

---

## APIs / Interfaces

### WiFi Manager API

| Function                | Parameters                                         | Returns | Description                        |
|-------------------------|----------------------------------------------------|---------|------------------------------------|
| `wifi_init()`           | `ssid`, `password`, `timeout_ms` (default 10000)   | `bool`  | Blocking connect to WiFi AP        |
| `wifi_check_connection()` | none                                             | `void`  | Non-blocking reconnect check       |
| `wifi_is_connected()`   | none                                               | `bool`  | Current connection status          |

### BLE Manager API

| Function                    | Parameters      | Returns  | Description                          |
|-----------------------------|-----------------|----------|--------------------------------------|
| `ble_init()`                | `device_name`   | `void`   | Start BLE server and advertising     |
| `ble_get_received_value()`  | none            | `String` | Get + clear last written value       |
| `ble_set_value()`           | `value`         | `void`   | Set characteristic value + notify    |
| `ble_is_client_connected()` | none            | `bool`   | Client connection status             |

### BLE UUIDs

| Item            | UUID                                   |
|-----------------|----------------------------------------|
| Service         | `4fafc201-1fb5-459e-8fcc-c5c9c331914b` |
| Characteristic  | `beb5483e-36e1-4688-b7f5-ea07361b26a8` |

### Protocols
- WiFi: IEEE 802.11 b/g/n (2.4 GHz)
- BLE: Bluetooth 4.2 (GATT server)
- Serial: UART at 115200 baud

---

## Infrastructure & Runtime Environment

### Hardware
- **Board**: ESP32-D0WD-V3 (revision v3.1)
- **Features**: WiFi, BT, Dual Core, 240 MHz, 320 KB RAM, 4 MB Flash
- **Crystal**: 40 MHz
- **MAC**: 68:fe:71:16:7d:60
- **USB-Serial Chip**: CP2102 (or compatible)
- **Serial Port**: `/dev/cu.usbserial-0001`

### Development Machine
- **OS**: macOS (darwin 25.3.0, arm64)
- **User**: roberto
- **Workspace**: `/Users/roberto/Projects/ESP32/esp-32u/`

### Required Software
- Python 3.9+ (system Python at `/Library/Developer/CommandLineTools/usr/bin/python3`)
- PlatformIO Core CLI 6.1.19 (`pip3 install --user platformio`)
- PlatformIO binary path: `$HOME/Library/Python/3.9/bin`

### Installed Packages (auto-managed by PlatformIO)
- Platform: espressif32 @ 6.13.0
- Framework: arduino-espressif32 @ 3.20017.241212
- Toolchain: xtensa-esp32 @ 8.4.0+2021r2-patch5
- esptool.py @ 4.11.0

### Partition Table
- `huge_app.csv` — single 3 MB app partition (no OTA), required because WiFi + BLE exceeds default 1.25 MB

### Docker Build Environment
- **Docker Desktop**: 29.2.1 (macOS arm64)
- **Base Image**: `python:3.11-slim-bookworm`
- **Builder Image**: `esp32u-builder` (~2.77 GB, includes full PlatformIO + ESP32 toolchain)
- **Firmware Image**: `scratch` stage with only binary artifacts

---

## Credentials & Access

### WiFi Credentials
- **Local build location**: `include/config.h` (git-ignored)
- **Local template**: `include/config.h.example`
- **Docker build location**: `wifi_creds.txt` (git-ignored, injected via BuildKit secret)
- **Docker template**: `wifi_creds.txt.example`
- **SSID**: Set `WIFI_SSID`
- **Password**: Set `WIFI_PASSWORD`
- **Scope**: Development / local network

**Rules:**
- `config.h` must ONLY exist locally, never committed
- `wifi_creds.txt` must ONLY exist locally, never committed
- Both are listed in `.gitignore`
- Docker images never contain credentials (BuildKit secrets are ephemeral, `config.h` is deleted post-build)

---

## Configuration

### Environment Variables
None required. All configuration is in `include/config.h`.

### Config Files
| File                       | Purpose                                    | Committed |
|----------------------------|--------------------------------------------|-----------|
| `platformio.ini`           | Build config, board, ports, flags          | Yes       |
| `include/config.h`         | WiFi SSID/password, BLE device name        | No        |
| `include/config.h.example` | Template for config.h                      | Yes       |
| `wifi_creds.txt`           | Docker build secrets (SSID/password)       | No        |
| `wifi_creds.txt.example`   | Template for wifi_creds.txt                | Yes       |
| `.env`                     | Docker compose env overrides               | No        |
| `.env.example`             | Template for .env                          | Yes       |
| `.gitignore`               | Excludes secrets, build artifacts          | Yes       |
| `.dockerignore`            | Excludes secrets from Docker build context | Yes       |
| `Dockerfile`               | Multi-stage build with security hardening  | Yes       |
| `docker-compose.yml`       | Build/extract/flash/monitor services       | Yes       |
| `scripts/entrypoint.sh`    | Docker entrypoint dispatcher               | Yes       |

### Key platformio.ini Settings
```ini
[env:esp32dev]
platform = espressif32
board = esp32dev
framework = arduino
board_build.partitions = huge_app.csv
upload_port = /dev/cu.usbserial-0001
monitor_port = /dev/cu.usbserial-0001
monitor_speed = 115200
build_flags = -D CONFIG_BT_ENABLED
```

---

## Software Versions & Tooling

| Tool                 | Version                        |
|----------------------|--------------------------------|
| PlatformIO Core      | 6.1.19                         |
| espressif32 platform | 6.13.0                         |
| Arduino ESP32 core   | 3.20017.241212                 |
| Toolchain (xtensa)   | 8.4.0+2021r2-patch5            |
| esptool.py           | 4.11.0                         |
| Python (host)        | 3.9.6                          |
| Python (Docker)      | 3.11 (slim-bookworm)           |
| Docker Desktop       | 29.2.1                         |
| pip                  | 21.2.4                         |

---

## Setup & Recovery Procedure

### From Scratch

1. **Ensure Python 3.9+ is available**
   ```bash
   python3 --version
   ```

2. **Install PlatformIO**
   ```bash
   python3 -m pip install --user platformio
   export PATH="$PATH:$HOME/Library/Python/3.9/bin"
   ```

3. **Add PATH permanently**
   Ensure `~/.zshrc` contains:
   ```bash
   export PATH="$PATH:$HOME/Library/Python/3.9/bin"
   ```

4. **Clone / copy project to workspace**
   ```bash
   cd /Users/roberto/Projects/ESP32/esp-32u
   ```

5. **Create config.h**
   ```bash
   cp include/config.h.example include/config.h
   ```
   Edit `include/config.h` with your WiFi SSID and password.

6. **Connect ESP32 via USB**
   Verify it appears at `/dev/cu.usbserial-0001` (may differ on other machines — update `platformio.ini` if needed).

7. **Build**
   ```bash
   pio run
   ```

8. **Flash**
   ```bash
   pio run --target upload
   ```

9. **Monitor**
   ```bash
   pio device monitor
   ```
   Press Ctrl+C to exit monitor.

### Docker Build (Alternative — No Local Toolchain)

1. **Install Docker Desktop**
   Download from https://www.docker.com/products/docker-desktop/

2. **Create secrets file**
   ```bash
   cp wifi_creds.txt.example wifi_creds.txt
   ```
   Edit `wifi_creds.txt` with your WiFi SSID and password.

3. **Build image**
   ```bash
   docker build --target builder --secret id=wifi_creds,src=wifi_creds.txt -t esp32u-builder .
   ```

4. **Extract firmware**
   ```bash
   mkdir -p output
   docker run --rm -v "$(pwd)/output:/home/pio/project/output" esp32u-builder \
       bash /home/pio/project/scripts/entrypoint.sh extract
   ```

5. **Flash from host** (requires esptool.py or PlatformIO on host)
   ```bash
   esptool.py --port /dev/cu.usbserial-0001 write_flash \
       0x1000 output/bootloader.bin \
       0x8000 output/partitions.bin \
       0x10000 output/firmware.bin
   ```

### Docker Security Measures
- **Non-root user**: Container runs as `pio` (UID 999)
- **BuildKit secrets**: WiFi credentials mounted ephemerally, never stored in layers
- **config.h deleted**: Removed from image after compilation
- **no-new-privileges**: Prevents privilege escalation inside container
- **Read-only rootfs**: Supported via docker-compose
- **.dockerignore**: Prevents `config.h`, `.env`, `.pio/` from entering build context
- **Multi-stage build**: `firmware` target produces a minimal scratch image with only binaries
- **Pinned versions**: Base image and PlatformIO version are pinned

### Verification Steps
- Serial output should show `ESP32U — IoT Starter` banner
- WiFi should connect (or timeout if SSID is wrong)
- BLE should report `Server started — advertising as "ESP32U"`
- Status line should print every 10 seconds

### Common Failure Points
- **Flash too large**: Ensure `board_build.partitions = huge_app.csv` is set
- **BLE init fails**: Do not set `CONFIG_BTDM_CTRL_MODE_BLE_ONLY` build flag
- **Serial port not found**: Check `/dev/cu.usbserial-*` and update `platformio.ini`
- **LDF "No dependencies"**: Headers must live in `lib/<name>/` alongside `.cpp` files, not in `include/`

---

## Current Status

### Completed
- PlatformIO project scaffolded and configured
- WiFi manager with connect/reconnect
- BLE GATT server with read/write/notify
- Firmware builds, flashes, and runs on hardware
- Serial output verified
- Docker build environment with security hardening
- BuildKit secrets for credential injection
- Multi-stage Dockerfile (builder + scratch firmware output)
- docker-compose.yml with build/extract/flash/monitor services
- Entrypoint script for container operations

### In-Progress
- None

### Known Technical Debt
- BLE and WiFi coexistence not stress-tested under heavy traffic
- No error handling for BLE `new` allocations (callbacks)
- WiFi reconnect uses simple interval, no exponential backoff

### Known Bugs / Risks
- None currently

---

## Changelog

| Date       | Change                                                        |
|------------|---------------------------------------------------------------|
| 2026-03-14 | Initial project creation with PlatformIO, WiFi + BLE scaffold |
| 2026-03-14 | Fixed BLE type mismatch (std::string vs Arduino String)       |
| 2026-03-14 | Switched to huge_app.csv partition (WiFi+BLE exceeds 1.25 MB) |
| 2026-03-14 | Removed CONFIG_BTDM_CTRL_MODE_BLE_ONLY flag (caused btStart failure) |
| 2026-03-14 | Added Docker build environment with multi-stage Dockerfile          |
| 2026-03-14 | Implemented BuildKit secrets for WiFi credential injection           |
| 2026-03-14 | Added docker-compose.yml, entrypoint.sh, .dockerignore              |
| 2026-03-14 | Installed Docker Desktop 29.2.1 on development machine              |
