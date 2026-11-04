// 0 to Hero – Mission 3.3: Verschlüsselt
// Baut zwei HTTPS-Verbindungen auf und prüft dabei die Zertifikate.
// Kommt ein HTTP-Statuscode zurück (egal welcher), hat die Verschlüsselung geklappt.
// Eine negative Zahl heißt: Die Verbindung kam gar nicht zustande.
//
// In diesem Ordner muss die Datei wurzeln.h liegen (die Wurzelzertifikate).

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <time.h>
#include "wurzeln.h"

const char* WLAN_NAME = "DEIN_WLAN";
const char* WLAN_PASSWORT = "DEIN_PASSWORT";

// Zertifikate sind nur in einem bestimmten Zeitraum gültig.
// Deshalb braucht der ESP32 zuerst die richtige Uhrzeit aus dem Internet (NTP).
bool uhrzeitHolen() {
  configTime(0, 0, "pool.ntp.org", "time.google.com");
  Serial.print("Hole Uhrzeit");
  for (int i = 0; i < 40; i++) {
    if (time(nullptr) > 1700000000) {       // nach November 2023: Uhrzeit ist gesetzt
      time_t jetzt = time(nullptr);
      Serial.print("\nUhrzeit (UTC): ");
      Serial.print(ctime(&jetzt));
      return true;
    }
    Serial.print(".");
    delay(250);
  }
  Serial.println("\nKeine Uhrzeit bekommen.");
  return false;
}

int httpsAbrufen(const char* url) {
  WiFiClientSecure client;
  client.setCACert(WURZELN);     // nur Server akzeptieren, deren Kette bei einer dieser Wurzeln endet
  HTTPClient https;
  if (!https.begin(client, url)) return -100;
  int code = https.GET();
  Serial.print("GET ");
  Serial.print(url);
  Serial.print(" -> ");
  Serial.println(code);
  if (code < 0) {
    Serial.print("Fehler: ");
    Serial.println(https.errorToString(code));
  }
  https.end();
  return code;
}

void setup() {
  Serial.begin(115200);
  delay(500);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WLAN_NAME, WLAN_PASSWORT);
  Serial.print("WLAN");
  while (WiFi.status() != WL_CONNECTED) { Serial.print("."); delay(250); }
  Serial.println(" verbunden");

  uhrzeitHolen();

  int discord = httpsAbrufen("https://discord.com/api/v10/gateway");
  int telegram = httpsAbrufen("https://api.telegram.org/bot123:ungueltig/getMe");

  if (discord > 0 && telegram > 0) {
    Serial.println("Beide Verbindungen verschlüsselt und geprüft.");
    Serial.printf("KW-CHECK:m-3-3:%012llX:%d:%d\n", (unsigned long long)ESP.getEfuseMac(), discord, telegram);
  } else {
    Serial.println("Mindestens eine Verbindung ist gescheitert. Siehe Fehlermeldung oben.");
  }
}

void loop() {}
