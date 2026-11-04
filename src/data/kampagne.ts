// Die komplette Kampagne. Reihenfolge = Lernreihenfolge.
// IDs ändern sich nie (gespeicherter Fortschritt hängt daran).
// "bereit: true" heißt: Es gibt eine ausgeschriebene Datei in src/missionen/<id>.mdx.

export type TeilKey =
  | 'grundplatte' | 'knopf' | 'gehirn' | 'stimme' | 'auge'
  | 'herz' | 'schlaf' | 'schalter' | 'gehaeuse';

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

export const ALLE_MISSIONEN: Mission[] = KAMPAGNE.flatMap((a) => a.missionen);

export function aktVon(missionId: string): Akt | undefined {
  return KAMPAGNE.find((a) => a.missionen.some((x) => x.id === missionId));
}

export function vorherige(missionId: string): Mission | undefined {
  const i = ALLE_MISSIONEN.findIndex((x) => x.id === missionId);
  return i > 0 ? ALLE_MISSIONEN[i - 1] : undefined;
}

export function naechste(missionId: string): Mission | undefined {
  const i = ALLE_MISSIONEN.findIndex((x) => x.id === missionId);
  return i >= 0 && i < ALLE_MISSIONEN.length - 1 ? ALLE_MISSIONEN[i + 1] : undefined;
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
