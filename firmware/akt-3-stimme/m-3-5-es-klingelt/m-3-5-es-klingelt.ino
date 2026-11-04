// 0 to Hero – Mission 3.5: Es klingelt
// Ein Druck auf den Klingelknopf (GPIO 4 gegen GND) schickt eine Nachricht
// nach Discord (Webhook) oder Telegram (Bot-API). Was davon, steht in secrets.h.
//
// In diesem Ordner brauchst du: secrets.h (aus secrets.example.h) und wurzeln.h.

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <time.h>
#include "wurzeln.h"

#if __has_include("secrets.h")
#include "secrets.h"
#else
#error "secrets.h fehlt: Kopiere secrets.example.h, nenne die Kopie secrets.h und trag deine Daten ein."
#endif

const int TASTER_PIN = 4;
const unsigned long ENTPRELLZEIT = 50;     // ms, wie in Mission 2.7
const unsigned long SPERRZEIT = 10000;     // ms: höchstens eine Nachricht alle 10 Sekunden

volatile bool gedrueckt = false;
volatile unsigned long letzterInterrupt = 0;
unsigned long letzteNachricht = 0;
int klingelZaehler = 0;

void IRAM_ATTR tasterISR() {               // Interrupt wie in Mission 2.8: nur einen Merker setzen
  unsigned long jetzt = millis();
  if (jetzt - letzterInterrupt > ENTPRELLZEIT) gedrueckt = true;
  letzterInterrupt = jetzt;
}

void wlanVerbinden() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WLAN_NAME, WLAN_PASSWORT);
  Serial.print("WLAN");
  while (WiFi.status() != WL_CONNECTED) { Serial.print("."); delay(250); }
  Serial.print(" verbunden, IP ");
  Serial.println(WiFi.localIP());
}

void uhrzeitHolen() {
  configTime(0, 0, "pool.ntp.org", "time.google.com");
  Serial.print("Uhrzeit");
  while (time(nullptr) < 1700000000) { Serial.print("."); delay(250); }
  Serial.println(" gesetzt");
}

// Sendet einen JSON-Text per HTTPS-POST und gibt den Statuscode zurück.
int postJson(const String& url, const String& json) {
  WiFiClientSecure client;
  client.setCACert(WURZELN);
  HTTPClient https;
  if (!https.begin(client, url)) return -100;
  https.addHeader("Content-Type", "application/json");
  int code = https.POST(json);
  if (code < 0) {
    Serial.print("Verbindungsfehler: ");
    Serial.println(https.errorToString(code));
  } else if (code >= 400) {
    Serial.print("Antwort vom Server: ");
    Serial.println(https.getString());   // dort steht meist, was nicht stimmt
  }
  https.end();
  return code;
}

int nachrichtSenden(const String& text) {
#if DIENST == 1
  String json = "{\"content\":\"" + text + "\"}";
  int code = postJson(DISCORD_WEBHOOK, json);
  bool ok = (code == 200 || code == 204);   // Discord antwortet mit 204 (ohne Inhalt) oder 200
#elif DIENST == 2
  String url = String("https://api.telegram.org/bot") + TELEGRAM_TOKEN + "/sendMessage";
  String json = String("{\"chat_id\":\"") + TELEGRAM_CHAT_ID + "\",\"text\":\"" + text + "\"}";
  int code = postJson(url, json);
  bool ok = (code == 200);
#else
#error "DIENST in secrets.h muss 1 (Discord) oder 2 (Telegram) sein."
#endif
  Serial.print("Statuscode: ");
  Serial.print(code);
  Serial.println(ok ? " (angekommen)" : " (nicht angekommen)");
  if (ok) Serial.printf("KW-CHECK:m-3-5:%012llX:%d\n", (unsigned long long)ESP.getEfuseMac(), code);
  return code;
}

void setup() {
  Serial.begin(115200);
  delay(500);
  pinMode(TASTER_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(TASTER_PIN), tasterISR, FALLING);
  wlanVerbinden();
  uhrzeitHolen();
  Serial.println("Bereit. Drück den Klingelknopf.");
}

void loop() {
  if (gedrueckt) {
    gedrueckt = false;
    if (millis() - letzteNachricht < SPERRZEIT && letzteNachricht != 0) {
      Serial.println("Zu schnell hintereinander, ich warte noch.");
      return;
    }
    letzteNachricht = millis();
    klingelZaehler++;
    Serial.println("Es klingelt! Sende Nachricht ...");
    nachrichtSenden("Es klingelt! (Nummer " + String(klingelZaehler) + " seit dem Start)");
  }
  if (WiFi.status() != WL_CONNECTED) {      // WLAN weg? Neu verbinden.
    Serial.println("WLAN verloren, verbinde neu.");
    wlanVerbinden();
  }
}
