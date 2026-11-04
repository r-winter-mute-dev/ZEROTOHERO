// Klingelwerk – Mission 2.6: Eingang
// Liest den Klingelknopf an GPIO 4. Absichtlich OHNE Entprellen.
// Aufbau: GPIO 4 -> Taster -> GND (der Pull-up ist im ESP32 eingebaut)

const int TASTER_PIN = 4;
int vorher = HIGH;
int zaehler = 0;
bool gemeldet = false;

void setup() {
  Serial.begin(115200);
  pinMode(TASTER_PIN, INPUT_PULLUP);  // ohne Druck: HIGH, gedrückt: LOW
  Serial.println("Drücke den Taster einige Male.");
}

void loop() {
  int jetzt = digitalRead(TASTER_PIN);
  if (vorher == HIGH && jetzt == LOW) {   // Übergang von HIGH nach LOW = Druck beginnt
    zaehler++;
    Serial.print("Gedrückt! Zähler: ");
    Serial.println(zaehler);
  }
  vorher = jetzt;

  if (zaehler >= 10 && !gemeldet) {
    Serial.printf("KW-CHECK:m-2-6:%012llX\n", (unsigned long long)ESP.getEfuseMac());
    gemeldet = true;
  }
}
