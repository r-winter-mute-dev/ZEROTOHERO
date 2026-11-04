// Code für die Lückentexte. "___" markiert eine Lücke.
// Liegt hier statt in den MDX-Dateien, weil MDX die Einrückung in mehrzeiligen Attributen entfernt.

export const PUZZLE_M23 = `const int LED_PIN = 2;
int pause = ___;
int zaehler = 0;

void loop() {
  digitalWrite(LED_PIN, HIGH);
  delay(pause);
  digitalWrite(LED_PIN, LOW);
  delay(pause);

  zaehler = zaehler ___ 1;

  if (zaehler ___ 5) {
    pause = ___;
  }
}`;

export const PUZZLE_M24 = `void ___(int anzahl, int dauer) {
  for (int i = 0; i ___ anzahl; i++) {
    digitalWrite(LED_PIN, HIGH);
    delay(dauer);
    digitalWrite(LED_PIN, LOW);
    delay(dauer);
  }
}

void loop() {
  blinke(___, 150);   // dreimal kurz
  delay(400);
  blinke(1, ___);     // einmal lang
  delay(1500);
}`;

export const PUZZLE_M27 = `if (millis() - letzteAenderung ___ ENTPRELLZEIT && jetzt != stabilerZustand) {
  stabilerZustand = jetzt;
  if (stabilerZustand == ___) {
    zaehler++;
  }
}`;

// Mission 4.4: Aufbau eines multipart/form-data-Pakets (Telegram sendPhoto, Bild gekürzt)
export const PUZZLE_M44 = `POST /bot<TOKEN>/sendPhoto HTTP/1.1
Host: api.telegram.org
Content-Type: multipart/form-data; boundary=0toHeroKlingelGrenze

___
Content-Disposition: form-data; name="chat_id"

123456789
--0toHeroKlingelGrenze
Content-Disposition: form-data; name="photo"; filename="klingel.jpg"
Content-Type: ___

ÿØÿà … (hier stehen die Bytes des Fotos) … ÿÙ
___`;
