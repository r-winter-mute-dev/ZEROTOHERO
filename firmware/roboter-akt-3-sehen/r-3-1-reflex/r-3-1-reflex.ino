// Kampagne 2, Mission 3.1: Einen Reflexsensor lesen
// Board: ESP32 Dev Module. Läuft am USB, die Motoren bleiben aus.
// Denk an die Programmier-Regel aus Mission 2.2: Akku AUS und 3V3-Kabel vom Wandler ab, bevor USB dran kommt.
//
// TCRT5000-Modul:  VCC -> 3V3   GND -> GND   AO -> GPIO 36 (am Board oft "VP")
// DO (digitaler Ausgang) bleibt vorerst frei.
// Der Sketch gibt fünfmal pro Sekunde die Spannung an AO aus.

const int SENSOR = 36;   // ADC1, nur Eingang

void setup() {
  Serial.begin(115200);
  delay(500);
  pinMode(SENSOR, INPUT);
  Serial.println();
  Serial.println("Mission 3.1: Reflexsensor. Halte weißes Papier und schwarzes Klebeband darunter.");
}

int anzahl = 0;

void loop() {
  // Zehn Messungen mitteln, das glättet das Rauschen.
  long summe = 0;
  for (int i = 0; i < 10; i++) summe += analogReadMilliVolts(SENSOR);
  int mv = summe / 10;
  Serial.printf("AO: %4d mV  ", mv);
  for (int i = 0; i < mv / 100; i++) Serial.print('#');   // kleiner Balken
  Serial.println();
  if (++anzahl == 25) Serial.printf("KW-CHECK:r-3-1:%012llX\n", (unsigned long long)ESP.getEfuseMac());
  delay(200);
}
