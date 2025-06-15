#include <ArduinoBLE.h>
#include "DistanceParser.h"

// Replace these with your sensor's BLE Service and Characteristic UUIDs
#define SENSOR_SERVICE_UUID "FFE0"
#define SENSOR_CHARACTERISTIC_UUID "FFE1"

BLEDevice peripheral;
BLECharacteristic sensorCharacteristic;
DistanceParser distanceParser;

void setup() {
  Serial.begin(115200);
  while (!Serial);

  if (!BLE.begin()) {
    Serial.println("Failed to initialize BLE!");
    BLE.end();
    delay(100);
    while (1);
  }

  Serial.println("Starting BLE scan...");
  BLE.scan();
}

void loop() {
  if (!peripheral.connected()) {
    peripheral = BLE.available();
    if (peripheral) {
      Serial.print("Connecting to: ");
      Serial.println(peripheral.address());
      BLE.stopScan();

      if (peripheral.connect()) {
        Serial.println("Connected!");
        if (peripheral.discoverAttributes()) {
          sensorCharacteristic = peripheral.characteristic(SENSOR_CHARACTERISTIC_UUID);
          if (sensorCharacteristic) {
            if (sensorCharacteristic.canSubscribe()) {
              sensorCharacteristic.subscribe();
              Serial.println("Subscribed to sensor notifications.");
            }
          } else {
            Serial.println("Sensor characteristic not found.");
            peripheral.disconnect();
          }
        } else {
          Serial.println("Failed to discover attributes.");
          peripheral.disconnect();
        }
      } else {
        Serial.println("Failed to connect.");
      }
      BLE.scan();  // Continue scanning if not connected
    }
  } else {
    if (sensorCharacteristic) {
      if (sensorCharacteristic.valueUpdated()) {
        uint8_t buffer[20];
        int length = sensorCharacteristic.readValue(buffer, sizeof(buffer));
        distanceParser.appendData(buffer, length);
      }
    }

    BLE.poll();  // Allow BLE stack to process events
  }
}
