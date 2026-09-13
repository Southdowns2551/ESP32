/**
 * @file ble_manager.cpp
 * @brief BLE GATT server implementation.
 *
 * Creates a single-service BLE server with one read/write characteristic.
 * Handles client connect/disconnect callbacks and restarts advertising
 * when a client disconnects.
 */

#include "ble_manager.h"
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

static BLECharacteristic* _characteristic = nullptr;
static bool               _client_connected = false;
static String             _received_value = "";

class ServerCallbacks : public BLEServerCallbacks {
    /**
     * @brief Tracks BLE client connection state and restarts
     *        advertising on disconnect so new clients can find the device.
     */
    void onConnect(BLEServer* server) override {
        _client_connected = true;
        Serial.println("[BLE] Client connected");
    }

    void onDisconnect(BLEServer* server) override {
        _client_connected = false;
        Serial.println("[BLE] Client disconnected — restarting advertising");
        BLEDevice::startAdvertising();
    }
};

class CharacteristicCallbacks : public BLECharacteristicCallbacks {
    /**
     * @brief Captures values written by a BLE client into _received_value.
     */
    void onWrite(BLECharacteristic* characteristic) override {
        std::string value = characteristic->getValue();
        if (value.length() > 0) {
            _received_value = String(value.c_str());
            Serial.printf("[BLE] Received: %s\n", value.c_str());
        }
    }
};

void ble_init(const char* device_name) {
    /**
     * @brief Stand up the full BLE stack: device → server → service →
     *        characteristic → advertising.
     *
     * @param device_name  Broadcast name visible to scanning clients.
     */
    BLEDevice::init(device_name);

    BLEServer* server = BLEDevice::createServer();
    server->setCallbacks(new ServerCallbacks());

    BLEService* service = server->createService(SERVICE_UUID);

    _characteristic = service->createCharacteristic(
        CHARACTERISTIC_UUID,
        BLECharacteristic::PROPERTY_READ  |
        BLECharacteristic::PROPERTY_WRITE |
        BLECharacteristic::PROPERTY_NOTIFY
    );

    _characteristic->addDescriptor(new BLE2902());
    _characteristic->setCallbacks(new CharacteristicCallbacks());
    _characteristic->setValue("Hello from ESP32U");

    service->start();

    BLEAdvertising* advertising = BLEDevice::getAdvertising();
    advertising->addServiceUUID(SERVICE_UUID);
    advertising->setScanResponse(true);
    advertising->setMinPreferred(0x06);
    advertising->setMinPreferred(0x12);
    BLEDevice::startAdvertising();

    Serial.printf("[BLE] Server started — advertising as \"%s\"\n", device_name);
}

String ble_get_received_value() {
    /**
     * @brief Return and clear the last value written by a BLE client.
     * @return The received string, or "" if nothing new.
     */
    String val = _received_value;
    _received_value = "";
    return val;
}

void ble_set_value(const String& value) {
    /**
     * @brief Update the characteristic value readable by BLE clients.
     * @param value  New string to expose.
     */
    if (_characteristic) {
        _characteristic->setValue(value.c_str());
        if (_client_connected) {
            _characteristic->notify();
        }
    }
}

bool ble_is_client_connected() {
    /**
     * @brief Check BLE client connection state.
     * @return true when a client is connected.
     */
    return _client_connected;
}
