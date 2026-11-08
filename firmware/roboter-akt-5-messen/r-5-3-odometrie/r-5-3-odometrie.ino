// Kampagne 2, Mission 5.3: Wo bin ich?
// Board: ESP32 Dev Module. Akku-Betrieb, fährt auf dem Boden.
// Motoren und Encoder wie in Mission 5.2.
//
// Der Roboter fährt laut Encoder genau STRECKE_CM geradeaus, dreht sich laut Encoder um 90° nach links
// und hält an. Du misst mit dem Maßband nach, wie weit er wirklich gefahren ist.

#include <Preferences.h>
Preferences speicher;

const int AIN1 = 25, AIN2 = 26, BIN1 = 32, BIN2 = 33, SLEEP = 27, VM_PIN = 34, ENC_L = 16, ENC_R = 17;
const int ANLAUF = 85;
const int IMPULSE_PRO_UMDREHUNG = 20;
const float RAD_DURCHMESSER_MM = 66;
const float SPURBREITE_MM = 140;       // aus Mission 2.4
const float STRECKE_CM = 100;
const int FAHRT = 35;                  // Prozent
const uint32_t SPERRZEIT_US = 1500;

volatile uint32_t zaehlerL = 0, zaehlerR = 0, letzteL = 0, letzteR = 0;
void IRAM_ATTR kanteL() { uint32_t t = micros(); if (t - letzteL > SPERRZEIT_US) { zaehlerL++; letzteL = t; } }
void IRAM_ATTR kanteR() { uint32_t t = micros(); if (t - letzteR > SPERRZEIT_US) { zaehlerR++; letzteR = t; } }

int prozentZuPwm(int p) { if (p == 0) return 0; int b = map(abs(p), 1, 100, ANLAUF, 255); return p > 0 ? b : -b; }
void pwm(int l, int r) {
  ledcWrite(AIN1, l > 0 ? l : 0); ledcWrite(AIN2, l < 0 ? -l : 0);
  ledcWrite(BIN1, r > 0 ? r : 0); ledcWrite(BIN2, r < 0 ? -r : 0);
}
void bremsen() { ledcWrite(AIN1, 255); ledcWrite(AIN2, 255); ledcWrite(BIN1, 255); ledcWrite(BIN2, 255); delay(400); pwm(0, 0); }

const float MM_PRO_IMPULS = PI * RAD_DURCHMESSER_MM / IMPULSE_PRO_UMDREHUNG;

void setup() {
  Serial.begin(115200);
  delay(500);
  speicher.begin("r53", false);
  pinMode(VM_PIN, INPUT);
  if (analogReadMilliVolts(VM_PIN) * 2 < 2500) {
    int s = speicher.getInt("strecke", -1), w = speicher.getInt("winkel", -1);
    if (s < 0) Serial.println("Noch nicht gefahren.");
    else {
      Serial.printf("Laut Encoder: %d mm gefahren, %d Grad gedreht\n", s, w);
      Serial.printf("KW-CHECK:r-5-3:%012llX:%d:%d\n", (unsigned long long)ESP.getEfuseMac(), s, w);
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

  // 1. Geradeaus, bis die Mitte STRECKE_CM gefahren ist
  zaehlerL = 0; zaehlerR = 0;
  int v = prozentZuPwm(FAHRT);
  pwm(v, v);
  while ((zaehlerL + zaehlerR) / 2.0 * MM_PRO_IMPULS < STRECKE_CM * 10) delay(2);
  bremsen();
  float strecke = (zaehlerL + zaehlerR) / 2.0 * MM_PRO_IMPULS;   // inklusive Nachrollen
  delay(1000);

  // 2. Auf der Stelle 90° nach links: jedes Rad fährt ein Viertel des Kreises mit Durchmesser = Spurbreite
  // Ein einkanaliger Encoder kennt keine Richtung. Das Programm weiß aber, wie es die Motoren ansteuert.
  zaehlerL = 0; zaehlerR = 0;
  const float bogen = PI * SPURBREITE_MM / 4;     // Weg jedes Rads für 90°
  pwm(-prozentZuPwm(FAHRT), prozentZuPwm(FAHRT));
  while ((zaehlerL + zaehlerR) / 2.0 * MM_PRO_IMPULS < bogen) delay(2);
  bremsen();
  float winkel = (zaehlerL + zaehlerR) * MM_PRO_IMPULS / SPURBREITE_MM * 180.0 / PI;   // (sR + sL) / b, weil sL rückwärts lief

  speicher.putInt("strecke", (int)strecke);
  speicher.putInt("winkel", (int)winkel);
  speicher.end();
}

void loop() {}
