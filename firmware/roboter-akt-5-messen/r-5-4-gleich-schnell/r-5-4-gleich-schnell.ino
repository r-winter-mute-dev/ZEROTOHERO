// Kampagne 2, Mission 5.4: Gleich schnell
// Board: ESP32 Dev Module. Akku-Betrieb, fährt auf dem Boden.
// Motoren und Encoder wie in Mission 5.2.
//
// Zwei Regelkreise: Jedes Rad wird auf dieselbe Sollgeschwindigkeit geregelt (PI-Regler).
// Mit REGELN = false fährt er wie in Mission 2.4 nur mit festem PWM. Vergleiche beide.

#include <Preferences.h>
Preferences speicher;

const int AIN1 = 25, AIN2 = 26, BIN1 = 32, BIN2 = 33, SLEEP = 27, VM_PIN = 34, ENC_L = 16, ENC_R = 17;
const int ANLAUF = 85;
const int IMPULSE_PRO_UMDREHUNG = 20;
const float RAD_DURCHMESSER_MM = 66;
const bool REGELN = true;
const float SOLL_CM_S = 20;            // Sollgeschwindigkeit beider Räder
const float KP = 2.0, KI = 4.0;        // Prozent pro (cm/s) und Prozent pro (cm/s · s)
const float STRECKE_CM = 200;
const uint32_t SPERRZEIT_US = 1500;

volatile uint32_t zaehlerL = 0, zaehlerR = 0, letzteL = 0, letzteR = 0;
void IRAM_ATTR kanteL() { uint32_t t = micros(); if (t - letzteL > SPERRZEIT_US) { zaehlerL++; letzteL = t; } }
void IRAM_ATTR kanteR() { uint32_t t = micros(); if (t - letzteR > SPERRZEIT_US) { zaehlerR++; letzteR = t; } }

const float MM_PRO_IMPULS = PI * RAD_DURCHMESSER_MM / IMPULSE_PRO_UMDREHUNG;

int prozentZuPwm(float p) { p = constrain(p, -100.0f, 100.0f); if (fabs(p) < 0.5) return 0; int b = map((long)fabs(p), 1, 100, ANLAUF, 255); return p > 0 ? b : -b; }
void fahre(float l, float r) {
  int a = prozentZuPwm(l), b = prozentZuPwm(r);
  ledcWrite(AIN1, a > 0 ? a : 0); ledcWrite(AIN2, a < 0 ? -a : 0);
  ledcWrite(BIN1, b > 0 ? b : 0); ledcWrite(BIN2, b < 0 ? -b : 0);
}
void bremsen() { ledcWrite(AIN1, 255); ledcWrite(AIN2, 255); ledcWrite(BIN1, 255); ledcWrite(BIN2, 255); delay(400); fahre(0, 0); }

struct Rad { float integral; uint32_t vorher; float v; float stell; };

void setup() {
  Serial.begin(115200);
  delay(500);
  speicher.begin("r54", false);
  pinMode(VM_PIN, INPUT);
  if (analogReadMilliVolts(VM_PIN) * 2 < 2500) {
    int vl = speicher.getInt("vl", -1), vr = speicher.getInt("vr", -1), reg = speicher.getInt("regeln", -1);
    if (vl < 0) Serial.println("Noch nicht gefahren.");
    else {
      Serial.printf("%s: links %.1f cm/s, rechts %.1f cm/s (Mittel der Fahrt)\n", reg ? "Geregelt" : "Ungeregelt", vl / 10.0, vr / 10.0);
      Serial.printf("KW-CHECK:r-5-4:%012llX:%d:%d:%d\n", (unsigned long long)ESP.getEfuseMac(), reg, vl, vr);
    }
    speicher.end();
    return;
  }

  pinMode(ENC_L, INPUT); pinMode(ENC_R, INPUT);
  attachInterrupt(digitalPinToInterrupt(ENC_L), kanteL, RISING);
  attachInterrupt(digitalPinToInterrupt(ENC_R), kanteR, RISING);
  pinMode(SLEEP, OUTPUT); digitalWrite(SLEEP, HIGH); delay(2);
  ledcAttach(AIN1, 20000, 8); ledcAttach(AIN2, 20000, 8);
  ledcAttach(BIN1, 20000, 8); ledcAttach(BIN2, 20000, 8);
  delay(5000);

  // Startwert: ungefähr passendes Prozent, damit der Regler nicht bei 0 beginnt
  Rad l = {0, 0, 0, 30}, r = {0, 0, 0, 30};
  const float DT = 0.1;                          // 100 ms: bei 20 Impulsen pro Umdrehung braucht es Zeit, um genug zu zählen
  zaehlerL = 0; zaehlerR = 0;
  unsigned long start = millis(), takt = start;
  while ((zaehlerL + zaehlerR) / 2.0 * MM_PRO_IMPULS < STRECKE_CM * 10) {
    fahre(l.stell, r.stell);
    while (millis() - takt < 100) {}
    takt += 100;
    uint32_t nl = zaehlerL, nr = zaehlerR;
    l.v = (nl - l.vorher) * MM_PRO_IMPULS / 10.0 / DT; l.vorher = nl;   // cm/s
    r.v = (nr - r.vorher) * MM_PRO_IMPULS / 10.0 / DT; r.vorher = nr;
    if (REGELN) {
      for (Rad *rad : {&l, &r}) {
        float e = SOLL_CM_S - rad->v;
        rad->integral = constrain(rad->integral + e * DT, -20.0f, 20.0f);
        rad->stell = constrain(30 + KP * e + KI * rad->integral, 0.0f, 100.0f);
      }
    }
  }
  float dauer = (millis() - start) / 1000.0;     // Sekunden, vor dem Bremsen gemessen
  bremsen();
  // Mittlere Geschwindigkeit jedes Rads über die ganze Fahrt, in Zehntel cm/s
  speicher.putInt("vl", (int)(l.vorher * MM_PRO_IMPULS / dauer));
  speicher.putInt("vr", (int)(r.vorher * MM_PRO_IMPULS / dauer));
  speicher.putInt("regeln", REGELN ? 1 : 0);
  speicher.end();
}

void loop() {}
