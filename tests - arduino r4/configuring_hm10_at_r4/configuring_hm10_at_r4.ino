#include <SoftwareSerial.h>

SoftwareSerial BTserial(2, 3); // RX, TX (to HM-10 TX and RX)

void setup() {
  Serial.begin(115200);      // USB Serial Monitor
  BTserial.begin(115200);    // HM-10 baud rate (adjust if needed)
  Serial.println("Enter AT commands:");
}

void loop() {
  if (BTserial.available()) {
    Serial.write(BTserial.read());
  }
  if (Serial.available()) {
    BTserial.write(Serial.read());
  }
}
