// Kampagne 2, Mission 5.1: Der Encoder
// Board: ESP32 Dev Module. Läuft am USB (Programmier-Regel aus Mission 2.2), die Motoren bleiben aus.
//
// Zwei Gabellichtschranken-Module (LM393, Schlitz 5 mm), Encoderscheiben auf den Motorachsen:
//   VCC -> 3V3   GND -> GND   DO links -> GPIO 16   DO rechts -> GPIO 17
// Drehe jedes Rad von Hand genau eine Umdrehung (Markierung mit Klebeband) und lies die Zähler ab.

const int ENC_L = 16, ENC_R = 17;
const uint32_t SPERRZEIT_US = 1500;   // Impulse, die schneller kommen, sind Störungen

volatile uint32_t zaehlerL = 0, zaehlerR = 0;
volatile uint32_t letzteL = 0, letzteR = 0;

// Interrupt-Routinen: so kurz wie möglich, nur zählen.
void IRAM_ATTR kanteL() { uint32_t t = micros(); if (t - letzteL > SPERRZEIT_US) { zaehlerL++; letzteL = t; } }
void IRAM_ATTR kanteR() { uint32_t t = micros(); if (t - letzteR > SPERRZEIT_US) { zaehlerR++; letzteR = t; } }

void setup() {
  Serial.begin(115200);
  delay(500);
  pinMode(ENC_L, INPUT); pinMode(ENC_R, INPUT);
  attachInterrupt(digitalPinToInterrupt(ENC_L), kanteL, RISING);
  attachInterrupt(digitalPinToInterrupt(ENC_R), kanteR, RISING);
  Serial.println();
  Serial.println("Mission 5.1: Drehe jedes Rad genau eine Umdrehung.");
}

bool gemeldet = false;

void loop() {
  uint32_t l = zaehlerL, r = zaehlerR;   // 32-Bit-Werte liest der ESP32 in einem Stück
  Serial.printf("links: %4lu   rechts: %4lu\n", (unsigned long)l, (unsigned long)r);
  if (!gemeldet && l >= 10 && r >= 10) {
    delay(3000);                          // noch 3 s Zeit, die Umdrehung zu beenden
    l = zaehlerL; r = zaehlerR;
    Serial.printf("KW-CHECK:r-5-1:%012llX:%lu:%lu\n", (unsigned long long)ESP.getEfuseMac(), (unsigned long)l, (unsigned long)r);
    gemeldet = true;
  }
  delay(300);
}
