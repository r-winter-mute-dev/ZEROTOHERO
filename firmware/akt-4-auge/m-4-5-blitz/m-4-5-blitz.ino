// 0 to Hero – Mission 4.5: Der Blitz
// Die helle Blitz-LED hängt über einen Transistor an GPIO 4.
// Erst ganz an und aus, dann gedimmt mit PWM.
// Achtung: Die LED ist sehr hell. Nicht direkt hineinschauen.
// Board: "AI Thinker ESP32-CAM".

const int BLITZ = 4;

void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println();

  // 1. Einfach an und aus, wie eine LED in Akt 2
  pinMode(BLITZ, OUTPUT);
  Serial.println("Blitz kurz an ...");
  digitalWrite(BLITZ, HIGH);
  delay(150);
  digitalWrite(BLITZ, LOW);
  delay(1000);

  // 2. PWM: 5000-mal pro Sekunde an und aus, Helligkeit in 256 Stufen (8 Bit)
  ledcAttach(BLITZ, 5000, 8);
}

void loop() {
  const int stufen[] = {0, 2, 5, 10, 20, 10, 5, 2};   // höchstens 20 von 255, also unter 10 %
  for (int s : stufen) {
    ledcWrite(BLITZ, s);
    Serial.printf("Tastgrad %d von 255 (%.0f %%)\n", s, s * 100.0 / 255);
    delay(600);
  }
  ledcWrite(BLITZ, 0);
  Serial.printf("KW-CHECK:m-4-5:%012llX:pwm-ok\n", (unsigned long long)ESP.getEfuseMac());
  delay(3000);
}
