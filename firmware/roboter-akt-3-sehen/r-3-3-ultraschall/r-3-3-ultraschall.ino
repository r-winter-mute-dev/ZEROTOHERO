// Kampagne 2, Mission 3.3: Ultraschall
// Board: ESP32 Dev Module. Läuft am USB, die Motoren bleiben aus.
//
// HC-SR04P oder RCWL-1601 (Versorgung 3 bis 5,5 V):
//   VCC -> 3V3   GND -> GND   Trig -> GPIO 18   Echo -> GPIO 19
// Kein 5-V-HC-SR04 ohne Spannungsteiler: Sein Echo-Signal hätte 5 V (siehe Mission 3.3).

const int TRIG = 18, ECHO = 19;
const float TEMPERATUR = 20.0;                         // °C, trag deine Raumtemperatur ein
const float SCHALL = 331.3 + 0.606 * TEMPERATUR;       // m/s, Näherung für trockene Luft

// Gibt den Abstand in Millimetern zurück, oder -1, wenn kein Echo kam.
int abstandMm() {
  digitalWrite(TRIG, LOW);
  delayMicroseconds(4);
  digitalWrite(TRIG, HIGH);                            // Messung starten: mindestens 10 µs HIGH
  delayMicroseconds(12);
  digitalWrite(TRIG, LOW);
  unsigned long us = pulseIn(ECHO, HIGH, 30000);        // höchstens 30 ms warten (gut 5 m hin und zurück)
  if (us == 0) return -1;
  return (int)(SCHALL * us / 2000.0);                  // m/s × µs / 2 / 1000 = mm
}

void setup() {
  Serial.begin(115200);
  delay(500);
  pinMode(TRIG, OUTPUT); pinMode(ECHO, INPUT);
  Serial.println();
  Serial.printf("Mission 3.3: Ultraschall. Schallgeschwindigkeit bei %.0f °C: %.1f m/s\n", TEMPERATUR, SCHALL);
}

int gueltig = 0;
int letzte[5];

void loop() {
  int mm = abstandMm();
  if (mm < 0) Serial.println("kein Echo (zu weit, zu schräg oder zu weich)");
  else {
    Serial.printf("Abstand: %4d mm\n", mm);
    letzte[gueltig % 5] = mm;
    if (++gueltig == 20) {
      // Median der letzten fünf Messungen
      int a[5]; for (int i = 0; i < 5; i++) a[i] = letzte[i];
      for (int i = 0; i < 5; i++) for (int j = i + 1; j < 5; j++) if (a[j] < a[i]) { int t = a[i]; a[i] = a[j]; a[j] = t; }
      Serial.printf("KW-CHECK:r-3-3:%012llX:%d\n", (unsigned long long)ESP.getEfuseMac(), a[2]);
    }
  }
  delay(100);   // Pause, damit sich das alte Echo nicht in die nächste Messung mischt
}
