/**
 * @file wifi_manager.h
 * @brief WiFi connection manager for ESP32.
 *
 * Provides functions to connect to a WiFi network, monitor connection
 * status, and automatically reconnect on disconnection.
 */

#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <Arduino.h>

/**
 * @brief Initialize WiFi in station mode and connect to the configured AP.
 *
 * Reads SSID and password from config.h. Blocks until connected or
 * the attempt times out.
 *
 * @param ssid      Network SSID.
 * @param password  Network password.
 * @param timeout_ms Maximum time to wait for connection (default 10 000 ms).
 * @return true if connected, false on timeout.
 */
bool wifi_init(const char* ssid, const char* password, unsigned long timeout_ms = 10000);

/**
 * @brief Check WiFi connection and reconnect if necessary.
 *
 * Call this periodically from loop(). If the link is down it will
 * attempt a non-blocking reconnect.
 */
void wifi_check_connection();

/**
 * @brief Return whether WiFi is currently connected.
 *
 * @return true if connected, false otherwise.
 */
bool wifi_is_connected();

/**
 * @brief Scan for available WiFi networks and print results to Serial.
 *
 * Sets WiFi to STA mode, performs a blocking scan, and prints each network's
 * SSID, RSSI, channel, and encryption type. Disconnects WiFi after scanning.
 *
 * @return Number of networks found, or -1 on scan failure.
 */
int wifi_scan();

#endif
