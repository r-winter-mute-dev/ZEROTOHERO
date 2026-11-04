# 0 to Hero

**Dein Einstieg in ein digitales Zeitalter.**

Diese Website wurde gebaut, um jedem Menschen die Möglichkeit zu geben, zu verstehen, was um ihn herum passiert: von der blinkenden LED bis zum selbst lernenden Roboterarm.

0 to Hero ist ein kostenloser, interaktiver Kurs für alle, die bei null anfangen. Du lernst nicht erst Theorie und baust dann irgendwann etwas. Du baust von Anfang an echte Geräte, und jedes Thema kommt genau dann, wenn du es brauchst. Die erste Kampagne ist eine smarte Türklingel.

Willkommen und viel Spaß beim Lernen.

**[Zum Kurs](https://r-winter-mute-dev.github.io/zerotohero/)**

## Kampagne 1: Die smarte Türklingel

Am Ende hängt an deiner Tür eine selbst gebaute Klingel: Ein ESP32-CAM mit 18650-Akku wacht auf Knopfdruck aus dem Tiefschlaf auf, macht ein Foto und schickt es an Telegram oder Discord. Sie läuft ohne Kabel.

Der Weg dorthin beginnt mit einer einzelnen LED.

## So funktioniert der Kurs

- **Neun Akte, neun Teile.** Jeder Akt baut ein Teil der Klingel: Knopf, Gehirn, Stimme, Auge, Herz und so weiter. Auf der Startseite füllt sich die Klingel Teil für Teil.
- **Kurze Missionen mit Prüfung.** Jede Mission endet mit einem Quiz, einer Rechenaufgabe, einer Übung im Simulator oder einem echten Messwert von deinem Aufbau.
- **Bosskämpfe.** Am Ende eines Akts wartet eine Fehlersuche: Eine Schaltung funktioniert nicht, und du findest mit dem Multimeter heraus, warum.
- **Auch ohne Hardware.** Wer die Bauteile noch nicht hat, kommt im Simulator weiter. Die echten Prüfungen lassen sich später nachholen.
- **Keine Anmeldung, kein Tracking.** Der Fortschritt bleibt in deinem Browser und lässt sich als Code sichern oder auf ein anderes Gerät übertragen.

## Lehrplan

| Akt | Teil | Du lernst |
| --- | --- | --- |
| 0 | Grundplatte | Sicherheit, Steckbrett, Multimeter |
| 1 | Knopf | Stromkreis, Spannung, Strom, Masse, Ohmsches Gesetz, Reihe und parallel, Spannungsteiler, Leistung, Taster. Ergebnis: Klingel v0, die summt |
| 2 | Gehirn | Mikrocontroller (ESP32), erste Programme, Eingänge, Entprellen |
| 3 | Stimme | WLAN, HTTP und APIs, Verschlüsselung, Nachricht an Discord oder Telegram |
| 4 | Auge | Kamera, Bilddaten, Foto verschicken, Transistor als Schalter |
| 5 | Herz | Li-Ion-Akku, Laden, Spannungswandler, Kondensatoren |
| 6 | Schlaf | Energiebudget, Strom messen, Tiefschlaf, Akkustand |
| 7 | Schalter | MOSFET, Selbsthalteschaltung, Ruhestrom fast null |
| 8 | Gehäuse | Löten, Lochraster, Gehäuse, Montage |

## Stand

| Bereich | Stand |
| --- | --- |
| Akt 0 bis Akt 4 | spielbar: Grundlagen, Mikrocontroller, WLAN, Nachricht und Foto an Discord oder Telegram |
| Akt 5 bis 8 | in Arbeit, die geplanten Missionen sind auf der Karte sichtbar |

## Was du brauchst

Für Akt 0 und 1 reicht ein kleines Einsteigerset: Steckbrett, Multimeter, Batteriehalter für 2 × AA, Widerstände, LEDs, ein Taster und ein aktiver Summer. Für Akt 2 kommen ein ESP32-DevKit und ein USB-Datenkabel dazu. Akt 3 braucht keine neuen Teile, nur ein 2,4-GHz-WLAN und Discord oder Telegram. Für Akt 4 kommen der ESP32-CAM und seine Programmierplatine dazu, der Akku folgt in Akt 5. Die komplette Liste mit Links zu Roboter-Bausatz.de steht auf der Seite „Einkaufsliste“, sortiert nach Akten.

## Mitmachen

Fehler gefunden, eine Erklärung unverständlich oder Lust, eine Mission zu schreiben? Beiträge sind willkommen. Wie eine Mission aufgebaut ist und welche Bausteine es gibt, steht in [CONTRIBUTING.md](CONTRIBUTING.md).

Am meisten hilft gerade:

- **Testen mit echter Hardware.** Einige erwartete Messbereiche sind vorläufig und brauchen Messwerte von echten Aufbauten.
- **Feedback von Einsteigern.** Wo bist du hängen geblieben?
- **Feedback zu Akt 2 bis 4 auf echter Hardware.** Die Sketches sind syntaktisch geprüft, aber noch nicht auf jedem Board getestet.
- **Missionen für Akt 5 und folgende.**

## Lokal starten

Voraussetzung: Node.js 22.23.4 oder neuer (empfohlen: Node 24).

```bash
npm install
npm run dev
```

Die Seite läuft dann unter http://localhost:4321.

## Technik

Statische Seite mit [Astro](https://astro.build) und MDX, ohne Server und ohne externe Dienste. Jede Mission ist eine MDX-Datei, die Prüfungen sind wiederverwendbare Komponenten. Die Seite wird über GitHub Actions automatisch auf GitHub Pages veröffentlicht.

Wer eine eigene Kopie betreiben will: Repository forken, unter **Settings → Pages** als Quelle **GitHub Actions** wählen, fertig. Der Workflow setzt den Pfad der Seite selbst.

## Lizenz

- Inhalte (Missionen, Texte, Zeichnungen): [CC BY-SA 4.0](LICENSE-INHALTE.md). Du darfst sie teilen und bearbeiten, solange du die Quelle nennst und unter derselben Lizenz weitergibst.
- Code: [MIT](LICENSE).
- Schrift: Atkinson Hyperlegible, SIL Open Font License.
