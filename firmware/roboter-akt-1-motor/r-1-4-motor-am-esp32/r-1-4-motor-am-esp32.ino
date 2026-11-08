// Kampagne 2, Mission 1.5: Ein Motor am DRV8833
// Board: ESP32 Dev Module. Räder aufbocken, bevor du hochlädst.
//
// Verdrahtung (Steckbrett):
//   DRV8833 VM (oft VCC/VIN)  <- Plus der 2 x AA (Schalter am Halter)
//   DRV8833 GND               <- Minus der 2 x AA UND GND des ESP32 (gemeinsame Masse)
//   DRV8833 AIN1 (oft IN1)    <- GPIO 25
//   DRV8833 AIN2 (oft IN2)    <- GPIO 26
//   DRV8833 nSLEEP (EEP/SLP)  <- GPIO 27  (wenn dein Modul den Pin herausführt)
//   DRV8833 AOUT1/AOUT2       -> die beiden Motorkabel
// Der ESP32 bekommt seinen Strom über USB. VM nie an 3V3 oder 5V des ESP32.
//
// Wahrheitstabelle laut Datenblatt (TI DRV8833):
//   IN1 IN2   0 0 Freilauf   1 0 vorwärts   0 1 rückwärts   1 1 bremsen

const int AIN1 = 25;
const int AIN2 = 26;
const int SLEEP = 27;

const uint32_t PWM_FREQ = 20000;  // 20 kHz: über dem Hörbereich, kein Pfeifen
const uint8_t PWM_BITS = 8;       // Werte 0 bis 255

// Geschwindigkeit von -255 (voll rückwärts) bis +255 (voll vorwärts). 0 = Freilauf.
// Die PWM liegt am einen Eingang, der andere bleibt auf 0 (im Datenblatt "fast decay").
void motor(int v) {
  v = constrain(v, -255, 255);
  if (v > 0) { ledcWrite(AIN1, v); ledcWrite(AIN2, 0); }
  else if (v < 0) { ledcWrite(AIN1, 0); ledcWrite(AIN2, -v); }
  else { ledcWrite(AIN1, 0); ledcWrite(AIN2, 0); }
}

// Beide Eingänge auf 1: Der Motor wird kurzgeschlossen und bremst.
// 255 von 255 ist bei 8 Bit nicht ganz 100 %, für das Bremsen reicht das.
void bremsen() {
  ledcWrite(AIN1, 255);
  ledcWrite(AIN2, 255);
}

void schritt(const char* text, int v, unsigned long ms) {
  Serial.println(text);
  motor(v);
  delay(ms);
}

int anlaufWert = -1;

void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println();
  Serial.println("Mission 1.5: Motor am DRV8833. Räder in der Luft?");

  pinMode(SLEEP, OUTPUT);
  digitalWrite(SLEEP, HIGH);   // Treiber aufwecken. Ohne HIGH hält ihn ein interner Pulldown im Schlaf.
  delay(2);                    // Laut Datenblatt bis zu 1 ms Aufwachzeit

  ledcAttach(AIN1, PWM_FREQ, PWM_BITS);
  ledcAttach(AIN2, PWM_FREQ, PWM_BITS);
  motor(0);
  delay(2000);                 // Zeit, die Hand wegzunehmen
}

void loop() {
  schritt("Vorwärts, volle Kraft (IN1 = PWM 255, IN2 = 0)", 255, 2000);
  schritt("Freilauf (IN1 = 0, IN2 = 0): rollt aus", 0, 1500);
  schritt("Rückwärts, volle Kraft (IN1 = 0, IN2 = PWM 255)", -255, 2000);
  Serial.println("Bremsen (IN1 = 1, IN2 = 1): hält schnell an");
  bremsen();
  delay(1500);
  motor(0);

  // Langsam hochfahren: Ab welchem PWM-Wert dreht der Motor?
  Serial.println("Rampe: PWM von 0 bis 255. Merk dir, ab welchem Wert sich das Rad dreht.");
  for (int v = 0; v <= 255; v += 5) {
    motor(v);
    Serial.printf("PWM %3d  (%.1f %%)\n", v, v * 100.0 / 255);
    delay(250);
  }
  schritt("Rampe fertig. Freilauf.", 0, 2000);

  Serial.printf("KW-CHECK:r-1-4:%012llX\n", (unsigned long long)ESP.getEfuseMac());
  Serial.println("Nächste Runde in 5 Sekunden.");
  delay(5000);
}
