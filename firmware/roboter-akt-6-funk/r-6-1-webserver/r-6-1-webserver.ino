// Kampagne 2, Mission 6.1: Ein Webserver
// Board: ESP32 Dev Module. Läuft am USB, die Motoren bleiben aus.
//
// Der Roboter öffnet sein eigenes WLAN (Access Point). Verbinde dein Handy damit und öffne
// im Browser http://192.168.4.1 . Die Seite zeigt Akkuspannung und Laufzeit und aktualisiert sich jede Sekunde.

#include <WiFi.h>
#include <WebServer.h>

const char* WLAN_NAME = "Roboter-0toHero";
const char* WLAN_PASSWORT = "bitte-aendern";   // mindestens 8 Zeichen, eigenes wählen!
const int VM_PIN = 34;

WebServer server(80);
bool gemeldet = false;

const char SEITE[] = R"rawliteral(<!DOCTYPE html>
<html lang="de"><head><meta charset="utf-8"><meta name="viewport" content="width=device-width, initial-scale=1">
<title>Roboter</title>
<style>body{font-family:sans-serif;background:#111;color:#eee;margin:2rem}b{color:#cba6f7}</style></head>
<body><h1>Hallo vom Roboter</h1>
<p>Akku: <b id="akku">?</b> V</p><p>Läuft seit: <b id="zeit">?</b> s</p>
<script>
async function holen(){
  try { const r = await fetch('/status'); const d = await r.json();
        document.getElementById('akku').textContent = d.akku.toFixed(2);
        document.getElementById('zeit').textContent = d.zeit; } catch(e) {}
}
holen(); setInterval(holen, 1000);
</script></body></html>)rawliteral";

void startseite() {
  server.send(200, "text/html", SEITE);
  if (!gemeldet) {
    Serial.printf("KW-CHECK:r-6-1:%012llX\n", (unsigned long long)ESP.getEfuseMac());
    gemeldet = true;
  }
}

void status() {
  float akku = analogReadMilliVolts(VM_PIN) * 2 / 1000.0;   // am USB ohne Akku: etwa 0
  String json = String("{\"akku\":") + String(akku, 2) + ",\"zeit\":" + String((unsigned long)(millis() / 1000)) + "}";
  server.send(200, "application/json", json);
}

void setup() {
  Serial.begin(115200);
  delay(500);
  pinMode(VM_PIN, INPUT);
  WiFi.mode(WIFI_AP);
  WiFi.softAP(WLAN_NAME, WLAN_PASSWORT);
  Serial.println();
  Serial.print("WLAN offen: "); Serial.println(WLAN_NAME);
  Serial.print("Adresse: http://"); Serial.println(WiFi.softAPIP());
  server.on("/", startseite);
  server.on("/status", status);
  server.begin();
}

void loop() {
  server.handleClient();   // so oft wie möglich: Anfragen annehmen und beantworten
}
