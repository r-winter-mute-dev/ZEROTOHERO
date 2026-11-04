// 0 to Hero – Mission 4.3: Erstes Foto
// Nimmt ein Foto in VGA (640 × 480) auf und gibt aus, wie groß es ist.
// Board: "AI Thinker ESP32-CAM".

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

void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println();
  if (!kameraStarten(FRAMESIZE_VGA, 12)) return;
  Serial.println("Kamera läuft.");

  camera_fb_t* fb = fotoMachen();
  if (!fb) { Serial.println("Kein Bild bekommen."); return; }

  unsigned long roh = (unsigned long)fb->width * fb->height * 2;   // so groß wäre es roh (RGB565)
  Serial.printf("Foto: %u × %u Pixel\n", (unsigned)fb->width, (unsigned)fb->height);
  Serial.printf("JPEG-Größe: %u Byte (%.1f KB)\n", (unsigned)fb->len, fb->len / 1024.0);
  Serial.printf("Roh wären es %lu Byte, also etwa %.0f-mal so viel.\n", roh, (double)roh / fb->len);
  Serial.printf("KW-CHECK:m-4-3:%012llX:%ux%u:%u\n", (unsigned long long)ESP.getEfuseMac(), (unsigned)fb->width, (unsigned)fb->height, (unsigned)fb->len);
  esp_camera_fb_return(fb);       // Puffer zurückgeben, sonst ist der Speicher weg
}

void loop() {}
