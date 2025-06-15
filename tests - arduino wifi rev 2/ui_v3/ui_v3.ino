#include <Adafruit_GFX.h> 
#include <Adafruit_ILI9341.h>
#include <ArduinoBLE.h>
#include <avr/io.h> 

#define tft_screen_CS    10
#define tft_screen_DC    9
#define tft_screen_RST   8 

Adafruit_ILI9341 tft_screen = Adafruit_ILI9341(tft_screen_CS, tft_screen_DC, tft_screen_RST);

#define	BLACK   0x0000
#define	RED     0xF800
#define WHITE   0xFFFF

const char* HM10address = "18:93:d7:49:2b:72";

void drawDangerScreen() {
  tft_screen.fillScreen(RED);
  tft_screen.setTextSize(3);
  tft_screen.setCursor(50, 100);
  tft_screen.print("CHECK!");
}


void setup() {
  tft_screen.begin();
  tft_screen.setRotation(1);  
  BLE.begin();
  BLE.scan();
  Serial1.begin(115200);
  delay(100);
}

void loop() {

  BLEDevice peripheral = BLE.available();

  if (peripheral){
    if (peripheral.address() == HM10address) {
      BLE.stopScan();
      peripheral.connect();
      delay(500);
      if (peripheral.discoverAttributes() == 0){
        _PROTECTED_WRITE(RSTCTRL.SWRR,1);
      }
      BLEService service = peripheral.service("FFE0");
      if (service) {
        BLECharacteristic characteristic = service.characteristic("FFE1"); // HM-10 TX Characteristic
        if (characteristic) {
          uint8_t command[] = {0x00, 0x11, 0x02, 0x4C};
          characteristic.writeValue(command, sizeof(command));
          delay(100);
          uint8_t buffer[4];
          while (true) {
            if (characteristic.readValue(buffer, sizeof(buffer))){
              uint8_t header = buffer[0];
              uint8_t highByte = buffer[1];
              uint8_t lowByte = buffer[2];
              uint8_t checksum = buffer[3];
              if (checksum == 0x54){
                continue;
              } 
              else {
                uint16_t distance = (highByte << 8) | lowByte; 
                float threshold = 1000;
                if (distance <= threshold){
                  drawDangerScreen();
                }
              }
              delay(200);
            }
          }
        }
      }
    }
  }
}

