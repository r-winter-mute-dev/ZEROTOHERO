// 0 to Hero – Mission 6.4: Klingel v3 mit Akkustand
// Wie Klingel v2 (Tiefschlaf, Aufwachen über GPIO 13), dazu misst sie bei jedem
// Klingeln die Akkuspannung und schreibt sie unter das Foto.
//
// Spannungsteiler: OUT+ des Lademoduls -> 10 kΩ -> GPIO 14 -> 10 kΩ -> GND.
// GPIO 14 gehört zum ADC2. Den kann der ESP32 nur benutzen, solange das WLAN aus ist.
// Deshalb misst die Klingel als Allererstes, noch vor dem WLAN.
//
// Die Klingel läuft am Akku, ohne USB. Den Nachweis findest du im Chat unter dem Foto.
//
// In diesem Ordner brauchst du: secrets.h (aus secrets.example.h) und wurzeln.h.
// Board: "AI Thinker ESP32-CAM". Keine SD-Karte einstecken: GPIO 13 gehört sonst zum Kartenslot.

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <time.h>
#include "esp_sleep.h"
#include "driver/rtc_io.h"
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

// ---------- Tiefschlaf ----------
const gpio_num_t KNOPF = GPIO_NUM_13;   // Klingelknopf gegen GND
const int BLITZ = 4;                    // Blitz-LED (über Transistor)
const int KAMERA_AUS = 32;              // PWDN der Kamera: HIGH = Kamera schaltet ab
const bool MIT_BLITZ = false;           // auf true setzen, wenn es an der Tür dunkel ist

// Variablen mit RTC_DATA_ATTR liegen im kleinen RTC-Speicher.
// Der bleibt im Tiefschlaf erhalten, der normale Arbeitsspeicher nicht.
RTC_DATA_ATTR int klingelZaehler = 0;

// Warum ist der Chip gerade wach?
const char* weckGrund() {
  switch (esp_sleep_get_wakeup_cause()) {
    case ESP_SLEEP_WAKEUP_EXT0:      return "knopf";   // GPIO 13 war LOW
    case ESP_SLEEP_WAKEUP_UNDEFINED: return "start";   // Strom an oder Reset-Taste
    default:                         return "anderes";
  }
}

// Nach dem Aufwachen: festgehaltene Pins wieder freigeben.
void pinsFreigeben() {
  gpio_deep_sleep_hold_dis();
  gpio_hold_dis(GPIO_NUM_4);
  gpio_hold_dis(GPIO_NUM_32);
  rtc_gpio_deinit(KNOPF);           // GPIO 13 wieder als normalen Pin benutzen
  pinMode(BLITZ, OUTPUT);
  digitalWrite(BLITZ, LOW);
  pinMode((int)KNOPF, INPUT_PULLUP);
}

void schlafen() {
  esp_camera_deinit();              // Kamera-Treiber beenden (meldet einen Fehler, wenn er nie lief: egal)
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);

  // Blitz aus und Kamera aus, und zwar so, dass es im Schlaf so bleibt.
  // Ohne "hold" verlieren die Pins im Tiefschlaf ihren Zustand.
  digitalWrite(BLITZ, LOW);
  gpio_hold_en(GPIO_NUM_4);
  pinMode(KAMERA_AUS, OUTPUT);
  digitalWrite(KAMERA_AUS, HIGH);
  gpio_hold_en(GPIO_NUM_32);
  gpio_deep_sleep_hold_en();

  // Erst schlafen, wenn der Knopf losgelassen ist. Sonst wacht der Chip sofort wieder auf.
  unsigned long t = millis();
  while (digitalRead((int)KNOPF) == LOW && millis() - t < 10000) delay(10);
  delay(50);                        // Prellen abwarten (Mission 2.7)

  // Aufwachen, sobald GPIO 13 LOW wird. Der interne Pull-up hält ihn sonst auf HIGH.
  rtc_gpio_pullup_en(KNOPF);
  rtc_gpio_pulldown_dis(KNOPF);
  esp_sleep_enable_ext0_wakeup(KNOPF, 0);

  Serial.println("Gute Nacht.");
  Serial.flush();
  esp_deep_sleep_start();           // ab hier schläft der Chip, setup() beginnt beim nächsten Druck neu
}

// ---------- Akkustand ----------
const int AKKU_PIN = 14;          // Mitte des Spannungsteilers
const float TEILER = 2.0;         // zwei gleiche Widerstände: Akku = 2 × Spannung am Pin
const float KORREKTUR = 1.000;    // aus Mission 6.4: Multimeterwert geteilt durch Klingelwert

float akkuMessen() {
  analogSetPinAttenuation(AKKU_PIN, ADC_11db);   // höchste Dämpfung: Messbereich bis etwa 2,45 V
  uint32_t summe = 0;
  for (int i = 0; i < 32; i++) {                 // 32 Messungen mitteln, das glättet Rauschen
    summe += analogReadMilliVolts(AKKU_PIN);     // schon mit der Werkskalibrierung des Chips
    delay(1);
  }
  float pin = summe / 32.0f / 1000.0f;           // Spannung am Pin in Volt
  return pin * TEILER * KORREKTUR;
}

String volt(float v) {                           // 3.95 -> "3,95"
  char t[8];
  snprintf(t, sizeof(t), "%.2f", v);
  for (char* p = t; *p; p++) if (*p == '.') *p = ',';
  return String(t);
}

const char* akkuWort(float v) {                  // grobe Einordnung, siehe Mission 6.4
  if (v >= 3.9f) return "voll";
  if (v >= 3.6f) return "mittel";
  if (v >= 3.3f) return "bald laden";
  return "leer, bitte laden";
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
#else
  return postJson(String("https://api.telegram.org/bot") + TELEGRAM_TOKEN + "/sendMessage",
                  String("{\"chat_id\":\"") + TELEGRAM_CHAT_ID + "\",\"text\":\"" + text + "\"}");
#endif
}

String kennung() {
  char k[13];
  snprintf(k, sizeof(k), "%012llX", (unsigned long long)ESP.getEfuseMac());
  return String(k);
}

// ---------- Ablauf: einmal von vorne nach hinten, dann schlafen ----------
void setup() {
  unsigned long start = millis();
  Serial.begin(115200);
  pinsFreigeben();

  const char* grund = weckGrund();
  Serial.printf("\nWach. Grund: %s\n", grund);

  // Beim Einschalten des Akkus (oder Reset) kein Foto, nur bereit machen.
  if (String(grund) != "knopf") {
    Serial.println("Bereit. Drück den Klingelknopf.");
    schlafen();
  }

  klingelZaehler++;

  // Zuerst den Akku messen: Danach startet das WLAN, und ADC2 ist gesperrt.
  float akku = akkuMessen();
  Serial.printf("Akku: %.3f V (Korrektur %.3f)\n", akku, KORREKTUR);

  // Unter 3,0 V nichts mehr senden: Das WLAN zieht Spitzen, die den Akku weiter herunterziehen.
  // Die Schutzschaltung trennt erst bei etwa 2,4 V, die Zelle ist laut Datenblatt bei 2,5 V leer.
  if (akku < 3.0f) {
    Serial.println("Akku zu leer. Kein Foto, gleich wieder schlafen. Bitte laden.");
    schlafen();
  }

  String fehler = "";
  if (!kameraStarten(FRAMESIZE_VGA, 12)) fehler = "Kamera startet nicht";
  else if (!wlanVerbinden(15000)) fehler = "kein WLAN";
  else if (!uhrzeitHolen(10000)) fehler = "keine Uhrzeit";

  if (fehler == "") {
    if (MIT_BLITZ) digitalWrite(BLITZ, HIGH);
    camera_fb_t* bild = fotoMachen();
    digitalWrite(BLITZ, LOW);
    if (!bild) fehler = "kein Bild";
    else {
      char nachweis[64];
      snprintf(nachweis, sizeof(nachweis), "KW-CHECK:m-6-4:%s:%d:%d",
               kennung().c_str(), (int)(akku * 1000 + 0.5f), (int)(KORREKTUR * 1000 + 0.5f));
      String text = String("Es klingelt! Akku: ") + volt(akku) + " V (" + akkuWort(akku) + "). Nachweis: " + nachweis;
      int code = fotoSenden(bild, text);
      esp_camera_fb_return(bild);
      if (!angekommen(code)) fehler = String("Statuscode ") + String(code);
      else Serial.println(text);
    }
  }

  if (fehler != "") {
    Serial.println(String("Fehler: ") + fehler);
    // Kam wenigstens das WLAN? Dann wenigstens eine Textnachricht schicken.
    if (WiFi.status() == WL_CONNECTED) nachrichtSenden(String("Klingel: ") + fehler + ", Akku " + volt(akku) + " V");
  }
  Serial.printf("Wach gewesen: %lu ms\n", millis() - start);
  schlafen();                       // auch nach einem Fehler: nie wach bleiben, sonst ist der Akku leer
}

void loop() {}                      // wird nie erreicht
