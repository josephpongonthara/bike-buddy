#include <ArduinoBLE.h>
#include <Arduino.h>
#include "cmsis_gcc.h"

const char* HM10address = "18:93:d7:49:2b:72";

void setup() {
  Serial.begin(115200);
  while (!Serial);

  if (!BLE.begin()) {
    Serial.println("Starting BLE failed. Resetting...");
    BLE.end();
    delay(100);
    NVIC_SystemReset();
  }

  Serial.println("Scanning for HM-10...");
  BLE.scan(); 
  Serial1.begin(115200);
}

void loop() {
  BLEDevice peripheral = BLE.available();

  if (peripheral) {
    Serial.println("Peripheral found:");
    Serial.print("Address: "); Serial.println(peripheral.address());
    Serial.print("RSSI: "); Serial.println(peripheral.rssi());

    if (peripheral.address() == HM10address) {
      Serial.println("HM-10 found! Connecting...");
      BLE.stopScan();
      if (peripheral.connect()) {
        Serial.println("Connected. Discovering services...");
        if (!peripheral.discoverAttributes()) {
          Serial.println("Attribute discovery failed. Resetting...");
          delay(500);
          NVIC_SystemReset();
        }

        BLEService service = peripheral.service("FFE0");
        if (service) {
          Serial.println("Service FFE0 found!");
          BLECharacteristic characteristic = service.characteristic("FFE1");
          if (characteristic) {
            Serial.println("Characteristic FFE1 found!");

            // Start exploring the characteristic stream:
            exploreCharacteristic(characteristic, peripheral);

          } else {
            Serial.println("Characteristic FFE1 not found.");
          }
        } else {
          Serial.println("Service FFE0 not found.");
        }

        peripheral.disconnect();
        Serial.println("Disconnected. Restarting scan.");
        BLE.scan();
      } else {
        Serial.println("Failed to connect to peripheral.");
      }
    }
  }
}

void exploreCharacteristic(BLECharacteristic characteristic, BLEDevice peripheral) {
  Serial.println("Starting characteristic data exploration...");
  
  while (peripheral.connected()) {
    if (characteristic.canRead()) {
      characteristic.read(); // Refresh characteristic value

      int length = characteristic.valueLength();
      if (length > 0) {
        if (length > 42) length = 42; // safety limit

        uint8_t dataHex[42] = {0};
        characteristic.readValue(dataHex, length);

        Serial.print("Received [");
        Serial.print(length);
        Serial.print(" bytes]: ");

        for (int i = 0; i < length; i++) {
          if (dataHex[i] < 16) Serial.print("0");  // Pad single-digit hex with 0
          Serial.print(dataHex[i], HEX);
          Serial.print(" ");
        }
        Serial.println();
      }
    }

    delay(100); // adjust delay to control print rate
  }

  Serial.println("Peripheral disconnected or exploration ended.");
}
