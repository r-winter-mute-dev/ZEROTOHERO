// Kampagne 2, Mission 3.4: Rechtzeitig halten
// Board: ESP32 Dev Module. Akku-Betrieb wie ab Mission 2.2. Fährt auf dem Boden!
// Aufbau: freie Bahn von gut 1 m, am Ende eine feste, glatte Wand (Karton, Buch, Tür). Keine Menschen oder Tiere als Ziel.
//
// Motoren: AIN1 25, AIN2 26 (links)  BIN1 32, BIN2 33 (rechts)  nSLEEP 27
// Ultraschall: Trig 18, Echo 19       Teiler an VM: GPIO 34
//
// Der Roboter fährt geradeaus, bis der Abstand unter HALT_CM fällt, und bremst dann.
// Danach misst er, wie weit er tatsächlich vor der Wand steht.
// Auslesen wie in Mission 2.2: Akku AUS, 3V3-Kabel vom Wandler ab, USB an.

#include <Preferences.h>
Preferences speicher;

const int AIN1 = 25, AIN2 = 26, BIN1 = 32, BIN2 = 33, SLEEP = 27, VM_PIN = 34;
const int TRIG = 18, ECHO = 19;
const int ANLAUF = 85;        // aus Mission 1.5
const int FAHRT = 40;         // Prozent
const int HALT_CM = 20;       // dein Anhalteweg aus Mission 3.4 plus Reserve
const unsigned long MAX_FAHRT_MS = 6000;   // Sicherheit: spätestens dann anhalten

// Schritt 1: NUR_FAHREN = true. Der Roboter fährt genau 3 s mit FAHRT % und bremst.
//            Miss mit dem Maßband, wie weit er gekommen ist: Geschwindigkeit = Strecke / 3 s.
// Schritt 2: HALT_CM aus Mission 3.4 eintragen, NUR_FAHREN = false, erneut hochladen.
const bool NUR_FAHREN = true;

int prozentZuPwm(int p) { if (p == 0) return 0; int b = map(abs(p), 1, 100, ANLAUF, 255); return p > 0 ? b : -b; }

void pwm(int links, int rechts) {
  ledcWrite(AIN1, links > 0 ? links : 0);  ledcWrite(AIN2, links < 0 ? -links : 0);
  ledcWrite(BIN1, rechts > 0 ? rechts : 0); ledcWrite(BIN2, rechts < 0 ? -rechts : 0);
}
void bremsen() { ledcWrite(AIN1, 255); ledcWrite(AIN2, 255); ledcWrite(BIN1, 255); ledcWrite(BIN2, 255); }

int abstandMm() {
  digitalWrite(TRIG, LOW); delayMicroseconds(4);
  digitalWrite(TRIG, HIGH); delayMicroseconds(12); digitalWrite(TRIG, LOW);
  unsigned long us = pulseIn(ECHO, HIGH, 30000);
  if (us == 0) return -1;
  return (int)(343.4 * us / 2000.0);   // 20 °C
}

void setup() {
  Serial.begin(115200);
  delay(500);
  speicher.begin("r34", false);
  pinMode(VM_PIN, INPUT);
  if (analogReadMilliVolts(VM_PIN) * 2 < 2500) {   // nur USB: Ergebnis zeigen
    int h = speicher.getInt("halt", -1), e = speicher.getInt("ende", -1);
    if (h < 0) Serial.println("Noch nicht gefahren. Erst mit Akku auf dem Boden laufen lassen.");
    else {
      Serial.printf("Haltabstand eingestellt: %d cm, gestanden bei %d mm\n", h, e);
      Serial.printf("KW-CHECK:r-3-4:%012llX:%d:%d\n", (unsigned long long)ESP.getEfuseMac(), h, e);
    }
    speicher.end();
    return;
  }

  pinMode(TRIG, OUTPUT); pinMode(ECHO, INPUT);
  pinMode(SLEEP, OUTPUT); digitalWrite(SLEEP, HIGH); delay(2);
  ledcAttach(AIN1, 20000, 8); ledcAttach(AIN2, 20000, 8);
  ledcAttach(BIN1, 20000, 8); ledcAttach(BIN2, 20000, 8);
  pwm(0, 0);
  delay(5000);   // Zeit, den Roboter abzusetzen

  int v = prozentZuPwm(FAHRT);
  if (NUR_FAHREN) {          // Geschwindigkeit messen: 3 s fahren, dann bremsen
    pwm(v, v);
    delay(3000);
    bremsen(); delay(800); pwm(0, 0);
    speicher.end();
    return;
  }
  pwm(v, v);
  unsigned long start = millis();
  while (millis() - start < MAX_FAHRT_MS) {
    int mm = abstandMm();
    if (mm > 0 && mm < HALT_CM * 10) break;   // kein Echo (-1) zählt NICHT als "nah"
    delay(30);
  }
  bremsen();
  delay(800);
  pwm(0, 0);

  // Endabstand: Median aus fünf Messungen
  int a[5];
  for (int i = 0; i < 5; i++) { a[i] = abstandMm(); delay(70); }
  for (int i = 0; i < 5; i++) for (int j = i + 1; j < 5; j++) if (a[j] < a[i]) { int t = a[i]; a[i] = a[j]; a[j] = t; }
  speicher.putInt("halt", HALT_CM);
  speicher.putInt("ende", a[2]);
  speicher.end();
}

void loop() {}
