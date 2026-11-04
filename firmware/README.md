# Firmware

Ein Ordner pro Akt, darin ein Arduino-Sketch pro Mission. Jeder Sketch ist für sich lauffähig: Ordner in der Arduino IDE öffnen, Board **ESP32 Dev Module** wählen, hochladen.

| Ordner | Akt | Stand |
| --- | --- | --- |
| `akt-2-gehirn/` | 2 | 8 Sketches: Blink, serielle Ausgabe, Variablen, Funktionen, Ausgang, Eingang, Entprellen, Interrupts |
| `akt-3-stimme/` | 3 | 4 Sketches: WLAN, HTTPS mit Zertifikatsprüfung, Zugangsdaten in `secrets.h`, Nachricht an Discord oder Telegram |
| `akt-4-auge/` | 4 | 5 Sketches für den ESP32-CAM (Board „AI Thinker ESP32-CAM“): PSRAM-Test, erstes Foto, Foto senden, Blitz mit PWM, Klingel v1 als Zustandsautomat |
| `akt-5-herz/` | 5 | 1 Sketch: Belastungstest am Akku, das Ergebnis kommt als Nachricht in den Chat |
| `akt-6-schlaf/` | 6 | folgt: Tiefschlaf, Aufwachen per Taster, Akkustand |
| `akt-7-schalter/` | 7 | folgt: Selbsthalteschaltung |

**Pins ESP32-CAM (Akt 4):** Blitz-LED an GPIO 4 (über Transistor), rote LED an GPIO 33 (LOW = an), Kamerapins laut AI-Thinker-Schaltplan. Als Klingelknopf dient in Akt 4 die Reset-Taste.

**Pins in Akt 2:** LED intern an GPIO 2 (nicht jedes Board hat eine), externe LED an GPIO 23, Taster an GPIO 4 gegen GND mit internem Pull-up.

**Gerätenachweis:** Jeder Sketch gibt über die serielle Schnittstelle (115200 Baud) eine Zeile im Format `KW-CHECK:<missions-id>:<chip-kennung>` aus. Die Komponente `Seriell` auf der Missionsseite liest diese Zeile über Web Serial oder per Handeingabe.

**Zugangsdaten** (WLAN, Webhook-URL, Bot-Token) gehören ab Mission 3.4 in eine Datei `secrets.h`, die per `.gitignore` vom Repository ausgeschlossen ist. Jeder Sketch, der sie braucht, liegt mit einer Vorlage `secrets.example.h` bereit: kopieren, umbenennen, ausfüllen.

**Zertifikate:** `wurzeln.h` enthält neun Wurzelzertifikate aus dem Mozilla-Zertifikatsspeicher (Let's Encrypt, Google Trust Services, SSL.com, Sectigo, DigiCert, Go Daddy). Damit prüft der ESP32 die Server von Discord und Telegram, statt die Prüfung mit `setInsecure()` abzuschalten. Neu erzeugen mit `python3 firmware/werkzeuge/wurzeln_erzeugen.py` (Linux mit Paket `ca-certificates`). Eine Kopie liegt unter `public/firmware/wurzeln.h` zum Herunterladen auf der Seite.

Die Sketches sind syntaktisch geprüft, aber noch nicht auf jeder Board-Variante getestet. Rückmeldungen sind willkommen.
