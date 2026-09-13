# ESP-32U

ESP32 IoT starter project with WiFi and Bluetooth Low Energy (BLE) support. Built with PlatformIO and the Arduino framework.

## Features

- WiFi station mode with automatic reconnection
- BLE GATT server with read/write/notify characteristic
- Periodic serial status output
- Modular architecture (wifi_manager, ble_manager)

## Quick Start

1. Install PlatformIO:
   ```bash
   python3 -m pip install --user platformio
   ```

2. Configure WiFi credentials:
   ```bash
   cp include/config.h.example include/config.h
   # Edit include/config.h with your SSID and password
   ```

3. Build and flash:
   ```bash
   pio run --target upload
   ```

4. Monitor serial output:
   ```bash
   pio device monitor
   ```

## Docker Build (No Local Toolchain Required)

1. Create your secrets file:
   ```bash
   cp wifi_creds.txt.example wifi_creds.txt
   # Edit wifi_creds.txt with your SSID and password
   ```

2. Build firmware inside Docker:
   ```bash
   docker build --target builder --secret id=wifi_creds,src=wifi_creds.txt -t esp32u-builder .
   ```

3. Extract firmware binaries:
   ```bash
   mkdir -p output
   docker run --rm -v "$(pwd)/output:/home/pio/project/output" esp32u-builder \
       bash /home/pio/project/scripts/entrypoint.sh extract
   ```

4. Flash from host (requires local esptool or PlatformIO):
   ```bash
   esptool.py --port /dev/cu.usbserial-0001 write_flash \
       0x1000 output/bootloader.bin \
       0x8000 output/partitions.bin \
       0x10000 output/firmware.bin
   ```

### Security

- WiFi credentials are injected via BuildKit secrets (never stored in image layers)
- Container runs as non-root user (`pio`)
- `no-new-privileges` enforced
- Read-only root filesystem
- `config.h` is deleted after compilation
- `.dockerignore` prevents secrets from entering the build context

## Project Structure

```
esp-32u/
├── src/main.cpp                  # Entry point
├── lib/wifi_manager/             # WiFi connection manager
├── lib/ble_manager/              # BLE GATT server
├── include/config.h.example      # Config template
├── include/config.h              # Your config (git-ignored)
├── scripts/entrypoint.sh         # Docker entrypoint
├── platformio.ini                # Build configuration
├── Dockerfile                    # Multi-stage build
├── docker-compose.yml            # Service definitions
├── wifi_creds.txt.example        # Docker secrets template
└── Master_Docs/Master_project.md # Full documentation
```

## Hardware

- Board: ESP32-D0WD-V3 (dual-core, 240 MHz, 4 MB flash)
- Serial port: `/dev/cu.usbserial-0001`

## Documentation

See [Master_Docs/Master_project.md](Master_Docs/Master_project.md) for full project documentation including architecture, APIs, setup procedures, and troubleshooting.
