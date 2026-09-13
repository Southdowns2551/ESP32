/**
 * @file wifi_manager.cpp
 * @brief WiFi connection manager implementation.
 *
 * Handles STA-mode connection, timeout-based initial connect,
 * and periodic reconnection when the link drops.
 */

#include "wifi_manager.h"
#include <WiFi.h>

static const unsigned long RECONNECT_INTERVAL_MS = 5000;
static unsigned long       _last_reconnect_attempt = 0;
static const char*         _ssid     = nullptr;
static const char*         _password = nullptr;

bool wifi_init(const char* ssid, const char* password, unsigned long timeout_ms) {
    /**
     * @brief Connect to WiFi in blocking mode up to timeout_ms.
     *
     * @param ssid        Network SSID.
     * @param password    Network password.
     * @param timeout_ms  Max wait time in milliseconds.
     * @return true on successful connection, false on timeout.
     */
    _ssid     = ssid;
    _password = password;

    WiFi.mode(WIFI_STA);
    WiFi.begin(_ssid, _password);

    Serial.printf("[WiFi] Connecting to %s", _ssid);

    unsigned long start = millis();
    while (WiFi.status() != WL_CONNECTED && (millis() - start) < timeout_ms) {
        delay(250);
        Serial.print(".");
    }
    Serial.println();

    if (WiFi.status() == WL_CONNECTED) {
        Serial.printf("[WiFi] Connected — IP: %s\n", WiFi.localIP().toString().c_str());
        return true;
    }

    Serial.println("[WiFi] Connection timed out");
    return false;
}

void wifi_check_connection() {
    /**
     * @brief Non-blocking reconnect check.
     *
     * If disconnected and enough time has elapsed since the last attempt,
     * triggers WiFi.reconnect(). Safe to call every loop() iteration.
     */
    if (WiFi.status() == WL_CONNECTED) return;

    unsigned long now = millis();
    if (now - _last_reconnect_attempt >= RECONNECT_INTERVAL_MS) {
        _last_reconnect_attempt = now;
        Serial.println("[WiFi] Reconnecting...");
        WiFi.reconnect();
    }
}

bool wifi_is_connected() {
    /**
     * @brief Thin wrapper around WiFi.status().
     * @return true when station is connected to an AP.
     */
    return WiFi.status() == WL_CONNECTED;
}

int wifi_scan() {
    /**
     * @brief Scan all available WiFi networks and print a formatted table to Serial.
     *
     * Puts WiFi in STA mode (disconnected), runs a blocking scan, then prints
     * each network sorted by signal strength (strongest first, handled by the
     * ESP-IDF scan internally).
     *
     * @return Number of networks found, or -1 on scan failure.
     *
     * Side effects: sets WiFi to STA+disconnect, prints to Serial,
     * calls WiFi.scanDelete() to free scan memory.
     */
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);

    Serial.println("[WiFi Scan] Scanning for networks...");
    int n = WiFi.scanNetworks();

    if (n == WIFI_SCAN_FAILED) {
        Serial.println("[WiFi Scan] Scan failed");
        return -1;
    }

    if (n == 0) {
        Serial.println("[WiFi Scan] No networks found");
        return 0;
    }

    Serial.printf("[WiFi Scan] %d network(s) found:\n", n);
    Serial.println("------------------------------------------------------");
    Serial.printf("%-4s %-32s %6s  %-4s  %s\n", "#", "SSID", "RSSI", "CH", "Encryption");
    Serial.println("------------------------------------------------------");

    for (int i = 0; i < n; i++) {
        const char* enc;
        switch (WiFi.encryptionType(i)) {
            case WIFI_AUTH_OPEN:            enc = "Open";       break;
            case WIFI_AUTH_WEP:             enc = "WEP";        break;
            case WIFI_AUTH_WPA_PSK:         enc = "WPA";        break;
            case WIFI_AUTH_WPA2_PSK:        enc = "WPA2";       break;
            case WIFI_AUTH_WPA_WPA2_PSK:    enc = "WPA/WPA2";   break;
            case WIFI_AUTH_WPA2_ENTERPRISE: enc = "WPA2-Ent";   break;
            case WIFI_AUTH_WPA3_PSK:        enc = "WPA3";       break;
            case WIFI_AUTH_WPA2_WPA3_PSK:   enc = "WPA2/WPA3";  break;
            default:                        enc = "Unknown";     break;
        }

        Serial.printf("%-4d %-32s %4d    %-4d  %s\n",
            i + 1,
            WiFi.SSID(i).c_str(),
            WiFi.RSSI(i),
            WiFi.channel(i),
            enc);
    }

    Serial.println("------------------------------------------------------");
    WiFi.scanDelete();
    return n;
}
