#define LIGHT_BOX_PIN   33
#define LIGHT_BOX_PIN2  32

void setup() {
  Serial.begin(115200);
  pinMode(LIGHT_BOX_PIN, OUTPUT);
  pinMode(LIGHT_BOX_PIN2, OUTPUT);
  digitalWrite(LIGHT_BOX_PIN2, LOW); // Ground pin
  Serial.println("Light Box LED Test Started");
}

void loop() {
  Serial.println("Light Box: ON");
  digitalWrite(LIGHT_BOX_PIN, HIGH);
  delay(2000);
  
  Serial.println("Light Box: OFF");
  digitalWrite(LIGHT_BOX_PIN, LOW);
  delay(2000);
}
