// 0 to Hero – Mission 4.6: Klingel v1
// Jeder Druck auf den Reset-Taster der ESP32-CAM startet das Programm neu,
// und das Programm läuft einmal durch: WLAN, Uhrzeit, Foto, senden.
// So arbeitet später auch der Tiefschlaf (Akt 6): Aufwachen heißt neu starten.
// In diesem Ordner brauchst du: secrets.h (aus secrets.example.h) und wurzeln.h.
// Board: "AI Thinker ESP32-CAM".

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

// ---------- Kamera (AI-Thinker ESP32-CAM) ----------
// Pinbelegung laut AI-Thinker-Schaltplan. Für andere Kameraboards sind die Pins anders.
#include "esp_camera.h"

bool kameraStarten(framesize_t groesse, int qualitaet) {
  camera_config_t c = {};
  c.pin_pwdn = 32;  c.pin_reset = -1;  c.pin_xclk = 0;
  c.pin_sccb_sda = 26;  c.pin_sccb_scl = 27;
  c.pin_d7 = 35;  c.pin_d6 = 34;  c.pin_d5 = 39;  c.pin_d4 = 36;
  c.pin_d3 = 21;  c.pin_d2 = 19;  c.pin_d1 = 18;  c.pin_d0 = 5;
  c.pin_vsync = 25;  c.pin_href = 23;  c.pin_pclk = 22;
  c.xclk_freq_hz = 20000000;            // Takt für den Bildsensor: 20 MHz
  c.ledc_timer = LEDC_TIMER_0;
  c.ledc_channel = LEDC_CHANNEL_0;
  c.pixel_format = PIXFORMAT_JPEG;      // der Sensor liefert gleich ein fertiges JPEG
  c.frame_size = groesse;
  c.jpeg_quality = qualitaet;           // 10 = sehr gut, 63 = sehr schlecht
  c.fb_count = 2;                       // zwei Bildpuffer ...
  c.fb_location = CAMERA_FB_IN_PSRAM;   // ... im großen PSRAM
  c.grab_mode = CAMERA_GRAB_LATEST;     // immer das neueste Bild nehmen

  esp_err_t fehler = esp_camera_init(&c);
  if (fehler != ESP_OK) {
    Serial.printf("Kamera startet nicht: Fehler 0x%x (%s)\n", (unsigned)fehler, esp_err_to_name(fehler));
    return false;
  }
  return true;
}

// Die ersten Bilder nach dem Start sind oft zu dunkel oder grünstichig,
// weil die automatische Belichtung erst ein paar Bilder braucht. Die werfen wir weg.
camera_fb_t* fotoMachen() {
  for (int i = 0; i < 3; i++) {
    camera_fb_t* alt = esp_camera_fb_get();
    if (alt) esp_camera_fb_return(alt);
    delay(100);
  }
  return esp_camera_fb_get();
}

// ---------- WLAN und Uhrzeit (wie in Akt 3) ----------
bool wlanVerbinden(unsigned long maxMs) {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WLAN_NAME, WLAN_PASSWORT);
  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < maxMs) delay(100);
  return WiFi.status() == WL_CONNECTED;
}

bool uhrzeitHolen(unsigned long maxMs) {
  configTime(0, 0, "pool.ntp.org", "time.google.com");
  unsigned long start = millis();
  while (time(nullptr) < 1700000000 && millis() - start < maxMs) delay(100);
  return time(nullptr) >= 1700000000;
}

// ---------- Foto verschicken (multipart/form-data) ----------
// Der Inhalt besteht aus mehreren Teilen, getrennt durch eine Grenzlinie.
// Ein Teil ist Text (die Nachricht), ein Teil ist die JPEG-Datei.
const char* GRENZE = "0toHeroKlingelGrenze";

String teilText(const char* name, const String& wert) {
  return String("--") + GRENZE + "\r\n"
       + "Content-Disposition: form-data; name=\"" + name + "\"\r\n\r\n"
       + wert + "\r\n";
}

String teilDateiKopf(const char* name) {
  return String("--") + GRENZE + "\r\n"
       + "Content-Disposition: form-data; name=\"" + name + "\"; filename=\"klingel.jpg\"\r\n"
       + "Content-Type: image/jpeg\r\n\r\n";
}

int fotoSenden(camera_fb_t* fb, const String& text) {
  String url, kopf;
#if DIENST == 1
  url = DISCORD_WEBHOOK;
  kopf = teilText("payload_json", String("{\"content\":\"") + text + "\"}") + teilDateiKopf("files[0]");
#elif DIENST == 2
  url = String("https://api.telegram.org/bot") + TELEGRAM_TOKEN + "/sendPhoto";
  kopf = teilText("chat_id", String(TELEGRAM_CHAT_ID)) + teilText("caption", text) + teilDateiKopf("photo");
#else
#error "DIENST in secrets.h muss 1 (Discord) oder 2 (Telegram) sein."
#endif
  String fuss = String("\r\n--") + GRENZE + "--\r\n";

  // Alles in einem Stück im PSRAM zusammensetzen: Kopf + Bild + Fuß
  size_t laenge = kopf.length() + fb->len + fuss.length();
  uint8_t* inhalt = (uint8_t*)ps_malloc(laenge);
  if (!inhalt) { Serial.println("Kein Speicher für den Versand."); return -200; }
  memcpy(inhalt, kopf.c_str(), kopf.length());
  memcpy(inhalt + kopf.length(), fb->buf, fb->len);
  memcpy(inhalt + kopf.length() + fb->len, fuss.c_str(), fuss.length());

  WiFiClientSecure client;
  client.setCACert(WURZELN);
  HTTPClient https;
  https.setTimeout(20000);                 // große Dateien brauchen etwas länger
  int code = -100;
  if (https.begin(client, url)) {
    https.addHeader("Content-Type", String("multipart/form-data; boundary=") + GRENZE);
    code = https.POST(inhalt, laenge);
    if (code < 0) { Serial.print("Verbindungsfehler: "); Serial.println(https.errorToString(code)); }
    else if (code >= 400) { Serial.print("Antwort vom Server: "); Serial.println(https.getString()); }
    https.end();
  }
  free(inhalt);
  return code;
}

bool angekommen(int code) {
#if DIENST == 1
  return code == 200 || code == 204;
#else
  return code == 200;
#endif
}

const int BLITZ = 4;
const bool MIT_BLITZ = false;   // auf true setzen, wenn es an der Tür dunkel ist

// Die Zustände der Klingel. Jeder Zustand weiß, welcher als nächster kommt.
enum Zustand { START, KAMERA, WLAN, UHRZEIT, FOTO, SENDEN, FERTIG, FEHLER };
const char* NAMEN[] = {"START", "KAMERA", "WLAN", "UHRZEIT", "FOTO", "SENDEN", "FERTIG", "FEHLER"};

Zustand zustand = START;
camera_fb_t* bild = nullptr;
int statuscode = 0;
unsigned long startzeit = 0;
String grund = "";

void wechsel(Zustand neu, const String& warum = "") {
  Serial.printf("[%6lu ms] %s -> %s\n", millis() - startzeit, NAMEN[zustand], NAMEN[neu]);
  zustand = neu;
  grund = warum;
}

void setup() {
  startzeit = millis();
  Serial.begin(115200);
  delay(300);
  Serial.println();
  Serial.println("Es klingelt!");
  pinMode(BLITZ, OUTPUT);
  digitalWrite(BLITZ, LOW);
}

void loop() {
  switch (zustand) {
    case START:
      wechsel(KAMERA);
      break;
    case KAMERA:
      if (kameraStarten(FRAMESIZE_VGA, 12)) wechsel(WLAN);
      else wechsel(FEHLER, "Kamera startet nicht");
      break;
    case WLAN:
      if (wlanVerbinden(15000)) wechsel(UHRZEIT);
      else wechsel(FEHLER, "kein WLAN");
      break;
    case UHRZEIT:
      if (uhrzeitHolen(10000)) wechsel(FOTO);
      else wechsel(FEHLER, "keine Uhrzeit");
      break;
    case FOTO:
      if (MIT_BLITZ) digitalWrite(BLITZ, HIGH);
      bild = fotoMachen();
      digitalWrite(BLITZ, LOW);
      if (bild) wechsel(SENDEN);
      else wechsel(FEHLER, "kein Bild");
      break;
    case SENDEN:
      statuscode = fotoSenden(bild, "Es klingelt!");
      esp_camera_fb_return(bild);
      bild = nullptr;
      if (angekommen(statuscode)) wechsel(FERTIG);
      else wechsel(FEHLER, String("Statuscode ") + String(statuscode));
      break;
    case FERTIG: {
      unsigned long dauer = millis() - startzeit;
      Serial.printf("Geschafft in %lu ms. Zum nächsten Klingeln: Reset-Taster drücken.\n", dauer);
      Serial.printf("KW-CHECK:m-4-6:%012llX:%d:%lu\n", (unsigned long long)ESP.getEfuseMac(), statuscode, dauer);
      while (true) delay(1000);    // nichts mehr zu tun, bis zum nächsten Reset
    }
    case FEHLER:
      Serial.print("Fehler: ");
      Serial.println(grund);
      Serial.println("Zum neuen Versuch: Reset-Taster drücken.");
      while (true) delay(1000);
  }
}
