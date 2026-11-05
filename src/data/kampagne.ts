// Die komplette Kampagne. Reihenfolge = Lernreihenfolge.
// IDs ändern sich nie (gespeicherter Fortschritt hängt daran).
// "bereit: true" heißt: Es gibt eine ausgeschriebene Datei in src/missionen/<id>.mdx.

export type TeilKey =
  | 'grundplatte' | 'knopf' | 'gehirn' | 'stimme' | 'auge'
  | 'herz' | 'schlaf' | 'schalter' | 'gehaeuse'
  // Kampagne 2
  | 'werkbank' | 'motor' | 'fahrwerk' | 'augen' | 'regler' | 'tacho' | 'funk';

export interface Mission {
  id: string;
  nr: string;          // Anzeige, z. B. "1.1" oder "Boss 1"
  titel: string;
  kurz: string;        // ein Satz: was man tut
  theorie: string;     // Theorie genau hier
  pruefung: string;    // wie geprüft wird
  xp: number;
  boss?: boolean;
  bereit?: boolean;
}

export interface Akt {
  nr: number;
  titel: string;
  teil: TeilKey;
  teilName: string;
  ziel: string;
  missionen: Mission[];
}

const m = (
  id: string, nr: string, titel: string, kurz: string, theorie: string, pruefung: string,
  extra: Partial<Mission> = {},
): Mission => ({ id, nr, titel, kurz, theorie, pruefung, xp: extra.boss ? 100 : 50, ...extra });

export const KAMPAGNE: Akt[] = [
  {
    nr: 0, titel: 'Basislager', teil: 'grundplatte', teilName: 'Grundplatte',
    ziel: 'Werkzeug kennen und sicher arbeiten.',
    missionen: [
      m('m-0-1', '0.1', 'Das Ziel', 'Die fertige Klingel kennenlernen und verstehen, wie der Kurs funktioniert.', 'Überblick über alle Teile der Klingel', 'Bestätigen', { bereit: true }),
      m('m-0-2', '0.2', 'Sicherheit', 'Die Regeln lernen, die dich und dein Material schützen.', 'Akkus, Löten, Netzspannung, Kamera und Datenschutz', 'Sicherheitsquiz, alles richtig', { bereit: true }),
      m('m-0-3', '0.3', 'Das Steckbrett', 'Herausfinden, welche Löcher miteinander verbunden sind.', 'Kontaktreihen, Mittelrinne, Versorgungsschienen', 'Steckbrett-Trainer', { bereit: true }),
      m('m-0-4', '0.4', 'Das Multimeter', 'Das wichtigste Messgerät richtig einstellen und eine Batterie messen.', 'Messarten, Buchsen, Messbereiche', 'Multimeter-Trainer und echter Messwert', { bereit: true }),
    ],
  },
  {
    nr: 1, titel: 'Der Knopf', teil: 'knopf', teilName: 'Knopf',
    ziel: 'Stromkreise verstehen und eine erste Klingel bauen, die summt.',
    missionen: [
      m('m-1-1', '1.1', 'Erster Stromkreis', 'Eine LED zum Leuchten bringen.', 'Spannung, Strom, geschlossener Kreis, Polarität', 'Simulator, Quiz, Messwert', { bereit: true }),
      m('m-1-2', '1.2', 'Masse und Bezugspunkt', 'Spannungen zwischen verschiedenen Punkten messen.', 'Spannung als Differenz, GND als Bezug, Maschenregel', 'Messplatz und Messprotokoll', { bereit: true }),
      m('m-1-3', '1.3', 'Ohmsches Gesetz', 'Den Vorwiderstand einer LED selbst berechnen und den Strom messen.', 'U = R × I, Farbcode, Normreihe E12', 'Rechenaufgaben und Strommessung', { bereit: true }),
      m('m-1-4', '1.4', 'Reihe und parallel', 'Widerstände kombinieren und nachmessen.', 'Reihen- und Parallelschaltung', 'Rechenaufgaben und Widerstandsmessung', { bereit: true }),
      m('m-1-5', '1.5', 'Spannungsteiler', 'Mit zwei Widerständen eine kleinere Spannung erzeugen.', 'Teilerformel, belasteter Teiler', 'Rechenaufgaben und Messprotokoll', { bereit: true }),
      m('m-1-6', '1.6', 'Leistung und Wärme', 'Berechnen, ob ein Bauteil zu heiß wird.', 'P = U × I, Nennleistung', 'Rechenaufgaben und Quiz', { bereit: true }),
      m('m-1-7', '1.7', 'Der Taster', 'Einen Taster einbauen und sein Prellen verstehen.', 'Schließer und Öffner, Durchgangsprüfung, Prellen', 'Quiz und Aufbau', { bereit: true }),
      m('m-1-8', '1.8', 'Klingel v0', 'Taster und Summer: die erste echte Klingel.', 'Aktiver und passiver Summer', 'Aufbau und Strommessung', { bereit: true }),
      m('m-1-boss', 'Boss 1', 'Die LED bleibt dunkel', 'Drei defekte Schaltungen mit dem Multimeter durchmessen und den Fehler finden.', 'Systematische Fehlersuche', 'Drei Fehler finden', { boss: true, bereit: true }),
    ],
  },
  {
    nr: 2, titel: 'Das Gehirn', teil: 'gehirn', teilName: 'Gehirn (ESP32)',
    ziel: 'Einen Mikrocontroller programmieren und den Klingelknopf auslesen.',
    missionen: [
      m('m-2-1', '2.1', 'Erstes Leben', 'Arduino IDE einrichten und das ESP32 blinken lassen.', 'Was ein Mikrocontroller ist, Firmware, Upload', 'Gerätenachweis über Web Serial', { bereit: true }),
      m('m-2-2', '2.2', 'Hallo, Welt', 'Das ESP32 schickt Text an deinen Computer.', 'Serielle Schnittstelle, Baudrate', 'Gerätenachweis mit eigenem Text', { bereit: true }),
      m('m-2-3', '2.3', 'Programmieren I', 'Blinkgeschwindigkeit per Variable und Bedingung steuern.', 'Variablen, Datentypen, if/else', 'Code-Puzzle und Gerätenachweis', { bereit: true }),
      m('m-2-4', '2.4', 'Programmieren II', 'Ein Klingelmuster mit Schleife und eigener Funktion.', 'Schleifen, Funktionen, setup() und loop()', 'Code-Puzzle und Gerätenachweis', { bereit: true }),
      m('m-2-5', '2.5', 'Ausgang', 'Eine externe LED am GPIO schalten.', '3,3-V-Logik, Strombegrenzung pro Pin', 'Rechenaufgabe, Messwert, Gerätenachweis', { bereit: true }),
      m('m-2-6', '2.6', 'Eingang', 'Den Klingelknopf am GPIO auslesen.', 'Schwebender Eingang, Pull-up', 'Quiz, Messwert, Gerätenachweis', { bereit: true }),
      m('m-2-7', '2.7', 'Entprellen', 'Das Prellen in Software herausfiltern.', 'Zeitfenster, millis(), Zustände', 'Code-Puzzle und Gerätenachweis', { bereit: true }),
      m('m-2-8', '2.8', 'Interrupts', 'Den Taster per Interrupt erkennen, auch wenn das Programm beschäftigt ist.', 'Polling gegen Interrupt, volatile', 'Quiz und Gerätenachweis', { bereit: true }),
      m('m-2-db', '2.9', 'Das Datenblatt', 'Das Datenblatt deines ESP32 finden und die wichtigen Grenzwerte ablesen.', 'Aufbau eines Datenblatts, Grenzwerte, Betriebsbereich, min/typ/max, Fußnoten', 'Übung mit echten Werten und Nachschlagen', { bereit: true }),
      m('m-2-boss', 'Boss 2', 'Das Board startet nicht', 'Upload- und Boot-Probleme anhand von Logausgaben eingrenzen.', 'Boot-Pins, Brownout, Upload-Modus, Baudrate', 'Fehlerszenarien', { boss: true, bereit: true }),
    ],
  },
  {
    nr: 3, titel: 'Die Stimme', teil: 'stimme', teilName: 'Stimme (WLAN)',
    ziel: 'Die Klingel meldet sich im Chat.',
    missionen: [
      m('m-3-1', '3.1', 'Ins WLAN', 'Den ESP32 mit dem WLAN verbinden.', 'WLAN, IP, DHCP, 2,4 GHz', 'Gerätenachweis mit IP-Adresse', { bereit: true }),
      m('m-3-2', '3.2', 'Was eine API ist', 'HTTP-Anfragen im Baukasten zusammenstecken.', 'Request/Response, Statuscodes, JSON', 'HTTP-Baukasten und Quiz', { bereit: true }),
      m('m-3-3', '3.3', 'Verschlüsselt', 'Eine HTTPS-Anfrage vom ESP32 senden.', 'TLS, Zertifikate, Uhrzeit per NTP', 'Gerätenachweis mit zwei Statuscodes', { bereit: true }),
      m('m-3-4', '3.4', 'Geheimnisse', 'Zugangsdaten aus dem Code herausholen.', 'Zugangsdaten wie Passwörter behandeln', 'Code-Review und Gerätenachweis', { bereit: true }),
      m('m-3-5', '3.5', 'Es klingelt', 'Knopfdruck schickt eine Nachricht nach Discord oder Telegram.', 'Webhook und Bot-API', 'Gerätenachweis und Nachricht auf dem Handy', { bereit: true }),
      m('m-3-boss', 'Boss 3', 'Die Nachricht kommt nie an', 'Netzwerkfehler anhand von Logs finden.', '5-GHz-Netz, TLS-Fehler, Statuscodes', 'Fehlerszenarien', { boss: true, bereit: true }),
    ],
  },
  {
    nr: 4, titel: 'Das Auge', teil: 'auge', teilName: 'Auge (Kamera)',
    ziel: 'Die Klingel schickt ein Foto.',
    missionen: [
      m('m-4-1', '4.1', 'Wechsel auf ESP32-CAM', 'Programmierplatine aufstecken und ein Beispiel flashen.', 'Unterschiede zum DevKit, PSRAM, feste Pins', 'Gerätenachweis mit PSRAM-Größe', { bereit: true }),
      m('m-4-2', '4.2', 'Pixel und Bytes', 'Auflösung und Qualität gegen Dateigröße abwägen.', 'Pixel, Bytes, JPEG, Speicher', 'Rechenaufgaben', { bereit: true }),
      m('m-4-3', '4.3', 'Erstes Foto', 'Ein Foto aufnehmen und seine Größe ausgeben.', 'Bildpuffer, Belichtung', 'Messwert und Gerätenachweis', { bereit: true }),
      m('m-4-4', '4.4', 'Foto verschicken', 'Das Foto per HTTPS in den Chat schicken.', 'Datei-Upload (multipart/form-data)', 'Puzzle, Gerätenachweis und Foto im Chat', { bereit: true }),
      m('m-4-5', '4.5', 'Der Blitz', 'Die Blitz-LED steuern.', 'Transistor als Schalter, PWM', 'Schaltplan-Trainer und Gerätenachweis', { bereit: true }),
      m('m-4-6', '4.6', 'Klingel v1', 'Reset-Taste, Foto, Chat: alles zusammen am USB-Kabel.', 'Zustandsautomat, Zeitgrenzen', 'Messwert Dauer und Foto im Chat', { bereit: true }),
      m('m-4-boss', 'Boss 4', 'Reset beim Einschalten der Kamera', 'Stromspitzen als Ursache erkennen.', 'Brownout, Kamerafehler, Speicher', 'Fehlerszenarien', { boss: true, bereit: true }),
    ],
  },
  {
    nr: 5, titel: 'Das Herz', teil: 'herz', teilName: 'Herz (Akku)',
    ziel: 'Die Klingel läuft ohne Kabel.',
    missionen: [
      m('m-5-1', '5.1', 'Energie gegen Leistung', 'Mit mAh und Wh rechnen und herausfinden, wohin die Energie geht.', 'Energie, Leistung, Kapazität, Wartestrom', 'Rechenaufgaben', { bereit: true }),
      m('m-5-2', '5.2', 'Die 18650-Zelle', 'Eine Li-Ion-Zelle prüfen und messen.', 'Spannungsfenster, Schutzschaltung, Brandrisiko, Notfall', 'Sicherheitsquiz und Messwert', { bereit: true }),
      m('m-5-loeten', '5.3', 'Erste Lötstellen', 'Kabel an Lademodul und Wandler löten.', 'Lötkolben, Lötzinn, gute Lötstelle, Sicherheit', 'Durchgangsprüfung', { bereit: true }),
      m('m-5-3', '5.4', 'Laden', 'Das Lademodul anschließen und beobachten.', 'Konstantstrom/Konstantspannung, Ladeende', 'Quiz und Messwert', { bereit: true }),
      m('m-5-4', '5.5', 'Spannung wandeln', 'Den Buck-Boost-Wandler einbauen und die Klingel am Akku betreiben.', 'Linear- gegen Schaltregler, Dropout', 'Simulator, Messwert und Foto vom Akku', { bereit: true }),
      m('m-5-5', '5.6', 'Puffern', 'Mit einem Kondensator Stromspitzen abfangen.', 'Kondensator als Energiespeicher, Leitungswiderstand', 'Belastungstest ohne Brownout', { bereit: true }),
      m('m-5-boss', 'Boss 5', 'Läuft nur mit vollem Akku', 'Fehler in der Akkuversorgung anhand von Messungen finden.', 'Dropout, Schutzschaltung, Polung, Pinbelegung', 'Fehlerszenarien', { boss: true, bereit: true }),
    ],
  },
  {
    nr: 6, titel: 'Der Schlaf', teil: 'schlaf', teilName: 'Schlaf',
    ziel: 'Die Klingel spart Energie.',
    missionen: [
      m('m-6-1', '6.1', 'Das Budget', 'Ausrechnen, wohin die Energie der Klingel geht und wie lange der Akku hält.', 'Wach und Schlaf getrennt rechnen, Reserve', 'Rechenaufgaben', { bereit: true }),
      m('m-6-2', '6.2', 'Strom messen', 'Den Strom der wachen Klingel messen, ohne sie zu stören.', 'Messstelle, Bürdenspannung, Messbereiche', 'Rechenaufgaben und Messwert', { bereit: true }),
      m('m-6-3', '6.3', 'Tiefschlaf', 'Klingel v2: schlafen, per Klingelknopf aufwachen, Foto senden.', 'Deep Sleep, RTC-Pins, Pins festhalten', 'Nachweis aus dem Chat', { bereit: true }),
      m('m-6-4', '6.4', 'Akkustand melden', 'Den Akkustand messen, kalibrieren und mit dem Foto mitschicken.', 'Spannungsteiler, ADC2 und WLAN, Kalibrierung', 'Messprotokoll und Nachweis aus dem Chat', { bereit: true }),
      m('m-6-5', '6.5', 'Die Enttäuschung', 'Den Ruhestrom der schlafenden Klingel messen.', 'Warum das Board im Schlaf Milliampere zieht', 'Messwert und Rechenaufgaben', { bereit: true }),
      m('m-6-boss', 'Boss 6', 'Akku nach wenigen Tagen leer', 'Akkufresser anhand von Messungen und Chatprotokollen finden.', 'Schwebende Pins, Dauerverbraucher, Wachbleiben', 'Fehlerszenarien', { boss: true, bereit: true }),
    ],
  },
  {
    nr: 7, titel: 'Der Schalter', teil: 'schalter', teilName: 'Schalter (MOSFET)',
    ziel: 'Ruhestrom fast null (Gold-Stufe).',
    missionen: [
      m('m-7-1', '7.1', 'MOSFET', 'Den Schalter ohne bewegliche Teile verstehen.', 'N- und P-Kanal, U_GS, Schwelle, R_DS(on)', 'Trainer', { bereit: true }),
      m('m-7-2', '7.2', 'Selbsthaltung', 'Der Knopf schaltet ein, das Schaltmodul hält, die Klingel schaltet sich selbst aus.', 'Selbsthalteschaltung, Pololu-Schaltmodul, was hinter den Schalter gehört', 'Trainer und Einbau', { bereit: true }),
      m('m-7-3', '7.3', 'Firmware', 'Klingel v4: messen, Foto, senden, aus.', 'Ablauf, Ausschalten, Zeitgrenze, Fehlerfälle', 'Nachweis aus dem Chat', { bereit: true }),
      m('m-7-4', '7.4', 'Null-Ampere', 'Den Ruhestrom erneut messen und mit Mission 6.5 vergleichen.', 'Kleine Ströme messen, Selbstentladung', 'Ruhestrom mindestens 100-mal kleiner als in 6.5', { bereit: true }),
    ],
  },
  {
    nr: 8, titel: 'Das Gehäuse', teil: 'gehaeuse', teilName: 'Gehäuse',
    ziel: 'Die Klingel hängt an der Tür.',
    missionen: [
      m('m-8-1', '8.1', 'Löten', 'Eine Übungsplatine löten.', 'Gute und schlechte Lötstellen, Reihenfolge', 'Lötstellen-Quiz und Übungsplatine', { bereit: true }),
      m('m-8-2', '8.2', 'Lochraster', 'Die Schaltung vom Steckbrett auf eine Lochrasterplatine übertragen.', 'Netze, Netzliste, Buchsenleisten', 'Netzliste und Durchgangsprüfung', { bereit: true }),
      m('m-8-3', '8.3', 'Gehäuse', 'Das Gehäuse mit Kamerafenster und Klingelknopf bauen.', 'IP65, Kondenswasser, Temperaturgrenzen des Akkus', 'Quiz und Einbau', { bereit: true }),
      m('m-8-4', '8.4', 'Draußen', 'Am Montageort prüfen, ob das WLAN reicht.', 'RSSI, Dämpfung, Abhilfe', 'Nachweis: Signalstärke vom Montageort', { bereit: true }),
      m('m-8-5', '8.5', 'Inbetriebnahme', 'Die Klingel datenschutzfreundlich einrichten und montieren.', 'Klingelkamera und Datenschutz, Wartung', 'Abschlussprüfung', { bereit: true }),
    ],
  },
];

// ---------- Kampagne 2: Der Fahrroboter ----------
export const FAHRROBOTER: Akt[] = [
  {
    nr: 0, titel: 'Startrampe', teil: 'werkbank', teilName: 'Werkbank',
    ziel: 'Wissen, was gebaut wird, und sicher mit bewegten Teilen arbeiten.',
    missionen: [
      m('r-0-1', '0.1', 'Das Ziel', 'Den fertigen Fahrroboter kennenlernen und prüfen, was du schon kannst.', 'Überblick, Teile, Voraussetzungen aus Kampagne 1', 'Bestätigen', { bereit: true }),
      m('r-0-2', '0.2', 'Sicherheit', 'Die Regeln für Motoren, Räder und Akku lernen.', 'Bewegte Teile, Blockieren, Akku, Aufbocken', 'Sicherheitsquiz', { bereit: true }),
    ],
  },
  {
    nr: 1, titel: 'Der Motor', teil: 'motor', teilName: 'Motor',
    ziel: 'Einen Gleichstrommotor verstehen und vom ESP32 aus steuern.',
    missionen: [
      m('r-1-1', '1.1', 'Wie ein Motor dreht', 'Einen Getriebemotor am Akku laufen lassen und seinen Strom messen.', 'Magnet, Spule, Kommutator, Getriebe', 'Messwert Leerlaufstrom', { bereit: true }),
      m('r-1-2', '1.2', 'Der Anlaufstrom', 'Ausrechnen und messen, wie viel Strom ein blockierter Motor zieht.', 'Wicklungswiderstand, Gegenspannung, Blockierstrom', 'Rechenaufgaben und Messwert', { bereit: true }),
      m('r-1-3', '1.3', 'Die H-Brücke', 'Verstehen, wie vier Schalter die Drehrichtung umkehren.', 'H-Brücke, Kurzschluss-Verbot, Freilauf und Bremsen', 'Trainer', { bereit: true }),
      m('r-1-db', '1.4', 'Das Datenblatt des Treibers', 'Im Datenblatt des DRV8833 die Werte finden, die der Roboter braucht.', 'Chip oder Modul, Typvarianten, Betriebsbereich, Fußnoten', 'Übung mit echten Werten und Nachschlagen', { bereit: true }),
      m('r-1-4', '1.5', 'Motor am ESP32', 'Den Motor über den DRV8833 vorwärts, rückwärts und langsam drehen lassen.', 'DRV8833, Wahrheitstabelle, PWM', 'Gerätenachweis', { bereit: true }),
      m('r-1-boss', 'Boss 1', 'Der Motor macht Ärger', 'Typische Motorfehler anhand von Messungen finden.', 'Polung, Versorgung, Sleep-Pin, Blockieren', 'Fehlerszenarien', { boss: true, bereit: true }),
    ],
  },
  {
    nr: 2, titel: 'Fahren', teil: 'fahrwerk', teilName: 'Fahrwerk',
    ziel: 'Der Roboter fährt geradeaus und um die Kurve.',
    missionen: [
      m('r-2-1', '2.1', 'Das Fahrgestell', 'Chassis, Motoren und Räder zusammenbauen.', 'Mechanik, Kabelführung', 'Bestätigen', { bereit: true }),
      m('r-2-2', '2.2', 'Eine Versorgung für alles', 'Motoren und ESP32 aus einem Akku versorgen, ohne Neustarts.', 'Stromspitzen, Puffer, gemeinsame Masse', 'Belastungstest', { bereit: true }),
      m('r-2-3', '2.3', 'Sanft anfahren', 'Mit einer Rampe anfahren und bremsen.', 'Rampen, Anlaufwert, Prozent statt PWM', 'Gerätenachweis', { bereit: true }),
      m('r-2-4', '2.4', 'Lenken', 'Mit zwei unterschiedlich schnellen Rädern Kurven fahren.', 'Differenzialantrieb, Kurvenradius', 'Rechenaufgaben und Fahrprobe', { bereit: true }),
      m('r-2-boss', 'Boss 2', 'Fährt im Kreis', 'Fehler in Antrieb und Versorgung finden.', 'Ungleiche Motoren, Brownout, Verdrahtung', 'Fehlerszenarien', { boss: true, bereit: true }),
    ],
  },
  {
    nr: 3, titel: 'Sehen', teil: 'augen', teilName: 'Augen (Sensoren)',
    ziel: 'Der Roboter erkennt eine Linie und Hindernisse.',
    missionen: [
      m('r-3-1', '3.1', 'Licht, das zurückkommt', 'Einen Infrarot-Reflexsensor lesen.', 'Reflexion, Fototransistor, analog und digital', 'Messwert'),
      m('r-3-2', '3.2', 'Die Linie erkennen', 'Zwei Sensoren kalibrieren und die Linie finden.', 'Schwellwert, Kalibrierung, Hysterese', 'Gerätenachweis'),
      m('r-3-3', '3.3', 'Ultraschall', 'Den Abstand zu einem Hindernis messen.', 'Laufzeit, Schallgeschwindigkeit, Temperatur', 'Rechenaufgaben und Messwert'),
      m('r-3-4', '3.4', 'Rechtzeitig halten', 'Vor einem Hindernis sicher anhalten.', 'Bremsweg, Reaktionszeit', 'Fahrprobe'),
      m('r-3-boss', 'Boss 3', 'Sieht Gespenster', 'Fehlmessungen der Sensoren erklären.', 'Sonnenlicht, Winkel, weiche Oberflächen', 'Fehlerszenarien', { boss: true }),
    ],
  },
  {
    nr: 4, titel: 'Regeln', teil: 'regler', teilName: 'Regler',
    ziel: 'Der Roboter folgt einer Linie ruhig und schnell.',
    missionen: [
      m('r-4-1', '4.1', 'Ein und aus', 'Mit einem Zweipunktregler der Linie folgen.', 'Regelkreis, Sollwert, Istwert', 'Simulator'),
      m('r-4-2', '4.2', 'Der P-Regler', 'Proportional zur Abweichung lenken.', 'Regelabweichung, Verstärkung', 'Simulator'),
      m('r-4-3', '4.3', 'PID', 'Integral und Differenzial verstehen.', 'Bleibende Abweichung, Dämpfung', 'Simulator'),
      m('r-4-4', '4.4', 'Einstellen', 'Den Regler am echten Roboter einstellen.', 'Schrittweises Tuning, Messen statt Raten', 'Rundenzeit'),
      m('r-4-boss', 'Boss 4', 'Zittert auf der Linie', 'Reglerfehler an Messkurven erkennen.', 'Schwingen, Überschwingen, zu träge', 'Fehlerszenarien', { boss: true }),
    ],
  },
  {
    nr: 5, titel: 'Messen', teil: 'tacho', teilName: 'Tacho (Encoder)',
    ziel: 'Der Roboter weiß, wie schnell und wie weit er fährt.',
    missionen: [
      m('r-5-1', '5.1', 'Der Encoder', 'Radumdrehungen mit einer Gabellichtschranke zählen.', 'Gabellichtschranke, Interrupts, Prellen', 'Gerätenachweis'),
      m('r-5-2', '5.2', 'Geschwindigkeit', 'Aus Impulsen die Geschwindigkeit berechnen.', 'Impulse pro Umdrehung, Radumfang', 'Rechenaufgaben und Messwert'),
      m('r-5-3', '5.3', 'Wo bin ich?', 'Strecke und Drehwinkel aus den Rädern berechnen.', 'Odometrie, Schlupf', 'Fahrprobe'),
      m('r-5-4', '5.4', 'Gleich schnell', 'Beide Räder auf dieselbe Drehzahl regeln.', 'Drehzahlregelung, zwei Regelkreise', 'Geradeausfahrt'),
    ],
  },
  {
    nr: 6, titel: 'Fernsteuern', teil: 'funk', teilName: 'Funk',
    ziel: 'Der Roboter fährt per Handy und hält sicher an, wenn die Verbindung abreißt.',
    missionen: [
      m('r-6-1', '6.1', 'Ein Webserver', 'Auf dem ESP32 eine kleine Webseite bereitstellen.', 'HTTP-Server, IP-Adresse, lokales Netz', 'Gerätenachweis'),
      m('r-6-2', '6.2', 'Die Fernbedienung', 'Den Roboter vom Handy aus steuern.', 'WebSocket, Latenz', 'Fahrprobe'),
      m('r-6-3', '6.3', 'Notaus', 'Bei Funkverlust sofort anhalten.', 'Watchdog, sicherer Zustand', 'Fehlertest'),
      m('r-6-boss', 'Finale', 'Der Parcours', 'Linie, Hindernis und Fernsteuerung in einem Lauf.', 'Alles aus Kampagne 2', 'Parcours', { boss: true }),
    ],
  },
];

export interface Kampagne {
  id: string;
  nr: number;
  titel: string;
  kurz: string;
  voraussetzung?: string;
  ueberschrift: string;   // große Überschrift auf der Kampagnenseite
  einstieg: string;       // ein Absatz darunter
  akte: Akt[];
}

export const KAMPAGNEN: Kampagne[] = [
  { id: 'klingel', nr: 1, titel: 'Die smarte Türklingel', kurz: 'Vom ersten Stromkreis bis zur Akku-Klingel, die ein Foto schickt.',
    ueberschrift: 'Bau eine Türklingel, die dir ein Foto aufs Handy schickt.',
    einstieg: 'Du fängst bei null an: Was ist Spannung, wie leuchtet eine LED? Mission für Mission baust du ein Teil der Klingel und lernst genau die Elektronik, die du dafür brauchst. Am Ende wacht ein ESP32-CAM auf Knopfdruck auf, macht ein Foto und schickt es an Telegram oder Discord.',
    akte: KAMPAGNE },
  { id: 'roboter', nr: 2, titel: 'Der Fahrroboter', kurz: 'Motoren, Sensoren und Regelung: ein Roboter, der einer Linie folgt und per Handy fährt.',
    voraussetzung: 'Kampagne 1 bis einschließlich Akt 2 (ESP32 programmieren) und Akt 5 (Akku und Wandler), oder gleichwertiges Vorwissen.',
    ueberschrift: 'Bau einen Roboter, der selbst seinen Weg findet.',
    einstieg: 'Motoren, Sensoren und Regelung: Du lässt zwei Räder drehen, gibst dem Roboter Augen für Linie und Hindernis und bringst ihm bei, ruhig zu fahren statt zu zappeln. Am Ende folgt er einer Linie, hält vor der Wand und fährt per Handy.',
    akte: FAHRROBOTER },
];

// Kampagnen, die geplant, aber noch nicht begonnen sind. Erscheinen auf der Startseite als Vorschau.
export const GEPLANT = [
  { nr: 3, titel: 'Die Wetterstation', kurz: 'Sensoren, Messfehler und Daten über Wochen: eine Station, die misst, speichert und Diagramme zeigt.' },
  { nr: 4, titel: 'Der Roboterarm', kurz: 'Servos, Gelenke und Koordinaten: ein Arm, der Gegenstände greift und ablegt.' },
  { nr: 5, titel: 'Der lernende Arm', kurz: 'Der Arm lernt aus Versuchen: Grundlagen des maschinellen Lernens am eigenen Gerät.' },
];

export const ALLE_MISSIONEN: Mission[] = KAMPAGNEN.flatMap((k) => k.akte.flatMap((a) => a.missionen));
export const missionenVon = (k: Kampagne): Mission[] => k.akte.flatMap((a) => a.missionen);

export function kampagneVon(missionId: string): Kampagne {
  return KAMPAGNEN.find((k) => k.akte.some((a) => a.missionen.some((x) => x.id === missionId))) ?? KAMPAGNEN[0];
}

export function aktVon(missionId: string): Akt | undefined {
  return kampagneVon(missionId).akte.find((a) => a.missionen.some((x) => x.id === missionId));
}

// Freischaltung innerhalb einer Kampagne: Die erste Mission jeder Kampagne ist immer offen.
export function vorherige(missionId: string): Mission | undefined {
  const liste = missionenVon(kampagneVon(missionId));
  const i = liste.findIndex((x) => x.id === missionId);
  return i > 0 ? liste[i - 1] : undefined;
}

export function naechste(missionId: string): Mission | undefined {
  const liste = missionenVon(kampagneVon(missionId));
  const i = liste.findIndex((x) => x.id === missionId);
  return i >= 0 && i < liste.length - 1 ? liste[i + 1] : undefined;
}

export const LEVEL = [
  { ab: 0, name: 'Lehrling' },
  { ab: 300, name: 'Steckbrett-Pilot' },
  { ab: 900, name: 'Firmware-Schmied' },
  { ab: 1500, name: 'Funker' },
  { ab: 2100, name: 'Energiemeister' },
  { ab: 2700, name: 'Klingel-Ingenieur' },
  { ab: 3150, name: 'Hero' },               // alle Missionen geschafft
];
