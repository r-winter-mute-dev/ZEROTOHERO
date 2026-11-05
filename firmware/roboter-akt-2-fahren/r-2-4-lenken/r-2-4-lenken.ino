// Kampagne 2, Mission 2.4: Lenken
// Board: ESP32 Dev Module. Versorgung wie in Mission 2.2.
// Diesmal fährt der Roboter auf dem Boden: freie Fläche von etwa 1 × 1 m, nichts Zerbrechliches.
// Motoren: AIN1 25, AIN2 26 (links)  BIN1 32, BIN2 33 (rechts)  nSLEEP 27
//
// Nach dem Einschalten wartet der Roboter 5 Sekunden, dann fährt er vier Figuren:
// geradeaus, Drehung auf der Stelle, Drehung um ein Rad, eine weite Linkskurve.
// Danach steht er. Auslesen wie in Mission 2.2: Akku AUS, 3V3-Kabel vom Wandler abziehen, USB anstecken.
// Teiler 10 kΩ / 10 kΩ von VM an GPIO 34 wie in Mission 2.2.

#include <Preferences.h>
Preferences speicher;
const int VM_PIN = 34;

const int AIN1 = 25, AIN2 = 26, BIN1 = 32, BIN2 = 33, SLEEP = 27;
const int ANLAUF = 85;   // aus Mission 1.4

void pwm(int links, int rechts) {
  links = constrain(links, -255, 255); rechts = constrain(rechts, -255, 255);
  ledcWrite(AIN1, links > 0 ? links : 0);  ledcWrite(AIN2, links < 0 ? -links : 0);
  ledcWrite(BIN1, rechts > 0 ? rechts : 0); ledcWrite(BIN2, rechts < 0 ? -rechts : 0);
}

int prozentZuPwm(int prozent) {
  prozent = constrain(prozent, -100, 100);
  if (prozent == 0) return 0;
  int betrag = map(abs(prozent), 1, 100, ANLAUF, 255);
  return prozent > 0 ? betrag : -betrag;
}

// Fahren in Prozent: links, rechts, Dauer. Danach kurz bremsen und Pause.
void fahre(const char* name, int links, int rechts, int ms) {
  Serial.println(name);
  pwm(prozentZuPwm(links), prozentZuPwm(rechts));
  delay(ms);
  // bremsen: alle Eingänge HIGH (siehe Mission 1.3)
  ledcWrite(AIN1, 255); ledcWrite(AIN2, 255); ledcWrite(BIN1, 255); ledcWrite(BIN2, 255);
  delay(300);
  pwm(0, 0);
  delay(1200);
}

void setup() {
  Serial.begin(115200);
  delay(500);
  pinMode(SLEEP, OUTPUT); digitalWrite(SLEEP, HIGH); delay(2);
  ledcAttach(AIN1, 20000, 8); ledcAttach(AIN2, 20000, 8);
  ledcAttach(BIN1, 20000, 8); ledcAttach(BIN2, 20000, 8);
  pwm(0, 0);
  Serial.println();
  speicher.begin("r24", false);
  pinMode(VM_PIN, INPUT);
  if (analogReadMilliVolts(VM_PIN) * 2 < 2500) {   // nur USB
    if (speicher.getInt("gefahren", 0)) Serial.printf("KW-CHECK:r-2-4:%012llX\n", (unsigned long long)ESP.getEfuseMac());
    else Serial.println("Noch nicht gefahren. Erst mit Akku auf dem Boden laufen lassen.");
    speicher.end();
    return;
  }
  Serial.println("Mission 2.4: Lenken. Start in 5 Sekunden.");
  delay(5000);

  fahre("Geradeaus", 50, 50, 1500);
  fahre("Auf der Stelle links drehen", -40, 40, 800);
  fahre("Um das linke Rad drehen", 0, 50, 1200);
  fahre("Weite Linkskurve", 30, 60, 2000);

  speicher.putInt("gefahren", 1);
  speicher.end();
  Serial.printf("KW-CHECK:r-2-4:%012llX\n", (unsigned long long)ESP.getEfuseMac());
}

void loop() {}
