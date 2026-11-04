// 0 to Hero – Mission 4.1: Wechsel auf ESP32-CAM
// Board in der Arduino IDE: "AI Thinker ESP32-CAM".
// Lässt die kleine rote LED auf der Rückseite blinken und prüft, ob das PSRAM erkannt wird.

const int ROTE_LED = 33;   // rote LED neben dem Reset-Taster, umgekehrte Logik: LOW = an

void setup() {
  Serial.begin(115200);
  delay(500);
  pinMode(ROTE_LED, OUTPUT);
  Serial.println();
  Serial.print("Chip: ");
  Serial.println(ESP.getChipModel());
  Serial.printf("Flash-Speicher: %u KiB\n", (unsigned)(ESP.getFlashChipSize() / 1024));
  if (psramFound()) {
    Serial.printf("PSRAM gefunden: %u KiB\n", (unsigned)(ESP.getPsramSize() / 1024));
  } else {
    Serial.println("Kein PSRAM gefunden. Ist als Board \"AI Thinker ESP32-CAM\" gewählt?");
  }
}

void loop() {
  digitalWrite(ROTE_LED, LOW);    // an
  delay(300);
  digitalWrite(ROTE_LED, HIGH);   // aus
  delay(700);
  if (psramFound()) {
    Serial.printf("KW-CHECK:m-4-1:%012llX:%u\n", (unsigned long long)ESP.getEfuseMac(), (unsigned)ESP.getPsramSize());
  }
}
