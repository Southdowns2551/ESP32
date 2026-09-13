# syntax=docker/dockerfile:1

FROM python:3.11-slim-bookworm AS builder

ARG BLE_DEVICE_NAME="ESP32U"

RUN groupadd -r pio && useradd -r -g pio -m -s /bin/bash pio

RUN apt-get update && \
    apt-get install -y --no-install-recommends \
        git \
        && \
    rm -rf /var/lib/apt/lists/*

USER pio
WORKDIR /home/pio

RUN pip install --no-cache-dir --user platformio==6.1.19

ENV PATH="/home/pio/.local/bin:${PATH}"

RUN pio platform install espressif32@6.13.0 --with-package framework-arduinoespressif32 && \
    pio platform install espressif32@6.13.0 --with-package toolchain-xtensa-esp32

WORKDIR /home/pio/project
COPY --chown=pio:pio platformio.ini ./
COPY --chown=pio:pio src/ ./src/
COPY --chown=pio:pio lib/ ./lib/
COPY --chown=pio:pio include/config.h.example ./include/config.h.example
COPY --chown=pio:pio scripts/ ./scripts/

# WiFi credentials are injected via BuildKit secrets so they never
# appear in image layers or build history.  The secret file is
# mounted read-only at /run/secrets/wifi_creds during this single
# RUN step and is discarded afterwards.
#
# Secret file format (one value per line):
#   WIFI_SSID=MyNetwork
#   WIFI_PASSWORD=MyPassword
RUN --mount=type=secret,id=wifi_creds,uid=999 \
    if [ -f /run/secrets/wifi_creds ]; then \
        WIFI_SSID=$(grep '^WIFI_SSID=' /run/secrets/wifi_creds | cut -d= -f2-); \
        WIFI_PASSWORD=$(grep '^WIFI_PASSWORD=' /run/secrets/wifi_creds | cut -d= -f2-); \
    else \
        WIFI_SSID="YOUR_SSID"; \
        WIFI_PASSWORD="YOUR_PASSWORD"; \
    fi && \
    printf '#ifndef CONFIG_H\n#define CONFIG_H\n\n#define WIFI_SSID     "%s"\n#define WIFI_PASSWORD "%s"\n#define BLE_DEVICE_NAME "%s"\n\n#endif\n' \
        "${WIFI_SSID}" "${WIFI_PASSWORD}" "${BLE_DEVICE_NAME}" > include/config.h && \
    pio run && \
    rm -f include/config.h

# --- Output stage: minimal image with just the firmware binary ---
FROM scratch AS firmware
COPY --from=builder /home/pio/project/.pio/build/esp32dev/firmware.bin /firmware.bin
COPY --from=builder /home/pio/project/.pio/build/esp32dev/firmware.elf /firmware.elf
COPY --from=builder /home/pio/project/.pio/build/esp32dev/partitions.bin /partitions.bin
COPY --from=builder /home/pio/project/.pio/build/esp32dev/bootloader.bin /bootloader.bin
