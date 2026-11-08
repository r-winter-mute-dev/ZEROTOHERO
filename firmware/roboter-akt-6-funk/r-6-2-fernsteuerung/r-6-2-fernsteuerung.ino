// Kampagne 2, Mission 6.2 und 6.3: Fernsteuerung mit Notaus
// Board: ESP32 Dev Module. Akku-Betrieb wie ab Mission 2.2. Fährt auf dem Boden!
// Motoren: AIN1 25, AIN2 26 (links)  BIN1 32, BIN2 33 (rechts)  nSLEEP 27   VM-Teiler: GPIO 34
//
// Handy mit dem WLAN des Roboters verbinden, http://192.168.4.1 öffnen.
// Die Seite schickt alle 100 ms, wie schnell gefahren und gelenkt werden soll.
// Notaus: Kommt NOTAUS_MS lang kein Befehl, bremst der Roboter sofort (Mission 6.3).

#include <WiFi.h>
#include <WebServer.h>

const char* WLAN_NAME = "Roboter-0toHero";
const char* WLAN_PASSWORT = "bitte-aendern";   // mindestens 8 Zeichen, eigenes wählen!
const int AIN1 = 25, AIN2 = 26, BIN1 = 32, BIN2 = 33, SLEEP = 27, VM_PIN = 34;
const int ANLAUF = 85;
const int MAX_PROZENT = 60;               // Höchstgeschwindigkeit für die Fernsteuerung
const unsigned long NOTAUS_MS = 300;      // Mission 6.3

WebServer server(80);
unsigned long letzterBefehl = 0;
bool steht = true;
unsigned long befehle = 0, notstopps = 0;

int prozentZuPwm(float p) { p = constrain(p, -100.0f, 100.0f); if (fabs(p) < 0.5) return 0; int b = map((long)fabs(p), 1, 100, ANLAUF, 255); return p > 0 ? b : -b; }
void fahre(float l, float r) {
  int a = prozentZuPwm(l), b = prozentZuPwm(r);
  ledcWrite(AIN1, a > 0 ? a : 0); ledcWrite(AIN2, a < 0 ? -a : 0);
  ledcWrite(BIN1, b > 0 ? b : 0); ledcWrite(BIN2, b < 0 ? -b : 0);
}
void bremsen() { ledcWrite(AIN1, 255); ledcWrite(AIN2, 255); ledcWrite(BIN1, 255); ledcWrite(BIN2, 255); }

const char SEITE[] = R"rawliteral(<!DOCTYPE html>
<html lang="de"><head><meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1, user-scalable=no">
<title>Fernsteuerung</title>
<style>
body{font-family:sans-serif;background:#111;color:#eee;margin:1rem;text-align:center;user-select:none;-webkit-user-select:none}
#feld{width:280px;height:280px;margin:1rem auto;border-radius:50%;background:#222;border:3px solid #cba6f7;position:relative;touch-action:none}
#knopf{width:70px;height:70px;border-radius:50%;background:#cba6f7;position:absolute;left:105px;top:105px}
#stop{font-size:1.4rem;padding:1rem 2.5rem;background:#f38ba8;color:#111;border:0;border-radius:1rem}
small{color:#999}
</style></head><body>
<h2>Fernsteuerung</h2>
<div id="feld"><div id="knopf"></div></div>
<button id="stop">STOPP</button>
<p>Antwortzeit: <b id="ms">–</b> ms · Akku: <b id="akku">–</b> V</p>
<p><small id="nachweis"></small></p>
<script>
const feld = document.getElementById('feld'), knopf = document.getElementById('knopf');
let v = 0, w = 0, aktiv = false;
function setzen(e) {
  const r = feld.getBoundingClientRect(), p = e.touches ? e.touches[0] : e;
  let x = (p.clientX - r.left - 140) / 140, y = (p.clientY - r.top - 140) / 140;
  const l = Math.hypot(x, y); if (l > 1) { x /= l; y /= l; }
  v = -y; w = -x;                      // oben = vorwärts, links = nach links lenken
  knopf.style.left = (105 + x * 140) + 'px'; knopf.style.top = (105 + y * 140) + 'px';
}
function los(e) { aktiv = true; setzen(e); e.preventDefault(); }
function weg() { aktiv = false; v = 0; w = 0; knopf.style.left = '105px'; knopf.style.top = '105px'; }
feld.addEventListener('pointerdown', los); feld.addEventListener('pointermove', e => { if (aktiv) setzen(e); });
window.addEventListener('pointerup', weg); window.addEventListener('pointercancel', weg);
document.getElementById('stop').addEventListener('click', weg);
// Alle 100 ms einen Befehl schicken, auch wenn sich nichts ändert: Das ist der Herzschlag.
setInterval(async () => {
  const t0 = performance.now();
  try {
    const r = await fetch('/fahr?v=' + v.toFixed(2) + '&w=' + w.toFixed(2));
    const d = await r.json();
    document.getElementById('ms').textContent = Math.round(performance.now() - t0);
    document.getElementById('akku').textContent = d.akku.toFixed(2);
    document.getElementById('nachweis').textContent = d.nachweis;
  } catch (e) { document.getElementById('ms').textContent = 'keine Verbindung'; }
}, 100);
</script></body></html>)rawliteral";

void fahrBefehl() {
  float v = constrain(server.arg("v").toFloat(), -1.0f, 1.0f);   // vorwärts/rückwärts
  float w = constrain(server.arg("w").toFloat(), -1.0f, 1.0f);   // lenken, + = links
  letzterBefehl = millis();
  befehle++;
  if (fabs(v) < 0.05 && fabs(w) < 0.05) { if (!steht) bremsen(); steht = true; }
  else {
    // Differenzialantrieb wie in Mission 2.4: links = v − w, rechts = v + w
    float l = constrain(v - w, -1.0f, 1.0f), r = constrain(v + w, -1.0f, 1.0f);
    fahre(l * MAX_PROZENT, r * MAX_PROZENT);
    steht = false;
  }
  char nachweis[64] = "";
  if (befehle >= 300 && notstopps >= 1) snprintf(nachweis, sizeof nachweis, "KW-CHECK:r-6-3:%012llX:%lu", (unsigned long long)ESP.getEfuseMac(), notstopps);
  else if (befehle >= 300) snprintf(nachweis, sizeof nachweis, "KW-CHECK:r-6-2:%012llX:%lu", (unsigned long long)ESP.getEfuseMac(), befehle);
  float akku = analogReadMilliVolts(VM_PIN) * 2 / 1000.0;
  server.send(200, "application/json", String("{\"akku\":") + String(akku, 2) + ",\"nachweis\":\"" + nachweis + "\"}");
}

void setup() {
  pinMode(SLEEP, OUTPUT); digitalWrite(SLEEP, HIGH); delay(2);
  ledcAttach(AIN1, 20000, 8); ledcAttach(AIN2, 20000, 8);
  ledcAttach(BIN1, 20000, 8); ledcAttach(BIN2, 20000, 8);
  fahre(0, 0);
  pinMode(VM_PIN, INPUT);
  WiFi.mode(WIFI_AP);
  WiFi.softAP(WLAN_NAME, WLAN_PASSWORT);
  server.on("/", []() { server.send(200, "text/html", SEITE); });
  server.on("/fahr", fahrBefehl);
  server.begin();
}

void loop() {
  server.handleClient();
  // Notaus: Herzschlag ausgeblieben? Dann sofort bremsen.
  if (!steht && millis() - letzterBefehl > NOTAUS_MS) {
    bremsen();
    steht = true;
    notstopps++;
  }
}
