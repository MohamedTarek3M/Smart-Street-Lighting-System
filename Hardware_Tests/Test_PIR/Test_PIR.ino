void setup() {
  Serial.begin(115200);
  pinMode(27, INPUT_PULLDOWN); // PIR_PIN
  Serial.println("PIR Sensor Test Started");
}

void loop() {
  bool motion = digitalRead(27);
  if (motion) {
    Serial.println("Motion Detected! (HIGH)");
  } else {
    Serial.println("No Motion (LOW)");
  }
  delay(500);
}
