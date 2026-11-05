// Kampagne 2, Mission 3.2: Die Linie erkennen
// Board: ESP32 Dev Module. Läuft am USB, die Motoren bleiben aus.
//
// Linker Sensor:  AO -> GPIO 36 (VP)     Rechter Sensor: AO -> GPIO 39 (VN)
// Beide Module:   VCC -> 3V3, GND -> GND
//
// Ablauf nach dem Start:
//   1. 3 Sekunden: Beide Sensoren über WEISS halten. Das ist der Bezug.
//   2. 8 Sekunden: Den Roboter langsam von Hand mehrmals quer über die Linie schieben.
//   3. Danach zeigt er laufend, wo die Linie liegt.

const int LINKS = 36, RECHTS = 39;

struct Sensor {
  int pin;
  int weiss, minimum, maximum;
  int schwelle, band;      // Schwelle in der Mitte, Hystereseband
  bool linieIstHoch;       // misst der Sensor über Schwarz mehr oder weniger als über Weiß?
  bool aufLinie;
};

Sensor l = {LINKS}, r = {RECHTS};
bool kalibriert = false;

int lesen(int pin) {
  long summe = 0;
  for (int i = 0; i < 8; i++) summe += analogReadMilliVolts(pin);
  return summe / 8;
}

void auswerten(Sensor &s) {
  int v = lesen(s.pin);
  // Abstand zum Weiß-Wert in Richtung Schwarz, damit es für beide Polaritäten gleich funktioniert
  int richtung = s.linieIstHoch ? v : -v;
  int schwelle = s.linieIstHoch ? s.schwelle : -s.schwelle;
  // Hysterese: zum Einschalten über die obere, zum Ausschalten unter die untere Grenze
  if (!s.aufLinie && richtung > schwelle + s.band / 2) s.aufLinie = true;
  if (s.aufLinie && richtung < schwelle - s.band / 2) s.aufLinie = false;
}

void setup() {
  Serial.begin(115200);
  delay(500);
  pinMode(LINKS, INPUT); pinMode(RECHTS, INPUT);
  Serial.println();
  Serial.println("Mission 3.2: Beide Sensoren jetzt über WEISS halten (3 s) ...");
  delay(1000);
  l.weiss = lesen(LINKS); r.weiss = lesen(RECHTS);
  delay(2000);
  l.weiss = (l.weiss + lesen(LINKS)) / 2; r.weiss = (r.weiss + lesen(RECHTS)) / 2;
  Serial.printf("Weiß: links %d mV, rechts %d mV\n", l.weiss, r.weiss);

  Serial.println("Jetzt 8 s lang langsam quer über die Linie schieben ...");
  l.minimum = l.maximum = l.weiss; r.minimum = r.maximum = r.weiss;
  unsigned long start = millis();
  while (millis() - start < 8000) {
    int a = lesen(LINKS), b = lesen(RECHTS);
    l.minimum = min(l.minimum, a); l.maximum = max(l.maximum, a);
    r.minimum = min(r.minimum, b); r.maximum = max(r.maximum, b);
  }

  for (Sensor *s : {&l, &r}) {
    // Schwarz ist das Ende, das weiter vom Weiß-Wert entfernt liegt.
    s->linieIstHoch = (s->maximum - s->weiss) > (s->weiss - s->minimum);
    int schwarz = s->linieIstHoch ? s->maximum : s->minimum;
    s->schwelle = (s->weiss + schwarz) / 2;
    s->band = abs(schwarz - s->weiss) / 5;   // 20 % der Spanne als Hysterese. Zum Ausprobieren: s->band = 0;
  }
  Serial.printf("Links:  weiß %d, schwarz %d, Schwelle %d mV\n", l.weiss, l.linieIstHoch ? l.maximum : l.minimum, l.schwelle);
  Serial.printf("Rechts: weiß %d, schwarz %d, Schwelle %d mV\n", r.weiss, r.linieIstHoch ? r.maximum : r.minimum, r.schwelle);

  int spanneL = l.maximum - l.minimum, spanneR = r.maximum - r.minimum;
  if (spanneL < 300 || spanneR < 300) {
    Serial.println("Zu wenig Unterschied zwischen Weiß und Schwarz (unter 300 mV).");
    Serial.println("Sensoren näher an den Boden (etwa 2 bis 10 mm), anderes Klebeband probieren, dann Reset.");
    return;
  }
  kalibriert = true;
  Serial.printf("KW-CHECK:r-3-2:%012llX:%d:%d\n", (unsigned long long)ESP.getEfuseMac(), spanneL, spanneR);
}

void loop() {
  if (!kalibriert) { delay(1000); return; }   // Kalibrierung fehlgeschlagen
  auswerten(l); auswerten(r);
  const char* lage = l.aufLinie && r.aufLinie ? "unter beiden (Kreuzung oder quer)"
                   : l.aufLinie ? "links" : r.aufLinie ? "rechts" : "nicht zu sehen (oder genau dazwischen)";
  Serial.printf("Linie: %s\n", lage);
  delay(150);
}
