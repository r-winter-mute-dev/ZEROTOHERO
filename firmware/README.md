# Firmware

Hier entsteht ab Akt 2 ein PlatformIO-Projekt pro Akt. Jeder Stand ist für sich lauffähig.

Geplant:

| Ordner | Akt | Inhalt |
| --- | --- | --- |
| `akt-2-gehirn/` | 2 | Blink, serielle Ausgabe, Taster mit Entprellen (ESP32-DevKit) |
| `akt-3-stimme/` | 3 | WLAN, HTTPS, Nachricht an Discord/Telegram |
| `akt-4-auge/` | 4 | Foto aufnehmen und senden (ESP32-CAM) |
| `akt-6-schlaf/` | 6 | Tiefschlaf, Aufwachen per Taster, Akkustand |
| `akt-7-schalter/` | 7 | Selbsthalteschaltung |

**Gerätenachweis:** Jede Prüfungs-Firmware gibt über die serielle Schnittstelle (115200 Baud) eine Zeile im Format `KW-CHECK:<missions-id>:<ergebnis>` aus. Die Komponente `Seriell` auf der Missionsseite liest diese Zeile über Web Serial.

**Zugangsdaten** (WLAN, Webhook-URL, Bot-Token) gehören in eine Datei `secrets.h`, die per `.gitignore` vom Repository ausgeschlossen ist.
