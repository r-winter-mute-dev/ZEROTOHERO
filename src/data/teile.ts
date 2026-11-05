// Einkaufsliste pro Akt. Links zu Roboter-Bausatz.de, recherchiert im November 2026.
// Verfügbarkeit und Preise ändern sich: vor dem Kauf im Shop prüfen.
// Teile ohne url werden erst nach der Prototyp-Messung festgelegt.

export interface Teil {
  teil: string;
  zweck: string;
  produkt?: string;
  url?: string;
  hinweis?: string;
  optional?: boolean;
}

const RB = 'https://www.roboter-bausatz.de';

export const TEILE: Record<number, Teil[]> = {
  0: [
    { teil: 'Steckbrett (830 Kontakte)', zweck: 'Alle Schaltungen ohne Löten', produkt: 'MB102 Breadboard 830 Kontakte', url: `${RB}/p/mb102-breadboard-830-kontakte`, hinweis: 'Ab Akt 2 lohnt das 3er-Set, weil das ESP32 sehr breit ist.' },
    { teil: 'Steckbrücken', zweck: 'Verbindungen auf dem Steckbrett', produkt: '65 Jumper Wire Kabel im Set', url: `${RB}/p/65-jumper-wire-kabel-im-set` },
    { teil: 'Digitales Multimeter', zweck: 'Fast jede Prüfung im Kurs', produkt: 'Kategorie Werkzeuge (Multimeter)', url: `${RB}/c/werkstattbedarf/werkzeuge/`, hinweis: 'Achte auf Gleichspannung, Widerstand, Durchgangsprüfer und einen mA-Bereich. Für Akt 6 ist ein µA-Bereich sehr hilfreich.' },
    { teil: 'Messleitungen mit Klemmen', zweck: 'Freihändig messen', produkt: 'Testclips Bananenstecker 1 m', url: `${RB}/p/testclips-bananenstecker-1m-fuer-multimeter`, optional: true },
  ],
  1: [
    { teil: 'Batteriehalter 2 × AA mit Schalter', zweck: 'Stromquelle für Akt 1', produkt: 'Batteriehalter 2x AA 3V mit An/Aus-Schalter', url: `${RB}/p/batteriehalter-2x-aa-3v-mit-an-aus-schalter`, hinweis: 'Mit dem Schalter klappt „erst aufbauen, dann Strom an“ ohne Batterien herauszunehmen. Zwei AA-Batterien gibt es im Supermarkt.' },
    { teil: 'Widerstandssortiment', zweck: '220 Ω, 1 kΩ, 2,2 kΩ und mehr', produkt: '300 Metallschicht-Widerstände, 30 Werte zu je 10 Stück', url: `${RB}/c/diy-elektronik/bauelemente/widerstaende/`, hinweis: 'Prüfe in der Werteliste, dass 220 Ω, 1 kΩ und 2,2 kΩ enthalten sind.' },
    { teil: 'LEDs 5 mm', zweck: 'Erster Stromkreis, später Anzeige', produkt: '100 x Leuchtdiode LED 5mm sortiert', url: `${RB}/p/100-x-leuchtdiode-led-5mm-sortiert-weiss-rot-gelb-blau-gruen` },
    { teil: 'Taster 6 × 6 mm', zweck: 'Der Klingelknopf', produkt: 'Drucktaster Sortiment 6x6, 70 Stück', url: `${RB}/p/microschalter-drucktaster-sortiment-6-6-70-stueck` },
    { teil: 'Aktiver Summer 3,3–5 V', zweck: 'Klingel v0', produkt: '10 Stück Aktiver Buzzer 3,3V-5V', url: `${RB}/p/10-stueck-aktiver-buzzer-3-3v-5v`, hinweis: 'Laut Shop unter 25 mA Stromaufnahme.' },
  ],
  2: [
    { teil: 'ESP32-DevKit', zweck: 'Das Gehirn zum Lernen', produkt: 'ESP32 NodeMcu Entwicklungsboard ESP-WROOM-32', url: `${RB}/p/esp32-nodemcu-entwicklungsboard-esp-wroom-32`, hinweis: 'Mit USB-Chip CP2102. Alternative mit USB-C und 38 Pins: NodeMcu ESP32 USB-C 38 Pin.' },
    { teil: 'USB-Datenkabel', zweck: 'Programmieren und serielle Ausgabe', produkt: 'Kein Link: vorhandenes Kabel nutzen', hinweis: 'Passend zum Board (Micro-USB oder USB-C). Es muss Daten übertragen, reine Ladekabel funktionieren nicht.' },
    { teil: 'Zweites Steckbrett', zweck: 'Platz neben dem breiten ESP32', produkt: '3x MB102 Breadboard 830 Kontakte', url: `${RB}/p/3x-mb102-breadboard-830-kontakte`, optional: true },
  ],
  4: [
    { teil: 'ESP32-CAM mit Kamera', zweck: 'Auge und Endprodukt', produkt: 'ESP32-Cam Board mit OV2640 Kamera', url: `${RB}/p/esp32-cam-wifi-bluetooth-board-mit-ov2640-kamera-kompatibel-mit-arduino` },
    { teil: 'Programmierplatine ESP32-CAM-MB', zweck: 'USB-Anschluss für das ESP32-CAM', produkt: 'ESP32-Cam-MB USB-zu-TTL Modul', url: `${RB}/p/esp32-cam-mb-usb-zu-ttl-modul-fuer-esp32-cam`, hinweis: 'Hat einen CH340-Chip. Eventuell brauchst du dafür einen eigenen Treiber.' },
  ],
  5: [
    { teil: 'Lötstation mit Zubehör', zweck: 'Kabel an die Module löten (Mission 5.3)', produkt: 'yourDroid regelbare Lötstation 48W ZD-99 mit Zubehör', url: `${RB}/p/yourdroid-regelbare-loetstation-48w-zd-99-mit-zubehoer`, hinweis: 'Laut Shop mit Ablage, Schwamm, 10 g Lötzinn und dritter Hand. Temperaturgeregelt, das ist für Einsteiger wichtig. Eine Schutzbrille brauchst du zusätzlich.' },
    { teil: 'Seitenschneider', zweck: 'Kabel kürzen und abisolieren', produkt: 'Kein Link: vorhandenes Werkzeug nutzen', hinweis: 'Eine kleine Elektronik-Zange reicht. Eine Abisolierzange macht es bequemer.' },
    { teil: '18650-Akku', zweck: 'Energiespeicher', produkt: 'BAK 18650 Lithium-Ionen-Akku 2900 mAh (flacher Pluspol)', url: `${RB}/p/bak-18650-lithium-ionen-akku-2900mah-3-6v-3-7v`, hinweis: 'Diese Zelle hat keine eigene Schutzschaltung. Sie darf nur zusammen mit einem Lademodul mit Schutzschaltung verwendet werden.' },
    { teil: 'Lademodul mit Schutzschaltung', zweck: 'Laden, Schutz vor Über- und Tiefentladung und Kurzschluss', produkt: 'TP4056 1A Lademodul USB-C (mit Schutzschaltung DW01)', url: `${RB}/p/tp4056-1a-lithium-ionen-lipo-lademodul-fuer-arduino-usb-c`, hinweis: 'Laut Shop im November 2026 nicht mehr verfügbar. Ersatz: ein TP4056-Modul mit DW01-Schutzschaltung, erkennbar an sechs Anschlüssen (IN+, IN−, B+, B−, OUT+, OUT−). Nicht verwechseln: Das „Step-Up Lademodul TP4056“ und das „TP4056 Solar Lademodul Micro USB“ haben laut Shop keine Schutzfunktionen.' },
    { teil: 'Batteriehalter für 1 × 18650', zweck: 'Akku sicher halten, mit Kabeln', produkt: 'Kategorie Batteriehalter', url: `${RB}/c/diy-elektronik/elektronische-bauelemente/batteriehalter/`, hinweis: 'Achte auf „1 × 18650“ und angelötete Kabel. Die Zelle nie direkt anlöten.' },
    { teil: 'Buck-Boost-Wandler 3,3 V', zweck: 'Stabile 3,3 V, egal wie voll der Akku ist', produkt: 'TPS63070 Buck-Boost Modul 3,3 V DC (Eingang 2–16 V, bis 2 A)', url: `${RB}/p/tps63070-buck-boost-modul-automatische-step-up-step-down-spannungsregelung-3-3-v-dc`, hinweis: 'Genau die 3,3-V-Variante nehmen. Die 5-V- und 9-V-Varianten würden den ESP32-CAM am 3V3-Pin zerstören.' },
    { teil: 'Elko 1000 µF', zweck: 'Puffer gegen Stromspitzen', produkt: '20x Elko Elektrolytkondensator 50V 1000uF 13x25mm Radial', url: `${RB}/p/20x-elko-elektrolytkondensator-50v-1000uf-13x25mm-radial`, hinweis: 'Ein Elko reicht, der Rest ist Vorrat. Elkos haben eine Richtung: Der Streifen auf dem Gehäuse markiert Minus.' },
    { teil: 'USB-Netzteil 5 V', zweck: 'Zum Laden', produkt: 'Kein Link: vorhandenes Handy-Netzteil nutzen', hinweis: 'Ein einfaches 5-V-Netzteil mit mindestens 1 A. Manche USB-C-Schnellladegeräte liefern dem Modul laut Händler keine 5 V, dann hilft ein USB-A-Netzteil mit passendem Kabel.' },
  ],
  6: [
    { teil: '2 Widerstände 10 kΩ', zweck: 'Spannungsteiler für den Akkustand (Mission 6.4)', produkt: 'Kein neuer Kauf: aus dem Widerstandssortiment aus Akt 1', hinweis: 'Farbringe Braun-Schwarz-Orange. Prüfe in der Werteliste deines Sortiments, dass 10 kΩ dabei ist.' },
    { teil: 'Taster', zweck: 'Der echte Klingelknopf an GPIO 13', produkt: 'Kein neuer Kauf: ein Taster aus Akt 1' },
    { teil: 'Multimeter mit mA-Bereich', zweck: 'Wachstrom und Ruhestrom messen', produkt: 'Kein neuer Kauf: dein Multimeter aus Akt 0', hinweis: 'Ein µA-Bereich macht Mission 6.5 genauer, ist aber kein Muss.' },
    { teil: 'Steckbrücke', zweck: 'Brücke über das Multimeter (Mission 6.5)', produkt: 'Kein neuer Kauf: aus dem Set aus Akt 0' },
  ],
  7: [
    { teil: 'Schaltmodul mit Selbsthaltung', zweck: 'Trennt die Klingel im Ruhezustand ganz vom Akku (Mission 7.2)', produkt: 'Pololu Mini Pushbutton Power Switch LV (Artikel 2808)', url: 'https://www.pololu.com/product/2808', hinweis: 'Bei Roboter-Bausatz.de nicht gefunden (Stand November 2026). Der Link führt zur Herstellerseite, dort stehen Händler in Deutschland. Wichtig ist die LV-Version: Nur sie arbeitet ab 2,2 V und damit an einer Li-Ion-Zelle. Die Stiftleisten lötest du selbst an (Mission 5.3).' },
    { teil: 'Widerstand 10 kΩ', zweck: 'Hält OFF beim Einschalten sicher auf LOW', produkt: 'Kein neuer Kauf: aus dem Widerstandssortiment aus Akt 1' },
  ],
  8: [
    { teil: 'Übungsplatine zum Löten', zweck: 'Löten lernen', produkt: 'Bausatz Übungsplatine zum Löten', url: `${RB}/p/bausatz-uebungsplatine-zum-loeten` },
    { teil: 'Lochrasterplatine', zweck: 'Die Schaltung dauerhaft verlöten (Mission 8.2)', produkt: 'Kein fester Link: Punktraster, Rastermaß 2,54 mm, etwa 7 × 9 cm', hinweis: 'Punktraster heißt: jedes Loch hat sein eigenes Kupferpad. Die Verbindungen ziehst du mit Draht und Lötzinn.' },
    { teil: 'Buchsenleisten 2,54 mm', zweck: 'Steckplatz für ESP32-CAM, Wandler, Lademodul und Schaltmodul', produkt: '20x Buchsenleiste 40-polig 2,54 mm (Artikel RBS18642)', url: `${RB}/datasheet/RBS18642.pdf`, hinweis: 'Der Link führt zum Datenblatt im Shop. Die Leisten lassen sich mit dem Seitenschneider auf die richtige Länge kürzen. So bleibt die ESP32-CAM zum Programmieren abnehmbar.' },
    { teil: 'Schaltdraht', zweck: 'Verbindungen auf der Lochrasterplatine', produkt: 'Kein fester Link: isolierter Draht, etwa 0,5 mm Durchmesser', hinweis: 'Für die Leitungen vom Akku und vom Wandler etwas dicker nehmen, sie tragen den größten Strom.' },
    { teil: 'Gehäuse', zweck: 'Schutz vor Regen und Staub (Mission 8.3)', produkt: 'Kein fester Link: Kunststoffgehäuse, Schutzart IP65, innen mindestens etwa 100 × 70 × 50 mm', hinweis: 'Kunststoff, kein Metall: Metall schirmt das WLAN ab. Jedes Loch, das du bohrst, verschlechtert die Schutzart, deshalb jedes Loch abdichten.' },
    { teil: 'Wasserdichter Drucktaster', zweck: 'Der Klingelknopf außen am Gehäuse', produkt: 'Kein fester Link: Drucktaster 12 oder 16 mm, tastend (nicht rastend), Schließer, mit Dichtung', hinweis: '„Tastend“ ist wichtig: Er springt nach dem Drücken zurück. Ein rastender Schalter bliebe gedrückt.' },
    { teil: 'Klare Scheibe für die Kamera', zweck: 'Kamerafenster', produkt: 'Kein fester Link: Acrylglas oder Polycarbonat, 1–2 mm, plus Silikon oder Heißkleber', hinweis: 'Reste aus dem Baumarkt reichen. Die Scheibe muss sauber und kratzfrei sein, sonst wird das Foto milchig.' },
  ],
};

// ---------- Kampagne 2: Fahrroboter ----------
// Vieles kennst du aus Kampagne 1. "Kein neuer Kauf" heißt: Das Teil liegt schon bei dir.
export const TEILE_ROBOTER: Record<number, Teil[]> = {
  0: [
    { teil: 'Werkzeug aus Kampagne 1', zweck: 'Steckbrett, Steckbrücken, Multimeter', produkt: 'Kein neuer Kauf: aus Kampagne 1, Akt 0' },
    { teil: 'ESP32-DevKit', zweck: 'Das Gehirn des Roboters', produkt: 'Kein neuer Kauf: dein Board aus Kampagne 1, Akt 2', hinweis: 'Falls du es nicht hast: ESP32 NodeMcu Entwicklungsboard ESP-WROOM-32 bei Roboter-Bausatz.de.' },
  ],
  1: [
    { teil: 'Fahrgestell mit zwei Getriebemotoren', zweck: 'Die Motoren für Akt 1, das Chassis für Akt 2', produkt: '2WD Smart Car Chassis Plattform (Artikel RBS11612)', url: `${RB}/p/2wd-smart-car-chassis-plattform-fuer-arduino-roboter`, hinweis: 'Laut Shop mit zwei Getriebemotoren, Rädern, Stützrad und Encoderscheiben. Prüfe die Nennspannung der Motoren auf der Produktseite. Bausätze dieser Art sind für etwa 3 bis 6 V gedacht. Die Motoren haben oft noch keine Kabel: Dann lötest du zwei Litzen an (Kampagne 1, Mission 5.3).' },
    { teil: 'Motortreiber DRV8833', zweck: 'H-Brücke für zwei Motoren', produkt: '2-Kanal DRV8833 DC-Motor-Treiber-Modul 3–10 V, 1,5 A (Artikel RBS12954)', url: `${RB}/p/2-kanal-drv8833-dc-motor-treiber-modul-3v-10v-1.5a-h-bruecke`, hinweis: 'Alternative im Shop: DRV8833-Modul als TB6612-Ersatz (RBS15374). Der DRV8833 arbeitet laut Datenblatt ab 2,7 V und passt damit zu einer einzelnen Li-Ion-Zelle.' },
    { teil: 'Batteriehalter 2 × AA mit Schalter', zweck: 'Getrennte Motorversorgung in Akt 1', produkt: 'Kein neuer Kauf: aus Kampagne 1, Akt 1', hinweis: 'Zwei frische AA-Batterien. In Akt 2 wechselst du auf den 18650-Akku.' },
  ],
  2: [
    { teil: 'Akku, Lademodul, Buck-Boost 3,3 V, Elko 1000 µF', zweck: 'Eine Versorgung für Motoren und ESP32', produkt: 'Kein neuer Kauf: aus Kampagne 1, Akt 5', hinweis: 'Achtung: Den Akku dann nicht in der Klingel und im Roboter gleichzeitig verwenden. Ein zweiter Satz ist bequemer.' },
    { teil: '2 Widerstände 10 kΩ', zweck: 'Spannungsteiler: Der ESP32 misst die Motorspannung (Mission 2.2)', produkt: 'Kein neuer Kauf: aus dem Widerstandssortiment (Kampagne 1, Akt 1)' },
    { teil: 'Ein/Aus-Schalter', zweck: 'Trennt den Akku von Motoren und Elektronik', produkt: 'Kein fester Link: Schiebe- oder Kippschalter für mindestens 2 A', hinweis: 'Der Schalter muss den Anlaufstrom beider Motoren tragen. Kleine Steckbrett-Schiebeschalter sind oft nur für wenige 100 mA gedacht, ein Blick auf die Angabe im Shop lohnt sich.' },
    { teil: 'Klebepads, Kabelbinder, Schrauben', zweck: 'Steckbrett, Akkuhalter und Treiber auf dem Fahrgestell befestigen', produkt: 'Kein fester Link: doppelseitiges Klebeband oder Klettband, kleine Kabelbinder', hinweis: 'Das Steckbrett hat auf der Unterseite oft schon eine Klebefläche.' },
  ],
};
