#!/usr/bin/env python3
"""Erzeugt firmware/akt-*/*/wurzeln.h aus dem Mozilla-Zertifikatsspeicher des Systems.

Aufruf (Linux, Paket ca-certificates):  python3 firmware/werkzeuge/wurzeln_erzeugen.py
Die Liste WURZELN deckt die Stellen ab, die die Klingel erreicht:
api.telegram.org (Go Daddy) und discord.com (Cloudflare, wechselt zwischen mehreren Stellen).
"""
import glob, os, subprocess, datetime

QUELLE = '/etc/ssl/certs'
WURZELN = [
    ('ISRG_Root_X1', "Let's Encrypt (RSA)"),
    ('ISRG_Root_X2', "Let's Encrypt (ECC)"),
    ('GTS_Root_R1', 'Google Trust Services (RSA)'),
    ('GTS_Root_R4', 'Google Trust Services (ECC)'),
    ('SSL.com_Root_Certification_Authority_RSA', 'SSL.com (RSA)'),
    ('SSL.com_Root_Certification_Authority_ECC', 'SSL.com (ECC)'),
    ('USERTrust_RSA_Certification_Authority', 'Sectigo'),
    ('DigiCert_Global_Root_G2', 'DigiCert'),
    ('Go_Daddy_Root_Certificate_Authority_-_G2', 'Go Daddy (Telegram)'),
]

def info(pfad):
    aus = subprocess.run(['openssl', 'x509', '-in', pfad, '-noout', '-enddate', '-fingerprint', '-sha256'],
                         capture_output=True, text=True, check=True).stdout.splitlines()
    ende = aus[0].split('=', 1)[1]
    fp = aus[1].split('=', 1)[1]
    return ende, fp

def main():
    hier = os.path.dirname(os.path.abspath(__file__))
    ziele = sorted(glob.glob(os.path.join(hier, '..', 'akt-*', '*', 'wurzeln.h')))
    ziele.append(os.path.join(hier, '..', '..', 'public', 'firmware', 'wurzeln.h'))  # Download auf der Seite
    kopf = ['// 0 to Hero – Wurzelzertifikate für HTTPS (automatisch erzeugt, nicht von Hand ändern)',
            '// Erzeugt am %s aus dem Mozilla-Zertifikatsspeicher (Paket ca-certificates).' % datetime.date.today().isoformat(),
            '// Neu erzeugen: python3 firmware/werkzeuge/wurzeln_erzeugen.py',
            '//',
            '// Enthalten (Name, gültig bis, SHA-256-Fingerabdruck):']
    pems = []
    for datei, wofuer in WURZELN:
        pfad = os.path.join(QUELLE, datei + '.pem')
        ende, fp = info(pfad)
        kopf.append('//   %s – %s – bis %s' % (datei.replace('_', ' '), wofuer, ende))
        kopf.append('//     %s' % fp)
        pems.append(open(pfad).read().strip())
    text = '\n'.join(kopf) + '\n\n#pragma once\n\n// Mehrere Zertifikate hintereinander: Der ESP32 vertraut jedem davon.\nconst char WURZELN[] = R"PEM(\n' + '\n'.join(pems) + '\n)PEM";\n'
    if not ziele:
        print('Keine Ziel-Dateien gefunden. Lege zuerst leere wurzeln.h in den Sketch-Ordnern an.')
    for z in ziele:
        open(z, 'w').write(text)
        print('geschrieben:', os.path.relpath(z))

if __name__ == '__main__':
    main()
