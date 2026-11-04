// 0 to Hero – Mission 5.5: Belastungstest am Akku
// Läuft OHNE USB, nur am Akku. Deshalb gibt es keinen seriellen Monitor:
// Das Ergebnis kommt als Nachricht in deinen Chat.
//
// Der Test macht fünf Runden mit der größten Last der Klingel: Blitz an, Foto, WLAN aktiv.
// Startet der Chip dabei wegen eines Spannungseinbruchs neu (Brownout), zählt er das mit.
// Nach dem Bericht beginnt die Zählung von vorne.
//
// In diesem Ordner brauchst du: secrets.h (aus secrets.example.h) und wurzeln.h.
// Board: "AI Thinker ESP32-CAM".

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <Preferences.h>
#include <time.h>
#include "esp_system.h"
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

// ---------- Textnachricht in den Chat (wie in Mission 3.5) ----------
int postJson(const String& url, const String& json) {
  WiFiClientSecure client;
  client.setCACert(WURZELN);
  HTTPClient https;
  if (!https.begin(client, url)) return -100;
  https.addHeader("Content-Type", "application/json");
  int code = https.POST(json);
  https.end();
  return code;
}

int nachrichtSenden(const String& text) {
#if DIENST == 1
  return postJson(DISCORD_WEBHOOK, String("{\"content\":\"") + text + "\"}");
#elif DIENST == 2
  return postJson(String("https://api.telegram.org/bot") + TELEGRAM_TOKEN + "/sendMessage",
                  String("{\"chat_id\":\"") + TELEGRAM_CHAT_ID + "\",\"text\":\"" + text + "\"}");
#else
#error "DIENST in secrets.h muss 1 (Discord) oder 2 (Telegram) sein."
#endif
}

const int BLITZ = 4;
const int RUNDEN = 5;
Preferences speicher;   // bleibt auch nach einem Neustart erhalten

void setup() {
  Serial.begin(115200);
  pinMode(BLITZ, OUTPUT);
  digitalWrite(BLITZ, LOW);

  // Warum ist der Chip gerade gestartet?
  speicher.begin("kw55", false);
  int brownouts = speicher.getInt("brownouts", 0);
  if (esp_reset_reason() == ESP_RST_BROWNOUT) {
    brownouts++;
    speicher.putInt("brownouts", brownouts);
  }

  if (!kameraStarten(FRAMESIZE_VGA, 12)) return;
  if (!wlanVerbinden(15000)) return;
  if (!uhrzeitHolen(10000)) return;

  // Fünf Runden größte Last: Blitz, Foto, WLAN
  for (int i = 1; i <= RUNDEN; i++) {
    digitalWrite(BLITZ, HIGH);
    camera_fb_t* fb = fotoMachen();
    digitalWrite(BLITZ, LOW);
    if (fb) esp_camera_fb_return(fb);
    WiFiClientSecure c;
    c.setCACert(WURZELN);
    HTTPClient h;
    if (h.begin(c, "https://discord.com/api/v10/gateway")) { h.GET(); h.end(); }
    delay(500);
  }

  char kennung[13];
  snprintf(kennung, sizeof(kennung), "%012llX", (unsigned long long)ESP.getEfuseMac());
  String bericht = String("Belastungstest: ") + RUNDEN + " Runden, Brownouts: " + brownouts
          + ". Nachweis: KW-CHECK:m-5-5:" + kennung + ":" + brownouts + ":" + RUNDEN;
  int code = nachrichtSenden(bericht);
  Serial.println(bericht);
  Serial.printf("Statuscode: %d\n", code);

  if (code == 200 || code == 204) speicher.putInt("brownouts", 0);   // Bericht angekommen: neu zählen
  speicher.end();
}

void loop() {}
