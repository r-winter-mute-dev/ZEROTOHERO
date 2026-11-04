// 0 to Hero – Mission 2.1: Erstes Leben
// Lässt die eingebaute LED blinken und meldet sich über die serielle Schnittstelle.
// Board: "ESP32 Dev Module"

const int LED_PIN = 2;  // eingebaute LED vieler ESP32-DevKits (nicht bei jedem Board vorhanden)

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
}

void loop() {
  digitalWrite(LED_PIN, HIGH);
  delay(500);
  digitalWrite(LED_PIN, LOW);
  delay(500);

  // Gerätenachweis für die Website: enthält die eindeutige Kennung deines Chips
  Serial.printf("KW-CHECK:m-2-1:%012llX\n", (unsigned long long)ESP.getEfuseMac());
}
