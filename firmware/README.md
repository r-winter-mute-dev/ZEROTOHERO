# Firmware

Ein Ordner pro Akt, darin ein Arduino-Sketch pro Mission. Jeder Sketch ist für sich lauffähig: Ordner in der Arduino IDE öffnen, Board **ESP32 Dev Module** wählen, hochladen.

| Ordner | Akt | Stand |
| --- | --- | --- |
| `akt-2-gehirn/` | 2 | 8 Sketches: Blink, serielle Ausgabe, Variablen, Funktionen, Ausgang, Eingang, Entprellen, Interrupts |
| `akt-3-stimme/` | 3 | folgt: WLAN, HTTPS, Nachricht an Discord/Telegram |
| `akt-4-auge/` | 4 | folgt: Foto aufnehmen und senden (ESP32-CAM) |
| `akt-6-schlaf/` | 6 | folgt: Tiefschlaf, Aufwachen per Taster, Akkustand |
| `akt-7-schalter/` | 7 | folgt: Selbsthalteschaltung |

**Pins in Akt 2:** LED intern an GPIO 2 (nicht jedes Board hat eine), externe LED an GPIO 23, Taster an GPIO 4 gegen GND mit internem Pull-up.

**Gerätenachweis:** Jeder Sketch gibt über die serielle Schnittstelle (115200 Baud) eine Zeile im Format `KW-CHECK:<missions-id>:<chip-kennung>` aus. Die Komponente `Seriell` auf der Missionsseite liest diese Zeile über Web Serial oder per Handeingabe.

**Zugangsdaten** (WLAN, Webhook-URL, Bot-Token) gehören ab Akt 3 in eine Datei `secrets.h`, die per `.gitignore` vom Repository ausgeschlossen ist.

Die Sketches sind syntaktisch geprüft, aber noch nicht auf jeder Board-Variante getestet. Rückmeldungen sind willkommen.
