// 0 to Hero – Mission 3.1: Ins WLAN
// Verbindet den ESP32 mit deinem WLAN und zeigt IP-Adresse und Signalstärke.
// Der ESP32 kennt nur 2,4-GHz-Netze. Ein reines 5-GHz-Netz findet er nicht.
//
// Achtung: Hier stehen WLAN-Name und Passwort noch direkt im Code.
// Teile diese Datei so nicht. In Mission 3.4 ziehst du sie in eine eigene Datei um.

#include <WiFi.h>

const char* WLAN_NAME = "DEIN_WLAN";      // <- Name deines WLANs (SSID)
const char* WLAN_PASSWORT = "DEIN_PASSWORT"; // <- Passwort

// Übersetzt den WLAN-Status in Klartext, damit du Fehler erkennst.
const char* statusText(wl_status_t s) {
  switch (s) {
    case WL_IDLE_STATUS:    return "wartet";
    case WL_NO_SSID_AVAIL:  return "WLAN nicht gefunden (Name falsch oder nur 5 GHz?)";
    case WL_CONNECTED:      return "verbunden";
    case WL_CONNECT_FAILED: return "Verbindung abgelehnt (Passwort falsch?)";
    case WL_CONNECTION_LOST: return "Verbindung verloren";
    case WL_DISCONNECTED:   return "getrennt";
    default:                return "unbekannt";
  }
}

void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println();
  Serial.print("Verbinde mit ");
  Serial.println(WLAN_NAME);

  WiFi.mode(WIFI_STA);              // als "Station", also wie ein Handy, das sich einwählt
  WiFi.begin(WLAN_NAME, WLAN_PASSWORT);

  unsigned long start = millis();
  wl_status_t letzter = WL_IDLE_STATUS;
  while (WiFi.status() != WL_CONNECTED && millis() - start < 20000) {
    wl_status_t s = WiFi.status();
    if (s != letzter) {
      Serial.print("Status: ");
      Serial.println(statusText(s));
      letzter = s;
    }
    delay(250);
  }

  if (WiFi.status() != WL_CONNECTED) {
    Serial.print("Keine Verbindung nach 20 Sekunden. Letzter Status: ");
    Serial.println(statusText(WiFi.status()));
    return;
  }

  Serial.println("Verbunden!");
  Serial.print("IP-Adresse (vom Router per DHCP): ");
  Serial.println(WiFi.localIP());
  Serial.print("Signalstärke (RSSI): ");
  Serial.print(WiFi.RSSI());
  Serial.println(" dBm");
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    Serial.printf("KW-CHECK:m-3-1:%012llX:%s\n", (unsigned long long)ESP.getEfuseMac(), WiFi.localIP().toString().c_str());
  }
  delay(5000);
}
