// Klingelwerk – Mission 2.4: Programmieren II (Schleifen und Funktionen)
// Ein Klingelmuster: dreimal kurz, einmal lang.

const int LED_PIN = 2;
int runde = 0;

// Eigene Funktion: lässt die LED "anzahl"-mal blinken, jeweils "dauer" Millisekunden
void blinke(int anzahl, int dauer) {
  for (int i = 0; i < anzahl; i++) {
    digitalWrite(LED_PIN, HIGH);
    delay(dauer);
    digitalWrite(LED_PIN, LOW);
    delay(dauer);
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
}

void loop() {
  blinke(3, 150);  // dreimal kurz
  delay(400);
  blinke(1, 600);  // einmal lang
  delay(1500);

  runde++;
  Serial.print("Klingelmuster Runde ");
  Serial.println(runde);
  if (runde == 3) {
    Serial.printf("KW-CHECK:m-2-4:%012llX\n", (unsigned long long)ESP.getEfuseMac());
  }
}
