# Mitmachen bei 0 to Hero

Danke, dass du helfen willst. Hier steht, wie das Projekt aufgebaut ist und wie du eine Mission schreibst.

## Fehler melden

Öffne ein Issue und beschreib, auf welcher Mission du warst und was nicht gestimmt hat. Bei Messwerten hilft es, wenn du dazuschreibst, welche Bauteile und welches Multimeter du benutzt hast.

## Aufbau des Projekts

```
src/
  config.ts              Name der Seite, Link zum Repository
  data/kampagne.ts       Alle Akte und Missionen (IDs, Titel, XP, Level)
  missionen/*.mdx        Ausgeschriebene Missionen, eine Datei pro Mission
  components/            Prüfungen und interaktive Bausteine
  scripts/engine.ts      Fortschritt, Punkte, Freischaltung, Export/Import
  scripts/station.ts     Gemeinsame Logik aller Prüfungen
  pages/                 Startseite, Missionsseiten, Fortschritt, Über
firmware/                Firmware für Akt 2 und folgende (in Arbeit)
hardware/                Schaltpläne, Lochraster-Layouts, Gehäuse (in Arbeit)
```

## Eine Mission schreiben

1. In `src/data/kampagne.ts` bei der Mission `bereit: true` setzen. Die **ID nie ändern**, der gespeicherte Fortschritt der Lernenden hängt daran.
2. Die Datei `src/missionen/<id>.mdx` anlegen. Als Vorlage eignet sich `m-1-1.mdx`.
3. Im Frontmatter alle Prüfungen unter `checks` auflisten. `hardware: true` heißt: Diese Prüfung braucht echte Bauteile und wird im Modus „ohne Hardware“ übersprungen.
4. Jede Prüfungskomponente bekommt eine `id`, die genau so in `checks` steht.
5. Unter `spick` die Formeln und Werte eintragen, die man in dieser Mission braucht. Sie stehen neben jeder Aufgabe, damit niemand sich etwas merken muss. In Bosskämpfen keine Lösungen hineinschreiben.

```mdx
---
id: m-2-1
abzeichen: Erstes Leben
checks:
  - id: quiz
  - id: geraet
    hardware: true
spick:
  - t: Baudrate
    v: 115200
---
import Quiz from '../components/Quiz.astro';
import Seriell from '../components/Seriell.astro';

Text der Mission …

<Quiz id="quiz" fragen={[{ frage: '…', richtig: '…', falsch: ['…', '…'] }]} />
<Seriell id="geraet" titel="Gerätenachweis" muster="KW-CHECK:m-2-1:blink-ok" />
```

## Bausteine

| Komponente | Zweck |
| --- | --- |
| `Karte` | Theorie-Karte (höchstens etwa 120 Wörter) |
| `Merke` | Der eine Satz, der hängen bleiben soll. Höchstens einer pro Abschnitt |
| `Checkliste` | Abhakliste für Aufbauschritte, der Stand wird im Browser gemerkt |
| `Hinweis` | Kasten für Sicherheit, Hintergrund oder vorläufige Werte |
| `Quiz` | Multiple Choice, alle Antworten müssen stimmen |
| `Zahl` | Ein Messwert wird gegen ein Toleranzband geprüft, mit Hilfetexten je nach Abweichung |
| `Bestaetigung` | Abhaken, zum Beispiel „Meine LED leuchtet“ |
| `Rechnen` | Rechenaufgaben mit Zufallswerten: `vorwiderstand`, `strom`, `reihe`, `parallel`, `teiler`, `leistung` |
| `Protokoll` | Mehrere echte Messwerte werden gegeneinander geprüft: `led-strom`, `summe`, `teiler` |
| `Messplatz` | Klickbarer Schaltplan mit virtuellem Multimeter, für Messaufgaben oder Fehlersuche |
| `KreisSim` | Mini-Simulator für den ersten Stromkreis |
| `Steckbrett` | Trainer: Welche Löcher sind verbunden? |
| `Multimeter` | Trainer: Messart und Buchse wählen |
| `Prellen` | Oszilloskop-Animation eines prellenden Tasters |
| `Seriell` | Gerätenachweis über Web Serial (Chrome/Edge), mit manueller Eingabe als Ersatz. Noch nicht mit echter Hardware getestet |
| `Lueckentext` | Code-Puzzle: Jede Lücke `___` im Code wird ein Eingabefeld. Den Code in `src/data/puzzles.ts` ablegen, weil MDX die Einrückung in mehrzeiligen Attributen entfernt |
| `Logdiagnose` | Fehlersuche anhand von Logausgaben, zeigt zufällige Fälle nacheinander |

## Regeln für jede Mission

1. Gliederung: Einstieg → Theorie-Karten → Übung oder Simulator → realer Aufbau → Prüfung → typische Fehler → Vertiefung.
2. Der Einstieg beginnt mit einem Problem an der Klingel, nie mit einer Definition.
3. Höchstens vier Theorie-Karten. Eine neue Idee pro Mission.
4. Mindestens eine Prüfung mit echtem Bezug (Messwert, Gerätenachweis), nicht nur ein Quiz.
5. Falsche Quizantworten sind echte Fehlvorstellungen, keine Scherzantworten.
6. Jeder Zahlenwert ist belegt (Datenblatt, Messung) oder als vorläufig markiert.
7. Der Weg ohne Hardware bleibt immer möglich.
8. Sprache: kurze Sätze, Du-Form, Fachbegriffe beim ersten Auftreten erklären.
9. Kleine Schritte: Die Seite zeigt jede Zwischenüberschrift und jede Theorie-Karte als eigenen Bildschirm. Eine Übung steht auf demselben Bildschirm wie die Erklärung davor. Lieber kurz erklären und gleich fragen, als lange erklären und am Ende viel abfragen (Vorlage: `m-0-2.mdx`).
10. Keine Zeitlimits, keine Serien, die man verlieren kann, keine blinkenden Belohnungen. Fortschritt soll sichtbar sein, nicht Druck machen.

## Firmware (ab Akt 2)

Jede Prüfungs-Firmware gibt über die serielle Schnittstelle (115200 Baud) eine Zeile im Format `KW-CHECK:<missions-id>:<ergebnis>` aus. Die Komponente `Seriell` liest diese Zeile im Browser. Zugangsdaten (WLAN, Webhook-URL, Bot-Token) gehören in eine Datei `secrets.h`, die per `.gitignore` ausgeschlossen ist.

## Lokal testen

```bash
npm install
npm run dev      # Entwicklungsserver auf http://localhost:4321
npm run build    # baut die Seite nach dist/
npm run preview  # zeigt den Build lokal an
```

## Lizenz von Beiträgen

Mit einem Beitrag stimmst du zu, dass Inhalte unter CC BY-SA 4.0 und Code unter MIT veröffentlicht werden.
