/**
 * @file ble_manager.h
 * @brief BLE server manager for ESP32.
 *
 * Sets up a BLE GATT server with a single service containing one
 * read/write characteristic, suitable for basic IoT communication.
 */

#ifndef BLE_MANAGER_H
#define BLE_MANAGER_H

#include <Arduino.h>

/**
 * @brief Initialize the BLE stack and start advertising.
 *
 * Creates a GATT server with one service (UUID defined internally)
 * and one read/write characteristic. Begins advertising immediately.
 *
 * @param device_name  Name broadcast during BLE advertising.
 */
void ble_init(const char* device_name);

/**
 * @brief Get the last value written to the BLE characteristic by a client.
 *
 * @return String containing the value, or empty string if nothing received.
 */
String ble_get_received_value();

/**
 * @brief Set the value of the BLE characteristic (readable by clients).
 *
 * @param value  The string value to expose via BLE.
 */
void ble_set_value(const String& value);

/**
 * @brief Return whether a BLE client is currently connected.
 *
 * @return true if a client is connected, false otherwise.
 */
bool ble_is_client_connected();

#endif
