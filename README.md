# Klingelwerk

Ein kostenloser, interaktiver Elektronik-Kurs. Lernende fangen bei null an und bauen Mission für Mission eine smarte Türklingel: Ein ESP32-CAM mit 18650-Akku wacht auf Knopfdruck aus dem Tiefschlaf auf, macht ein Foto und schickt es an Telegram oder Discord.

Die Seite ist komplett statisch. Es gibt keinen Server, keine Anmeldung und keine Cookies. Der Fortschritt bleibt im Browser (localStorage) und lässt sich als Code exportieren.

## Stand

| Bereich | Stand |
| --- | --- |
| Spiel-Engine (XP, Level, Freischaltung, Abzeichen, Export/Import, Modus ohne Hardware) | fertig |
| Startseite mit Klingel, die sich füllt, und Kampagnenkarte | fertig |
| Akt 0 – Basislager (4 Missionen) | fertig |
| Akt 1 – Der Knopf (8 Missionen + Bosskampf) | fertig |
| Akt 2–8 (43 Missionen) | als geplante Missionen auf der Karte, Inhalte folgen |

## Auf GitHub Pages veröffentlichen

1. Neues Repository auf GitHub anlegen, zum Beispiel `klingelwerk`.
2. Alle Dateien dieses Ordners hochladen (ohne `node_modules` und `dist`) und auf den Branch `main` pushen:
   ```bash
   git init
   git add .
   git commit -m "Klingelwerk: erste Version"
   git branch -M main
   git remote add origin https://github.com/r-winter-mute-dev/klingelwerk.git
   git push -u origin main
   ```
3. Im Repository: **Settings → Pages → Build and deployment → Source: GitHub Actions** wählen.
4. Unter **Actions** läuft jetzt „Seite auf GitHub Pages veröffentlichen“. Nach etwa einer Minute ist die Seite erreichbar unter `https://DEIN-NAME.github.io/klingelwerk/`.

Jeder weitere Push auf `main` veröffentlicht die Seite automatisch neu. Der Workflow liegt in `.github/workflows/deploy.yml` und setzt den richtigen Pfad (`/klingelwerk/`) selbst, auch wenn du das Repository anders nennst oder eine eigene Domain einrichtest.

## Lokal entwickeln

Voraussetzung: Node.js 22 oder neuer.

```bash
npm install
npm run dev      # Entwicklungsserver auf http://localhost:4321
npm run build    # baut die fertige Seite nach dist/
npm run preview  # zeigt den Build lokal an
```

## Aufbau

```
src/
  config.ts              Name der Seite, Link zum Repository
  data/kampagne.ts       Alle Akte und Missionen (IDs, Titel, XP, Level)
  missionen/*.mdx        Ausgeschriebene Missionen, eine Datei pro Mission
  components/            Prüfungen und interaktive Bausteine
  scripts/engine.ts      Fortschritt, Punkte, Freischaltung, Export/Import
  scripts/station.ts     Gemeinsame Logik aller Prüfungen
  pages/                 Startseite, Missionsseiten, Fortschritt, Über
firmware/                Firmware für Akt 2+ (folgt)
hardware/                Schaltpläne, Lochraster-Layouts, Gehäuse (folgt)
```

## Eine neue Mission schreiben

1. In `src/data/kampagne.ts` bei der Mission `bereit: true` setzen. Die **ID nie ändern**, der gespeicherte Fortschritt hängt daran.
2. Datei `src/missionen/<id>.mdx` anlegen. Vorlage: `m-1-1.mdx`.
3. Im Frontmatter alle Prüfungen unter `checks` auflisten. `hardware: true` heißt: Diese Prüfung braucht echte Bauteile und wird im Modus „ohne Hardware“ übersprungen.
4. Jede Prüfungskomponente bekommt eine `id`, die genau so in `checks` steht.

```mdx
---
id: m-2-1
abzeichen: Erstes Leben
checks:
  - id: quiz
  - id: geraet
    hardware: true
---
import Quiz from '../components/Quiz.astro';
import Seriell from '../components/Seriell.astro';

Text der Mission …

<Quiz id="quiz" fragen={[{ frage: '…', richtig: '…', falsch: ['…', '…'] }]} />
<Seriell id="geraet" titel="Gerätenachweis" muster="KW-CHECK:m-2-1:blink-ok" />
```

### Verfügbare Bausteine

| Komponente | Zweck |
| --- | --- |
| `Karte` | Theorie-Karte (höchstens etwa 120 Wörter) |
| `Hinweis` | Kasten für Sicherheit, Hintergrund oder vorläufige Werte |
| `Quiz` | Multiple Choice, alle Antworten müssen stimmen |
| `Zahl` | Ein Messwert wird gegen ein Toleranzband geprüft, mit Hilfetexten je nach Abweichung |
| `Bestaetigung` | Abhaken („Meine LED leuchtet“) |
| `Rechnen` | Rechenaufgaben mit Zufallswerten: `vorwiderstand`, `strom`, `reihe`, `parallel`, `teiler`, `leistung` |
| `Protokoll` | Mehrere echte Messwerte werden gegeneinander geprüft: `led-strom`, `summe`, `teiler` |
| `Messplatz` | Klickbarer Schaltplan mit virtuellem Multimeter, Messaufgaben oder Fehlersuche |
| `KreisSim` | Mini-Simulator für den ersten Stromkreis |
| `Steckbrett` | Trainer: Welche Löcher sind verbunden? |
| `Multimeter` | Trainer: Messart und Buchse wählen |
| `Prellen` | Oszilloskop-Animation eines prellenden Tasters |
| `Seriell` | Gerätenachweis über Web Serial (Chrome/Edge), mit manueller Eingabe als Ersatz. Noch nicht mit echter Hardware getestet |

### Regeln für jede Mission

1. Gliederung: Einstieg → Theorie-Karten → Simulator/Übung → realer Aufbau → Prüfung → typische Fehler → Vertiefung.
2. Der Einstieg beginnt mit einem Problem an der Klingel, nie mit einer Definition.
3. Höchstens vier Theorie-Karten. Eine neue Idee pro Mission.
4. Mindestens eine Prüfung mit echtem Bezug (Messwert, Gerätenachweis), nicht nur ein Quiz.
5. Falsche Quizantworten sind echte Fehlvorstellungen.
6. Jeder Zahlenwert ist belegt (Datenblatt, Messung) oder als vorläufig markiert.
7. Der Weg ohne Hardware bleibt immer möglich.

## Offene Punkte vor dem Launch

- Die als vorläufig markierten Toleranzbänder (LED-Spannung, Strom des Summers) mit einem echten Aufbau nachmessen.
- Ein Impressum ergänzen, falls die Seite in Deutschland betrieben wird. Ob und in welcher Form es nötig ist, bitte selbst prüfen.
- In `src/config.ts` den Link zum Repository eintragen.

## Lizenz

- Code: MIT, siehe `LICENSE`.
- Inhalte (Texte der Missionen, Zeichnungen): CC BY-SA 4.0, siehe `LICENSE-INHALTE.md`.
- Schrift Atkinson Hyperlegible: SIL Open Font License, wird lokal mitgeliefert (keine Anfragen an Google).
