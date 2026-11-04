// 0 to Hero – Vorlage für deine Zugangsdaten
// 1. Diese Datei kopieren und die Kopie "secrets.h" nennen (im selben Ordner).
// 2. In secrets.h deine echten Daten eintragen.
// 3. secrets.h nie teilen und nie hochladen. Die .gitignore des Projekts schließt sie aus.
#pragma once

#define WLAN_NAME      "DEIN_WLAN"
#define WLAN_PASSWORT  "DEIN_PASSWORT"

// Wohin soll die Klingel schreiben? 1 = Discord, 2 = Telegram
#define DIENST 1

// Discord: Kanal-Einstellungen → Integrationen → Webhooks → Webhook-URL kopieren
#define DISCORD_WEBHOOK "https://discord.com/api/webhooks/DEINE_ID/DEIN_TOKEN"

// Telegram: Token von @BotFather, Chat-ID über getUpdates (siehe Mission 3.5)
#define TELEGRAM_TOKEN   "123456789:DEIN_TOKEN"
#define TELEGRAM_CHAT_ID "DEINE_CHAT_ID"
