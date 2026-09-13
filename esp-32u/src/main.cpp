/**
 * @file main.cpp
 * @brief ESP32U entry point — initializes WiFi and BLE, then runs
 *        a periodic status loop.
 *
 * WiFi credentials are read from include/config.h (git-ignored).
 * Copy include/config.h.example → include/config.h and fill in your values.
 */

#include <Arduino.h>
#include "config.h"
#include "wifi_manager.h"
#include "ble_manager.h"

static const unsigned long STATUS_INTERVAL_MS = 10000;
static unsigned long       last_status_print  = 0;

void setup() {
    /**
     * @brief Arduino setup — serial init, WiFi connect, BLE server start.
     *
     * Side effects: opens Serial at 115200, connects WiFi, starts BLE
     * advertising.
     */
    Serial.begin(115200);
    delay(1000);

    Serial.println("=============================");
    Serial.println("  ESP32U — IoT Starter");
    Serial.println("=============================");

    wifi_scan();
    Serial.println();

    wifi_init(WIFI_SSID, WIFI_PASSWORD);
    ble_init(BLE_DEVICE_NAME);

    Serial.println("[Setup] Initialization complete\n");
}

void loop() {
    /**
     * @brief Arduino main loop — checks WiFi health, processes BLE data,
     *        and prints a periodic status line.
     */
    wifi_check_connection();

    String ble_data = ble_get_received_value();
    if (ble_data.length() > 0) {
        Serial.printf("[Loop] BLE data received: %s\n", ble_data.c_str());
    }

    unsigned long now = millis();
    if (now - last_status_print >= STATUS_INTERVAL_MS) {
        last_status_print = now;
        Serial.printf("[Status] WiFi: %s | BLE client: %s | Uptime: %lu s\n",
            wifi_is_connected() ? "Connected" : "Disconnected",
            ble_is_client_connected() ? "Yes" : "No",
            now / 1000);
    }
}
