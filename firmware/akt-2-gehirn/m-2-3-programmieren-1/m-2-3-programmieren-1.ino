// 0 to Hero – Mission 2.3: Programmieren I (Variablen und Bedingungen)
// Die LED blinkt erst langsam, ab dem fünften Mal schnell.

const int LED_PIN = 2;
int pause = 1000;   // Wartezeit in Millisekunden
int zaehler = 0;    // wie oft die LED schon geblinkt hat

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
}

void loop() {
  digitalWrite(LED_PIN, HIGH);
  delay(pause);
  digitalWrite(LED_PIN, LOW);
  delay(pause);

  zaehler = zaehler + 1;
  Serial.print("Blink Nummer ");
  Serial.println(zaehler);

  if (zaehler >= 5) {
    pause = 200;
  }

  if (zaehler == 10) {
    Serial.printf("KW-CHECK:m-2-3:%012llX\n", (unsigned long long)ESP.getEfuseMac());
  }
}
