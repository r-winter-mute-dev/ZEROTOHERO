// 0 to Hero – Mission 2.5: Ausgang
// Schaltet eine externe LED an GPIO 23 alle zwei Sekunden um.
// Aufbau: GPIO 23 -> 220-Ohm-Widerstand -> LED (langes Bein) -> LED (kurzes Bein) -> GND

const int LED_PIN = 23;

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
}

void loop() {
  digitalWrite(LED_PIN, HIGH);
  Serial.println("LED an (GPIO 23 = HIGH, jetzt messen)");
  delay(2000);

  digitalWrite(LED_PIN, LOW);
  Serial.println("LED aus (GPIO 23 = LOW)");
  delay(2000);

  Serial.printf("KW-CHECK:m-2-5:%012llX\n", (unsigned long long)ESP.getEfuseMac());
}
