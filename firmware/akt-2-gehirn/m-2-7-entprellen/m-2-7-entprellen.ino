// Klingelwerk – Mission 2.7: Entprellen
// Zählt jeden echten Tastendruck genau einmal.
// Aufbau wie in Mission 2.6: GPIO 4 -> Taster -> GND

const int TASTER_PIN = 4;
const unsigned long ENTPRELLZEIT = 30;  // Millisekunden, in denen der Pegel stabil sein muss

int letzterPegel = HIGH;        // was zuletzt gelesen wurde
int stabilerZustand = HIGH;     // was als gültig gilt
unsigned long letzteAenderung = 0;
int zaehler = 0;

void setup() {
  Serial.begin(115200);
  pinMode(TASTER_PIN, INPUT_PULLUP);
  Serial.println("Drücke den Taster genau 10-mal.");
}

void loop() {
  int jetzt = digitalRead(TASTER_PIN);

  if (jetzt != letzterPegel) {          // der Pegel hat sich geändert (vielleicht nur Prellen)
    letzteAenderung = millis();
    letzterPegel = jetzt;
  }

  if (millis() - letzteAenderung > ENTPRELLZEIT && jetzt != stabilerZustand) {
    stabilerZustand = jetzt;            // lange genug stabil: neuer gültiger Zustand
    if (stabilerZustand == LOW) {
      zaehler++;
      Serial.print("Druck ");
      Serial.println(zaehler);
      if (zaehler == 10) {
        Serial.printf("KW-CHECK:m-2-7:%012llX\n", (unsigned long long)ESP.getEfuseMac());
      }
    }
  }
}
