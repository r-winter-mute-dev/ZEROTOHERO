// 0 to Hero – Mission 3.4: Geheimnisse
// WLAN-Name und Passwort stehen jetzt in secrets.h, nicht mehr im Programm.
// Fehlt secrets.h, bricht das Übersetzen mit einer Meldung ab. Das ist Absicht.

#include <WiFi.h>
#include <string.h>

#if __has_include("secrets.h")
#include "secrets.h"
#else
#error "secrets.h fehlt: Kopiere secrets.example.h, nenne die Kopie secrets.h und trag deine Daten ein."
#endif

void setup() {
  Serial.begin(115200);
  delay(500);

  if (strcmp(WLAN_NAME, "DEIN_WLAN") == 0) {
    Serial.println("In secrets.h steht noch der Platzhalter. Trag deinen WLAN-Namen ein.");
    return;
  }

  WiFi.mode(WIFI_STA);
  WiFi.begin(WLAN_NAME, WLAN_PASSWORT);
  Serial.print("Verbinde");
  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < 20000) { Serial.print("."); delay(250); }
  Serial.println();
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Verbunden, und kein Passwort steht im Programm.");
  } else {
    Serial.println("Keine Verbindung. Stimmen die Daten in secrets.h?");
  }
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    Serial.printf("KW-CHECK:m-3-4:%012llX:secrets-ok\n", (unsigned long long)ESP.getEfuseMac());
  }
  delay(5000);
}
