// Kampagne 2, Mission 2.2: Belastungstest für die gemeinsame Versorgung
// Board: ESP32 Dev Module. Räder aufbocken.
//
// Versorgung (Akku-Betrieb, USB dabei NICHT eingesteckt):
//   18650 -> Lademodul (TP4056 mit DW01) -> OUT+ / OUT-
//   OUT+ -> Schalter -> DRV8833 VM   und   -> Buck-Boost 3,3 V IN+ -> 3V3-Pin des ESP32
//   OUT- -> GND überall (Treiber, Wandler, ESP32)
//   Elko 1000 µF direkt an VM und GND des Treibers (Streifen = Minus an GND)
// Messung:
//   Teiler 10 kΩ / 10 kΩ von VM nach GND, Mitte an GPIO 34 (nur Eingang, ADC1)
// Motoren: AIN1 25, AIN2 26 (links)  BIN1 32, BIN2 33 (rechts)  nSLEEP 27
//
// Der Sketch startet beide Motoren zehnmal schlagartig mit voller Kraft und misst
// dabei, wie tief die Spannung an VM einbricht. Neustarts durch Unterspannung
// (Brownout) werden mitgezählt. Das Ergebnis landet im Speicher des ESP32.
// Auslesen: Akku AUS, Kabel vom Wandler zum 3V3-Pin abziehen, dann USB anstecken.
// Ohne Spannung an VM misst der Sketch nichts, sondern zeigt nur das gespeicherte Ergebnis.

#include <Preferences.h>
#include "esp_system.h"

const int AIN1 = 25, AIN2 = 26, BIN1 = 32, BIN2 = 33, SLEEP = 27;
const int VM_PIN = 34;
const float TEILER = 2.0;   // 10 kΩ / 10 kΩ

Preferences speicher;

void motoren(int links, int rechts) {
  links = constrain(links, -255, 255); rechts = constrain(rechts, -255, 255);
  ledcWrite(AIN1, links > 0 ? links : 0);  ledcWrite(AIN2, links < 0 ? -links : 0);
  ledcWrite(BIN1, rechts > 0 ? rechts : 0); ledcWrite(BIN2, rechts < 0 ? -rechts : 0);
}

int vmMillivolt() { return (int)(analogReadMilliVolts(VM_PIN) * TEILER); }

void ergebnisZeigen() {
  int e = speicher.getInt("einbruch", -1), n = speicher.getInt("brownout", 0);
  if (e < 0) { Serial.println("Noch kein Ergebnis gespeichert. Erst mit Akku laufen lassen."); return; }
  Serial.printf("Gespeichertes Ergebnis: größter Einbruch %d mV, Neustarts durch Unterspannung %d\n", e, n);
  Serial.printf("KW-CHECK:r-2-2:%012llX:%d:%d\n", (unsigned long long)ESP.getEfuseMac(), e, n);
}

void setup() {
  Serial.begin(115200);
  delay(500);
  speicher.begin("r22", false);
  pinMode(VM_PIN, INPUT);
  Serial.println();
  Serial.println("Mission 2.2: Belastungstest");

  // Keine Motorspannung? Dann hängt nur USB dran: Ergebnis zeigen und fertig.
  if (vmMillivolt() < 2500) { ergebnisZeigen(); speicher.end(); return; }

  // Neustarts zählen: Einschalten beginnt einen neuen Test, Brownout zählt hoch.
  int neustarts = speicher.getInt("brownout", 0);
  esp_reset_reason_t grund = esp_reset_reason();
  if (grund == ESP_RST_POWERON) { neustarts = 0; speicher.putInt("einbruch", -1); }
  if (grund == ESP_RST_BROWNOUT) neustarts++;
  speicher.putInt("brownout", neustarts);

  pinMode(SLEEP, OUTPUT); digitalWrite(SLEEP, HIGH); delay(2);
  ledcAttach(AIN1, 20000, 8); ledcAttach(AIN2, 20000, 8);
  ledcAttach(BIN1, 20000, 8); ledcAttach(BIN2, 20000, 8);
  motoren(0, 0);
  delay(3000);   // Zeit, die Hand wegzunehmen

  int groesster = 0;
  for (int runde = 1; runde <= 10; runde++) {
    int ruhe = vmMillivolt();
    int minimum = ruhe;
    motoren(255, 255);                       // schlagartig volle Kraft
    unsigned long start = millis();
    while (millis() - start < 400) {         // 400 ms lang so oft wie möglich messen
      int u = vmMillivolt();
      if (u < minimum) minimum = u;
    }
    motoren(0, 0);
    int einbruch = ruhe - minimum;
    if (einbruch > groesster) groesster = einbruch;
    Serial.printf("Start %2d: Ruhe %4d mV, Minimum %4d mV, Einbruch %4d mV\n", runde, ruhe, minimum, einbruch);
    delay(1200);
  }
  speicher.putInt("einbruch", groesster);
  ergebnisZeigen();
  speicher.end();
}

void loop() {}
