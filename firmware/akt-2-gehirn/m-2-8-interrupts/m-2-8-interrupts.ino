// Klingelwerk – Mission 2.8: Interrupts
// Das Hauptprogramm ist absichtlich langsam (zwei Sekunden pro Durchlauf).
// Trotzdem geht kein Tastendruck verloren, weil ein Interrupt ihn sofort merkt.
// Aufbau wie in Mission 2.6: GPIO 4 -> Taster -> GND

const int TASTER_PIN = 4;

volatile bool gedrueckt = false;          // wird im Interrupt geändert, deshalb "volatile"
volatile unsigned long letzterDruck = 0;
int zaehler = 0;

// Interrupt-Routine: so kurz wie möglich, nur ein Merker setzen
void IRAM_ATTR beimDruecken() {
  unsigned long t = millis();
  if (t - letzterDruck > 200) {           // einfaches Entprellen: 200 ms Sperrzeit
    gedrueckt = true;
    letzterDruck = t;
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(TASTER_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(TASTER_PIN), beimDruecken, FALLING);
  Serial.println("Drücke den Taster 5-mal, auch während das Programm wartet.");
}

void loop() {
  Serial.println("Beschäftigt …");
  delay(2000);                            // simuliert ein langsames Hauptprogramm

  if (gedrueckt) {
    gedrueckt = false;
    zaehler++;
    Serial.print("Klingel! Druck ");
    Serial.println(zaehler);
    if (zaehler == 5) {
      Serial.printf("KW-CHECK:m-2-8:%012llX\n", (unsigned long long)ESP.getEfuseMac());
    }
  }
}
