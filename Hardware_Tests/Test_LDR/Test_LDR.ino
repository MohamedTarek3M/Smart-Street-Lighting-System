void setup() {
  Serial.begin(115200);
  // LDR_PIN is 34 (Analog)
  Serial.println("LDR Sensor Test Started");
}

void loop() {
  int ldrValue = analogRead(34);
  Serial.print("LDR Value: ");
  Serial.println(ldrValue);
  delay(500);
}
