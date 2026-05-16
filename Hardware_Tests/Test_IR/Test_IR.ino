void setup() {
  Serial.begin(115200);
  pinMode(26, INPUT_PULLUP); // IR_PIN
  Serial.println("IR Obstacle Sensor Test Started");
}

void loop() {
  // IR Obstacle sensor goes LOW when obstacle is detected
  bool obstacle = (digitalRead(26) == LOW); 
  if (obstacle) {
    Serial.println("Obstacle Detected! (LOW)");
  } else {
    Serial.println("Clear (HIGH)");
  }
  delay(500);
}
