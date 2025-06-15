void setup() {
  Serial.begin(115200);     // Monitor
  Serial1.begin(115200);    // HM-10 (Use TX1/RX1 on Mega)

  Serial.println("Enter AT commands:");
}

void loop() {
  if (Serial1.available()) {
    Serial.write(Serial1.read());
  }
  if (Serial.available()) {
    Serial1.write(Serial.read());
  }
}
