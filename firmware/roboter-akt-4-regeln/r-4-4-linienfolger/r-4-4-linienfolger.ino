// Kampagne 2, Mission 4.4: Der Linienfolger
// Board: ESP32 Dev Module. Akku-Betrieb wie ab Mission 2.2. Fährt auf dem Boden!
//
// Motoren: AIN1 25, AIN2 26 (links)  BIN1 32, BIN2 33 (rechts)  nSLEEP 27   Teiler an VM: GPIO 34
// Liniensensoren: AO links -> GPIO 36 (VP), AO rechts -> GPIO 39 (VN)
//
// Ablauf:
//   1. Roboter mittig auf die Linie stellen (Linie zwischen den Sensoren), Akku EIN.
//   2. Nach 3 s dreht er sich langsam etwas nach links und rechts und kalibriert beide Sensoren.
//   3. Nach 2 s Pause folgt er der Linie, höchstens FAHRZEIT_MS lang.
//   4. Verliert er die Linie länger als 1 s, hält er sofort an.
// Auslesen wie in Mission 2.2: Akku AUS, 3V3-Kabel vom Wandler ab, USB an.

#include <Preferences.h>
Preferences speicher;

const int AIN1 = 25, AIN2 = 26, BIN1 = 32, BIN2 = 33, SLEEP = 27, VM_PIN = 34;
const int LINKS = 36, RECHTS = 39;

// ---- Hier einstellen (Mission 4.4) ----
const int ANLAUF = 85;          // aus Mission 1.5
const float FAHRT = 25;         // Grundgeschwindigkeit in Prozent
const float KP = 1.0;           // proportional: Startwert, in Mission 4.4 erhöhen
const float KI = 0.0;           // integral
const float KD = 0.0;           // differenzial
const unsigned long FAHRZEIT_MS = 30000;
// ---------------------------------------

struct Sensor { int pin, weiss, schwarz; };
Sensor l = {LINKS}, r = {RECHTS};

int lesen(int pin) { long s = 0; for (int i = 0; i < 4; i++) s += analogReadMilliVolts(pin); return s / 4; }

// 0 = weiß, 1 = schwarz, egal ob der Sensor über Schwarz mehr oder weniger Spannung liefert
float normiert(const Sensor &s) {
  float n = (float)(lesen(s.pin) - s.weiss) / (float)(s.schwarz - s.weiss);
  return constrain(n, 0.0f, 1.0f);
}

int prozentZuPwm(float p) {
  p = constrain(p, -100.0f, 100.0f);
  if (fabs(p) < 0.5) return 0;
  int b = map((long)fabs(p), 1, 100, ANLAUF, 255);
  return p > 0 ? b : -b;
}
void fahre(float links, float rechts) {
  int a = prozentZuPwm(links), b = prozentZuPwm(rechts);
  ledcWrite(AIN1, a > 0 ? a : 0); ledcWrite(AIN2, a < 0 ? -a : 0);
  ledcWrite(BIN1, b > 0 ? b : 0); ledcWrite(BIN2, b < 0 ? -b : 0);
}
void bremsen() { ledcWrite(AIN1, 255); ledcWrite(AIN2, 255); ledcWrite(BIN1, 255); ledcWrite(BIN2, 255); delay(400); fahre(0, 0); }

void kalibrieren() {
  l.weiss = lesen(LINKS); r.weiss = lesen(RECHTS);
  int lmin = l.weiss, lmax = l.weiss, rmin = r.weiss, rmax = r.weiss;
  // links drehen, rechts drehen, zurück: jeder Sensor wandert einmal über die Linie
  const float schritte[3][2] = {{-30, 30}, {30, -30}, {-30, 30}};
  const int dauer[3] = {500, 1000, 500};
  for (int i = 0; i < 3; i++) {
    fahre(schritte[i][0], schritte[i][1]);
    unsigned long t = millis();
    while (millis() - t < (unsigned long)dauer[i]) {
      int a = lesen(LINKS), b = lesen(RECHTS);
      lmin = min(lmin, a); lmax = max(lmax, a); rmin = min(rmin, b); rmax = max(rmax, b);
    }
  }
  bremsen();
  l.schwarz = (lmax - l.weiss) > (l.weiss - lmin) ? lmax : lmin;
  r.schwarz = (rmax - r.weiss) > (r.weiss - rmin) ? rmax : rmin;
}

void setup() {
  Serial.begin(115200);
  delay(500);
  speicher.begin("r44", false);
  pinMode(VM_PIN, INPUT); pinMode(LINKS, INPUT); pinMode(RECHTS, INPUT);
  if (analogReadMilliVolts(VM_PIN) * 2 < 2500) {   // nur USB: Ergebnis zeigen
    int zeit = speicher.getInt("zeit", -1), verloren = speicher.getInt("verloren", -1), fehler = speicher.getInt("fehler", -1);
    if (zeit < 0) Serial.println("Noch nicht gefahren.");
    else {
      Serial.printf("Gefahren: %d s, Linie verloren: %s, mittlerer Fehler: %d Promille\n", zeit / 1000, verloren ? "ja" : "nein", fehler);
      Serial.printf("KW-CHECK:r-4-4:%012llX:%d:%d:%d\n", (unsigned long long)ESP.getEfuseMac(), zeit / 1000, verloren, fehler);
    }
    speicher.end();
    return;
  }

  pinMode(SLEEP, OUTPUT); digitalWrite(SLEEP, HIGH); delay(2);
  ledcAttach(AIN1, 20000, 8); ledcAttach(AIN2, 20000, 8);
  ledcAttach(BIN1, 20000, 8); ledcAttach(BIN2, 20000, 8);
  fahre(0, 0);
  delay(3000);
  kalibrieren();
  if (abs(l.schwarz - l.weiss) < 300 || abs(r.schwarz - r.weiss) < 300) {   // Kalibrierung misslungen
    speicher.putInt("zeit", 0); speicher.putInt("verloren", 1); speicher.putInt("fehler", 1000);
    speicher.end();
    return;
  }
  delay(2000);

  float integral = 0, letzterFehler = 0, letzteRichtung = 1, summeFehler = 0;
  long schritte = 0;
  unsigned long start = millis(), verlorenSeit = 0, letzterTakt = micros();
  bool verloren = false;
  while (millis() - start < FAHRZEIT_MS) {
    // fester Takt von 10 ms, damit I und D mit gleichem Zeitschritt rechnen
    while (micros() - letzterTakt < 10000) {}
    letzterTakt += 10000;
    const float dt = 0.01;

    float nl = normiert(l), nr = normiert(r);
    float e;
    if (nl < 0.1 && nr < 0.1) {             // Linie zwischen den Sensoren ODER verloren
      e = (fabs(letzterFehler) > 0.3) ? letzteRichtung : 0;   // vorher am Rand: in diese Richtung weiter suchen
      if (e != 0 && verlorenSeit == 0) verlorenSeit = millis();
    } else {
      e = nl - nr;                            // > 0: Linie links, also nach links lenken
      verlorenSeit = 0;
      if (fabs(e) > 0.1) letzteRichtung = e > 0 ? 1 : -1;
    }
    if (verlorenSeit && millis() - verlorenSeit > 1000) { verloren = true; break; }

    integral = constrain(integral + e * dt, -1.0f, 1.0f);   // begrenzen, damit sich nichts aufschaukelt
    float ableitung = (e - letzterFehler) / dt;
    float u = KP * e + KI * integral + KD * ableitung;
    u = constrain(u, -1.0f, 1.0f);
    letzterFehler = e;
    summeFehler += fabs(e); schritte++;

    fahre(FAHRT * (1 - u), FAHRT * (1 + u));    // u > 0: rechtes Rad schneller, Linkskurve
  }
  bremsen();
  speicher.putInt("zeit", millis() - start);
  speicher.putInt("verloren", verloren ? 1 : 0);
  speicher.putInt("fehler", schritte ? (int)(1000 * summeFehler / schritte) : 1000);
  speicher.end();
}

void loop() {}
