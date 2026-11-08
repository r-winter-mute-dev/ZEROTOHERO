// Kampagne 2, Mission 5.2: Geschwindigkeit
// Board: ESP32 Dev Module. Akku-Betrieb wie ab Mission 2.2, Roboter AUFGEBOCKT.
// Motoren: AIN1 25, AIN2 26 (links)  BIN1 32, BIN2 33 (rechts)  nSLEEP 27   VM-Teiler: GPIO 34
// Encoder: links GPIO 16, rechts GPIO 17
//
// Fährt beide Motoren nacheinander mit 30, 50, 70 und 100 % und misst je 2 s lang die Impulse.
// Ergebnis über USB auslesen wie in Mission 2.2.

#include <Preferences.h>
Preferences speicher;

const int AIN1 = 25, AIN2 = 26, BIN1 = 32, BIN2 = 33, SLEEP = 27, VM_PIN = 34, ENC_L = 16, ENC_R = 17;
const int ANLAUF = 85;                 // aus Mission 1.5
const int IMPULSE_PRO_UMDREHUNG = 20;  // aus Mission 5.1
const float RAD_DURCHMESSER_MM = 66;   // nachmessen!
const uint32_t SPERRZEIT_US = 1500;

volatile uint32_t zaehlerL = 0, zaehlerR = 0, letzteL = 0, letzteR = 0;
void IRAM_ATTR kanteL() { uint32_t t = micros(); if (t - letzteL > SPERRZEIT_US) { zaehlerL++; letzteL = t; } }
void IRAM_ATTR kanteR() { uint32_t t = micros(); if (t - letzteR > SPERRZEIT_US) { zaehlerR++; letzteR = t; } }

int prozentZuPwm(int p) { if (p == 0) return 0; int b = map(abs(p), 1, 100, ANLAUF, 255); return p > 0 ? b : -b; }
void pwm(int l, int r) {
  ledcWrite(AIN1, l > 0 ? l : 0); ledcWrite(AIN2, l < 0 ? -l : 0);
  ledcWrite(BIN1, r > 0 ? r : 0); ledcWrite(BIN2, r < 0 ? -r : 0);
}

// Impulse pro Sekunde in cm/s umrechnen
float cmProS(float impulseProS) { return impulseProS / IMPULSE_PRO_UMDREHUNG * PI * RAD_DURCHMESSER_MM / 10.0; }

void setup() {
  Serial.begin(115200);
  delay(500);
  speicher.begin("r52", false);
  pinMode(VM_PIN, INPUT);
  if (analogReadMilliVolts(VM_PIN) * 2 < 2500) {
    if (speicher.getInt("l50", -1) < 0) Serial.println("Noch nicht gemessen.");
    else {
      const int stufen[4] = {30, 50, 70, 100};
      Serial.println("Stufe   links cm/s   rechts cm/s");
      for (int i = 0; i < 4; i++) {
        char k1[8], k2[8]; sprintf(k1, "l%d", stufen[i]); sprintf(k2, "r%d", stufen[i]);
        Serial.printf("%3d %%   %6.1f       %6.1f\n", stufen[i], speicher.getInt(k1, 0) / 10.0, speicher.getInt(k2, 0) / 10.0);
      }
      Serial.printf("KW-CHECK:r-5-2:%012llX:%d:%d\n", (unsigned long long)ESP.getEfuseMac(), speicher.getInt("l50", 0), speicher.getInt("r50", 0));
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
  delay(3000);

  const int stufen[4] = {30, 50, 70, 100};
  for (int i = 0; i < 4; i++) {
    int v = prozentZuPwm(stufen[i]);
    pwm(v, v);
    delay(800);                                   // erst anlaufen lassen
    uint32_t l0 = zaehlerL, r0 = zaehlerR;
    delay(2000);                                  // 2 s zählen
    float vl = cmProS((zaehlerL - l0) / 2.0), vr = cmProS((zaehlerR - r0) / 2.0);
    char k1[8], k2[8]; sprintf(k1, "l%d", stufen[i]); sprintf(k2, "r%d", stufen[i]);
    speicher.putInt(k1, (int)(vl * 10)); speicher.putInt(k2, (int)(vr * 10));
  }
  pwm(0, 0);
  speicher.end();
}

void loop() {}
