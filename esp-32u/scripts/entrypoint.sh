#!/usr/bin/env bash
# @file entrypoint.sh
# @brief Docker entrypoint for ESP32U build/flash/monitor operations.
#
# Accepts a single command argument:
#   build   — compile firmware (already done during image build; re-runs if sources changed)
#   extract — copy firmware binaries to ./output/ bind mount
#   flash   — upload firmware to ESP32 via serial
#   monitor — open serial monitor at 115200 baud
#
# Errors: exits non-zero on unknown command or PlatformIO failure.
# Side effects: writes to ./output/ (extract), writes to serial device (flash/monitor).

set -euo pipefail

PROJECT_DIR="/home/pio/project"
OUTPUT_DIR="${PROJECT_DIR}/output"
BUILD_DIR="${PROJECT_DIR}/.pio/build/esp32dev"

case "${1:-build}" in
    build)
        echo "[entrypoint] Building firmware..."
        cd "${PROJECT_DIR}"
        pio run
        echo "[entrypoint] Build complete."
        ;;

    extract)
        echo "[entrypoint] Extracting firmware binaries to output/..."
        mkdir -p "${OUTPUT_DIR}"
        cp "${BUILD_DIR}/firmware.bin"    "${OUTPUT_DIR}/"
        cp "${BUILD_DIR}/firmware.elf"    "${OUTPUT_DIR}/"
        cp "${BUILD_DIR}/partitions.bin"  "${OUTPUT_DIR}/"
        cp "${BUILD_DIR}/bootloader.bin"  "${OUTPUT_DIR}/"
        echo "[entrypoint] Firmware files:"
        ls -lh "${OUTPUT_DIR}"/*.bin "${OUTPUT_DIR}"/*.elf
        ;;

    flash)
        echo "[entrypoint] Flashing firmware to ESP32..."
        cd "${PROJECT_DIR}"
        pio run --target upload
        echo "[entrypoint] Flash complete."
        ;;

    monitor)
        echo "[entrypoint] Starting serial monitor (115200 baud)..."
        echo "[entrypoint] Press Ctrl+C to exit."
        cd "${PROJECT_DIR}"
        pio device monitor
        ;;

    *)
        echo "[entrypoint] ERROR: Unknown command '${1}'"
        echo "Usage: entrypoint.sh {build|extract|flash|monitor}"
        exit 1
        ;;
esac
