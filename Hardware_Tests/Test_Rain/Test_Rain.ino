void setup() {
  Serial.begin(115200);
  pinMode(25, INPUT_PULLUP); // RAIN_PIN
  Serial.println("Rain Sensor Test Started");
}

void loop() {
  // Rain sensor typically goes LOW when rain is detected
  bool raining = (digitalRead(25) == LOW);
  if (raining) {
    Serial.println("Rain Detected! (LOW)");
  } else {
    Serial.println("Dry (HIGH)");
  }
  delay(500);
}
