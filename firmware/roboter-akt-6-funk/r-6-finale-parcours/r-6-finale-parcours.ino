// Kampagne 2, Finale: Der Parcours
// Board: ESP32 Dev Module. Akku-Betrieb. Fährt auf dem Boden!
// Alles zusammen: Fernsteuerung mit Notaus (Akt 6), Linienfolger mit PID (Akt 4),
// Ultraschall-Stopp vor Hindernissen (Akt 3), Prozent statt PWM (Akt 2).
//
// Pins: Motoren AIN1 25, AIN2 26, BIN1 32, BIN2 33, nSLEEP 27 · VM-Teiler 34
//       Liniensensoren 36 (links), 39 (rechts) · Ultraschall Trig 18, Echo 19
// Handy mit dem WLAN des Roboters verbinden, http://192.168.4.1 öffnen.

#include <WiFi.h>
#include <WebServer.h>

const char* WLAN_NAME = "Roboter-0toHero";
const char* WLAN_PASSWORT = "bitte-aendern";
const int AIN1 = 25, AIN2 = 26, BIN1 = 32, BIN2 = 33, SLEEP = 27, VM_PIN = 34;
const int LINKS = 36, RECHTS = 39, TRIG = 18, ECHO = 19;

// ---- deine Werte ----
const int ANLAUF = 85;
const int MAX_PROZENT = 60;        // Fernsteuerung
const float FAHRT = 25;            // Linienfolger
const float KP = 2.0, KI = 0.0, KD = 0.05;
const int HALT_CM = 20;            // aus Mission 3.4
const unsigned long NOTAUS_MS = 300;
// ----------------------

enum Modus { STEHT, FERN, LINIE };
Modus modus = STEHT;
WebServer server(80);

unsigned long letzterBefehl = 0, linieMs = 0, fernBefehle = 0, hindernisStopps = 0;
float fernV = 0, fernW = 0;
int abstandMm = -1;
bool hindernis = false, kalibriert = false;

struct Sensor { int pin, weiss, schwarz; } sl = {LINKS}, sr = {RECHTS};

int prozentZuPwm(float p) { p = constrain(p, -100.0f, 100.0f); if (fabs(p) < 0.5) return 0; int b = map((long)fabs(p), 1, 100, ANLAUF, 255); return p > 0 ? b : -b; }
void fahre(float l, float r) {
  int a = prozentZuPwm(l), b = prozentZuPwm(r);
  ledcWrite(AIN1, a > 0 ? a : 0); ledcWrite(AIN2, a < 0 ? -a : 0);
  ledcWrite(BIN1, b > 0 ? b : 0); ledcWrite(BIN2, b < 0 ? -b : 0);
}
void bremsen() { ledcWrite(AIN1, 255); ledcWrite(AIN2, 255); ledcWrite(BIN1, 255); ledcWrite(BIN2, 255); }

int lesen(int pin) { long s = 0; for (int i = 0; i < 4; i++) s += analogReadMilliVolts(pin); return s / 4; }
float normiert(const Sensor &s) { return constrain((float)(lesen(s.pin) - s.weiss) / (s.schwarz - s.weiss), 0.0f, 1.0f); }

// Ultraschall mit kurzem Zeitlimit: höchstens 6 ms warten (etwa 1 m), damit der Regeltakt nicht leidet.
int ultraschallMm() {
  digitalWrite(TRIG, LOW); delayMicroseconds(4);
  digitalWrite(TRIG, HIGH); delayMicroseconds(12); digitalWrite(TRIG, LOW);
  unsigned long us = pulseIn(ECHO, HIGH, 6000);
  return us == 0 ? -1 : (int)(343.4 * us / 2000.0);
}

void kalibrieren() {
  sl.weiss = lesen(LINKS); sr.weiss = lesen(RECHTS);
  int lmin = sl.weiss, lmax = sl.weiss, rmin = sr.weiss, rmax = sr.weiss;
  const float s[3][2] = {{-30, 30}, {30, -30}, {-30, 30}}; const int d[3] = {500, 1000, 500};
  for (int i = 0; i < 3; i++) {
    fahre(s[i][0], s[i][1]);
    unsigned long t = millis();
    while (millis() - t < (unsigned long)d[i]) { int a = lesen(LINKS), b = lesen(RECHTS); lmin = min(lmin, a); lmax = max(lmax, a); rmin = min(rmin, b); rmax = max(rmax, b); }
  }
  bremsen();
  sl.schwarz = (lmax - sl.weiss) > (sl.weiss - lmin) ? lmax : lmin;
  sr.schwarz = (rmax - sr.weiss) > (sr.weiss - rmin) ? rmax : rmin;
  kalibriert = abs(sl.schwarz - sl.weiss) >= 300 && abs(sr.schwarz - sr.weiss) >= 300;
}

const char SEITE[] = R"rawliteral(<!DOCTYPE html>
<html lang="de"><head><meta charset="utf-8"><meta name="viewport" content="width=device-width, initial-scale=1, user-scalable=no">
<title>Parcours</title><style>
body{font-family:sans-serif;background:#111;color:#eee;margin:1rem;text-align:center;user-select:none;-webkit-user-select:none}
#feld{width:240px;height:240px;margin:.8rem auto;border-radius:50%;background:#222;border:3px solid #cba6f7;position:relative;touch-action:none}
#knopf{width:60px;height:60px;border-radius:50%;background:#cba6f7;position:absolute;left:90px;top:90px}
button{font-size:1.05rem;padding:.7rem 1rem;margin:.2rem;border:0;border-radius:.7rem;background:#333;color:#eee}
#stop{background:#f38ba8;color:#111;font-size:1.3rem;padding:.9rem 2.2rem}
b{color:#cba6f7} small{color:#999;word-break:break-all}
</style></head><body>
<h2>Parcours</h2>
<p><button onclick="modus('fern')">Fernsteuern</button><button onclick="modus('kal')">Kalibrieren</button><button onclick="modus('linie')">Linie folgen</button></p>
<div id="feld"><div id="knopf"></div></div>
<button id="stop" onclick="modus('stopp')">STOPP</button>
<p>Modus: <b id="m">–</b> · Abstand: <b id="a">–</b> · Akku: <b id="akku">–</b> V</p>
<p>Linie: <b id="l">0</b> s · Hindernis-Stopps: <b id="h">0</b> · Fernbefehle: <b id="f">0</b></p>
<p><small id="nachweis"></small></p>
<script>
const feld=document.getElementById('feld'),knopf=document.getElementById('knopf');let v=0,w=0,aktiv=false;
function setzen(e){const r=feld.getBoundingClientRect();let x=(e.clientX-r.left-120)/120,y=(e.clientY-r.top-120)/120;const l=Math.hypot(x,y);if(l>1){x/=l;y/=l}v=-y;w=-x;knopf.style.left=(90+x*120)+'px';knopf.style.top=(90+y*120)+'px'}
function weg(){aktiv=false;v=0;w=0;knopf.style.left='90px';knopf.style.top='90px'}
feld.addEventListener('pointerdown',e=>{aktiv=true;setzen(e)});feld.addEventListener('pointermove',e=>{if(aktiv)setzen(e)});
window.addEventListener('pointerup',weg);window.addEventListener('pointercancel',weg);
function modus(m){weg();fetch('/modus?m='+m)}
setInterval(async()=>{try{const d=await (await fetch('/fahr?v='+v.toFixed(2)+'&w='+w.toFixed(2))).json();
for(const k of ['m','a','akku','l','h','f'])document.getElementById(k).textContent=d[k];document.getElementById('nachweis').textContent=d.nachweis}catch(e){document.getElementById('m').textContent='keine Verbindung'}},100);
</script></body></html>)rawliteral";

void antworten() {
  const char* name = modus == FERN ? "Fernsteuern" : modus == LINIE ? "Linie folgen" : "steht";
  char nachweis[80] = "";
  if (linieMs >= 20000 && hindernisStopps >= 1 && fernBefehle >= 100)
    snprintf(nachweis, sizeof nachweis, "KW-CHECK:r-6-boss:%012llX:%lu:%lu:%lu", (unsigned long long)ESP.getEfuseMac(), linieMs / 1000, hindernisStopps, fernBefehle);
  String a = abstandMm < 0 ? String("frei") : String(abstandMm / 10) + " cm";
  String json = String("{\"m\":\"") + name + "\",\"a\":\"" + a + "\",\"akku\":\"" + String(analogReadMilliVolts(VM_PIN) * 2 / 1000.0, 2) +
                "\",\"l\":" + String(linieMs / 1000) + ",\"h\":" + String(hindernisStopps) + ",\"f\":" + String(fernBefehle) + ",\"nachweis\":\"" + nachweis + "\"}";
  server.send(200, "application/json", json);
}

void fahrBefehl() {
  letzterBefehl = millis();                    // jeder Befehl ist ein Herzschlag, in jedem Modus
  fernV = constrain(server.arg("v").toFloat(), -1.0f, 1.0f);
  fernW = constrain(server.arg("w").toFloat(), -1.0f, 1.0f);
  if (modus == FERN && (fabs(fernV) > 0.05 || fabs(fernW) > 0.05)) fernBefehle++;
  antworten();
}

void modusBefehl() {
  String m = server.arg("m");
  bremsen();
  if (m == "fern") modus = FERN;
  else if (m == "linie" && kalibriert) modus = LINIE;
  else if (m == "kal") { modus = STEHT; server.send(200, "text/plain", "ok"); kalibrieren(); letzterBefehl = millis(); return; }
  else modus = STEHT;
  server.send(200, "text/plain", "ok");
}

void setup() {
  pinMode(SLEEP, OUTPUT); digitalWrite(SLEEP, HIGH); delay(2);
  ledcAttach(AIN1, 20000, 8); ledcAttach(AIN2, 20000, 8);
  ledcAttach(BIN1, 20000, 8); ledcAttach(BIN2, 20000, 8);
  fahre(0, 0);
  pinMode(VM_PIN, INPUT); pinMode(LINKS, INPUT); pinMode(RECHTS, INPUT);
  pinMode(TRIG, OUTPUT); pinMode(ECHO, INPUT);
  WiFi.mode(WIFI_AP);
  WiFi.softAP(WLAN_NAME, WLAN_PASSWORT);
  server.on("/", []() { server.send(200, "text/html", SEITE); });
  server.on("/fahr", fahrBefehl);
  server.on("/modus", modusBefehl);
  server.begin();
}

float integral = 0, letzterFehler = 0, letzteRichtung = 1;
unsigned long takt = 0, letzteMessung = 0;

void loop() {
  server.handleClient();

  // Herzschlag ausgeblieben: in jedem Modus anhalten (Mission 6.3)
  if (modus != STEHT && millis() - letzterBefehl > NOTAUS_MS) { bremsen(); modus = STEHT; }

  // Ultraschall alle 100 ms
  if (millis() - letzteMessung >= 100) {
    letzteMessung = millis();
    abstandMm = ultraschallMm();
    bool nah = abstandMm > 0 && abstandMm < HALT_CM * 10;
    if (nah && !hindernis && modus != STEHT) hindernisStopps++;
    hindernis = nah;
  }

  // Regeltakt 10 ms
  if (millis() - takt < 10) return;
  takt = millis();

  if (modus == FERN) {
    float v = fernV;
    if (hindernis && v > 0) v = 0;               // vorwärts gesperrt, rückwärts und drehen erlaubt
    if (fabs(v) < 0.05 && fabs(fernW) < 0.05) bremsen();
    else fahre(constrain(v - fernW, -1.0f, 1.0f) * MAX_PROZENT, constrain(v + fernW, -1.0f, 1.0f) * MAX_PROZENT);
  } else if (modus == LINIE) {
    if (hindernis) { bremsen(); return; }       // wartet, bis das Hindernis weg ist
    float nl = normiert(sl), nr = normiert(sr), e;
    if (nl < 0.1 && nr < 0.1) e = fabs(letzterFehler) > 0.3 ? letzteRichtung : 0;
    else { e = nl - nr; if (fabs(e) > 0.1) letzteRichtung = e > 0 ? 1 : -1; }
    integral = constrain(integral + e * 0.01f, -1.0f, 1.0f);
    float u = constrain(KP * e + KI * integral + KD * (e - letzterFehler) / 0.01f, -1.0f, 1.0f);
    letzterFehler = e;
    fahre(FAHRT * (1 - u), FAHRT * (1 + u));
    linieMs += 10;
  }
}
