#include <ArduinoBLE.h>
#include <avr/io.h>

const char* HM10address = "18:93:d7:49:2b:72";

void setup() {
  Serial.begin(115200);
  while (!Serial);

  if (!BLE.begin()) {
    Serial.println("Starting Bluetooth® Low Energy module failed!");
    while (1);
  }

  Serial.println("Scanning for HM-10...");
  BLE.scan(); 
  Serial1.begin(115200);  
  delay(100);
}

void loop() {

  BLEDevice peripheral = BLE.available();

  if (peripheral){
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
      Serial.println(peripheral.discoverAttributes());
      if (peripheral.discoverAttributes() == 0){
        Serial.println("Failed to discover attributes. Restarting ....");
        delay(1000);
        _PROTECTED_WRITE(RSTCTRL.SWRR,1);
      }
      else{
        Serial.println("Services discovered");
      }
      BLEService service = peripheral.service("FFE0"); // HM-10 Service UUID
      if (service) {
        Serial.println("Service FFE0 found!");
        BLECharacteristic characteristic = service.characteristic("FFE1"); // HM-10 TX Characteristic
        if (characteristic) {
          Serial.println("Characteristic FFE1 found!");
          uint8_t command[] = {0x00, 0x11, 0x02, 0x4C};
          characteristic.writeValue(command, sizeof(command));
          Serial.println("Sent command to switch to Binary mode");
          delay(1000);

          uint8_t buffer[4];

          while (true) {
            if (characteristic.readValue(buffer, sizeof(buffer))){
              uint8_t header = buffer[0];
              uint8_t highByte = buffer[1];
              uint8_t lowByte = buffer[2];
              uint8_t checksum = buffer[3];
                            
              if (checksum == 0x54){
                Serial.println("Checksum is absent, restarting read");
                //uint8_t discard = characteristic.read(); // discard the remaining bytes
                //discard = characteristic.read();
                //discard = characteristic.read();
                continue;
              } 
              else {
                uint16_t distance = (highByte << 8) | lowByte; 

                Serial.print("Raw Data: ");
                Serial.print(header, HEX);
                Serial.print(" ");
                Serial.print(highByte, HEX);
                Serial.print(" ");
                Serial.print(lowByte, HEX);
                Serial.print(" ");
                Serial.print(checksum, HEX);
                Serial.println();

                if (distance == 0x0001) {
                  Serial.println("Error: Unable to measure distance");
                } else if (distance == 0x0000) {
                  Serial.println("Target too close");
                } else if (distance == 0xFFFF) {
                  Serial.println("Target too far");
                } else {
                  Serial.print("Distance: ");
                  Serial.print(distance);
                  Serial.println(" mm");
                }
              }
              delay(200);
            }
          }
        } 
        else {
              Serial.println("Characteristic not found.");
        }
      }
      else{
        Serial.print("Service not found");        
      }
    }
  }
}
