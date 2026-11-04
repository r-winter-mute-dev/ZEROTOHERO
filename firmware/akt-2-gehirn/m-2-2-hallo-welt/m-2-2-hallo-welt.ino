// 0 to Hero – Mission 2.2: Hallo, Welt
// Schickt einen eigenen Text an den Computer.

String name = "DEIN NAME";  // <- hier deinen Namen eintragen

void setup() {
  Serial.begin(115200);
}

void loop() {
  Serial.println("Hallo! Ich bin die Klingel von " + name + ".");
  Serial.printf("KW-CHECK:m-2-2:%012llX:%s\n", (unsigned long long)ESP.getEfuseMac(), name.c_str());
  delay(2000);
}
