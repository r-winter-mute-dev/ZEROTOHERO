// Kampagne 2, Mission 2.3: Sanft anfahren
// Board: ESP32 Dev Module. Versorgung wie in Mission 2.2. Räder aufbocken.
// Motoren: AIN1 25, AIN2 26 (links)  BIN1 32, BIN2 33 (rechts)  nSLEEP 27
// Teiler 10 kΩ / 10 kΩ von VM an GPIO 34.
//
// Vergleicht zwei Arten anzufahren: schlagartig und mit einer Rampe.
// Dazu kommt die Umrechnung von 0 bis 100 % auf den Bereich, in dem der Motor wirklich dreht.
// Auslesen wie in Mission 2.2: Akku AUS, 3V3-Kabel vom Wandler abziehen, USB anstecken.

#include <Preferences.h>
Preferences speicher;

const int AIN1 = 25, AIN2 = 26, BIN1 = 32, BIN2 = 33, SLEEP = 27;
const int VM_PIN = 34;

// Dein Anlaufwert aus Mission 1.5. Darunter brummt der Motor nur.
const int ANLAUF = 85;

int aktuellL = 0, aktuellR = 0;   // zuletzt gesetzte PWM-Werte

int vmMillivolt() { return (int)(analogReadMilliVolts(VM_PIN) * 2.0); }

void pwm(int links, int rechts) {
  aktuellL = constrain(links, -255, 255); aktuellR = constrain(rechts, -255, 255);
  ledcWrite(AIN1, aktuellL > 0 ? aktuellL : 0);  ledcWrite(AIN2, aktuellL < 0 ? -aktuellL : 0);
  ledcWrite(BIN1, aktuellR > 0 ? aktuellR : 0); ledcWrite(BIN2, aktuellR < 0 ? -aktuellR : 0);
}

// 0 bis 100 % auf PWM umrechnen. 0 % bleibt 0, alles andere startet beim Anlaufwert.
int prozentZuPwm(int prozent) {
  prozent = constrain(prozent, -100, 100);
  if (prozent == 0) return 0;
  int betrag = map(abs(prozent), 1, 100, ANLAUF, 255);
  return prozent > 0 ? betrag : -betrag;
}

// Von den aktuellen Werten in "dauerMs" gleichmäßig zu den Zielwerten fahren.
// Gibt den tiefsten gemessenen Wert an VM zurück.
int rampe(int zielL, int zielR, int dauerMs) {
  int startL = aktuellL, startR = aktuellR;
  int minimum = vmMillivolt();
  const int SCHRITTE = 50;
  for (int i = 1; i <= SCHRITTE; i++) {
    pwm(startL + (zielL - startL) * i / SCHRITTE, startR + (zielR - startR) * i / SCHRITTE);
    unsigned long t = millis();
    while (millis() - t < (unsigned long)(dauerMs / SCHRITTE)) {
      int u = vmMillivolt();
      if (u < minimum) minimum = u;
    }
  }
  return minimum;
}

void setup() {
  Serial.begin(115200);
  delay(500);
  pinMode(VM_PIN, INPUT);
  pinMode(SLEEP, OUTPUT); digitalWrite(SLEEP, HIGH); delay(2);
  ledcAttach(AIN1, 20000, 8); ledcAttach(AIN2, 20000, 8);
  ledcAttach(BIN1, 20000, 8); ledcAttach(BIN2, 20000, 8);
  pwm(0, 0);
  Serial.println();
  Serial.println("Mission 2.3: Sanft anfahren");
  speicher.begin("r23", false);
  if (vmMillivolt() < 2500) {   // nur USB: gespeichertes Ergebnis zeigen
    int a = speicher.getInt("sprung", -1), b = speicher.getInt("rampe", -1);
    if (a < 0) Serial.println("Noch kein Ergebnis. Erst mit Akku laufen lassen.");
    else Serial.printf("KW-CHECK:r-2-3:%012llX:%d:%d\n", (unsigned long long)ESP.getEfuseMac(), a, b);
    speicher.end();
    return;
  }
  Serial.printf("1 %% = PWM %d, 50 %% = PWM %d, 100 %% = PWM %d\n", prozentZuPwm(1), prozentZuPwm(50), prozentZuPwm(100));
  delay(3000);

  int ziel = prozentZuPwm(80);
  int ruhe = vmMillivolt();
  int sprung = rampe(ziel, ziel, 1);       // praktisch schlagartig
  delay(1000);
  rampe(0, 0, 300);
  delay(1500);

  ruhe = vmMillivolt();
  int sanft = rampe(ziel, ziel, 600);      // 0,6 s Rampe
  delay(1000);
  rampe(0, 0, 600);                        // auch sanft bremsen

  int eSprung = ruhe - sprung, eRampe = ruhe - sanft;
  speicher.putInt("sprung", eSprung); speicher.putInt("rampe", eRampe); speicher.end();
  Serial.printf("Einbruch schlagartig: %d mV, mit Rampe: %d mV\n", eSprung, eRampe);
  Serial.printf("KW-CHECK:r-2-3:%012llX:%d:%d\n", (unsigned long long)ESP.getEfuseMac(), eSprung, eRampe);
}

void loop() {}
