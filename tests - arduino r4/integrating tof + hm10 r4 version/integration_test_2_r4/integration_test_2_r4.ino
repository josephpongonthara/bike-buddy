#include <ArduinoBLE.h>
#include <Arduino.h>
#include "cmsis_gcc.h"

const char* HM10address = "18:93:d7:49:2b:72";

void setup() {
  Serial.begin(115200);
  while (!Serial);

  if (!BLE.begin()) {
    Serial.println("Starting Bluetooth® Low Energy module failed!");
    BLE.end();
    delay(100);
    NVIC_SystemReset();
    while (1);
  }

  Serial.println("Scanning for HM-10...");
  BLE.scan(); 
  Serial1.begin(115200);  
  delay(100);
}

void loop() {
  BLEDevice peripheral = BLE.available();

  if (peripheral) {
    Serial.println("Discovered a peripheral");
    Serial.print("Address: ");
    Serial.println(peripheral.address());
    Serial.print("RSSI: ");
    Serial.println(peripheral.rssi());

    if (peripheral.address() == HM10address) {
      Serial.println("HM-10 found!");
      BLE.stopScan();
      peripheral.connect();
      delay(500);

      Serial.println("Discovering services...");
      peripheral.discoverAttributes();
      if (peripheral.discoverAttributes() == 0) {
        Serial.println("Failed to discover attributes. Restarting...");
        delay(1000);
        NVIC_SystemReset();
      } else {
        Serial.println("Services discovered");
      }

      BLEService service = peripheral.service("FFE0"); // HM-10 Service UUID
      if (service) {
        Serial.println("Service FFE0 found!");
        BLECharacteristic characteristic = service.characteristic("FFE1"); // HM-10 TX Characteristic
        if (characteristic) {
          Serial.println("Characteristic FFE1 found!");

          // Switch to text mode:
          uint8_t commandTextMode[] = {0x00, 0x11, 0x01, 0x45};
          characteristic.writeValue(commandTextMode, sizeof(commandTextMode));
          Serial.println("Sent command to switch to Text mode");
          delay(1000);

          String buffer = "";
          while (true) {
            // Attempt to read new text data
            if (characteristic.canRead()) {
              int len = characteristic.valueLength();
              if (len > 0) {
                uint8_t data[len];
                if (characteristic.readValue(data, len)) {
                  for (int i = 0; i < len; i++) {
                    char c = (char)data[i];
                    if (c == '\n') {
                      // End of line received, parse distance
                      parseAndPrintDistance(buffer);
                      buffer = "";
                    } else {
                      buffer += c;
                    }
                  }
                }
              }
            }
            delay(100);
          }
        } else {
          Serial.println("Characteristic not found.");
        }
      } else {
        Serial.println("Service not found");
      }
    }
  }
}

void parseAndPrintDistance(String data) {
  data.trim(); // Clean up white space
  Serial.print("Distance: ");
  Serial.print(data);
  Serial.println(" mm");
  /*
  if (data == "-1") {
    Serial.println("Error: Unable to measure distance");
  } else if (data == "-Inf") {
    Serial.println("Target too close");
  } else if (data == "+Inf") {
    Serial.println("Target too far");
  } else {
    Serial.print("Distance: ");
    Serial.print(data);
    Serial.println(" mm");
  }
  */
}
