// Anzeige-Einstellungen. Getrennt vom Lernfortschritt gespeichert, weil sie nichts mit dem Spielstand zu tun haben.
export interface Einstellungen {
  schrift: 'normal' | 'gross' | 'sehr-gross';
  abstand: 'normal' | 'weit';
  bewegung: 'system' | 'reduziert';
  fokus: boolean;
  schritte: boolean;
}

const KEY = 'kw:einstellungen';
export const STANDARD: Einstellungen = { schrift: 'normal', abstand: 'normal', bewegung: 'system', fokus: false, schritte: true };

export function einstellungen(): Einstellungen {
  try {
    const roh = localStorage.getItem(KEY);
    return roh ? { ...STANDARD, ...JSON.parse(roh) } : { ...STANDARD };
  } catch {
    return { ...STANDARD };
  }
}

export function anwenden(e = einstellungen()) {
  const d = document.documentElement.dataset;
  d.schrift = e.schrift;
  d.abstand = e.abstand;
  d.bewegung = e.bewegung;
  d.fokus = e.fokus ? 'an' : 'aus';
  d.schritte = e.schritte ? 'an' : 'aus';
}

export function setzen(teil: Partial<Einstellungen>) {
  const e = { ...einstellungen(), ...teil };
  try { localStorage.setItem(KEY, JSON.stringify(e)); } catch { /* nur für diese Sitzung */ }
  anwenden(e);
  window.dispatchEvent(new CustomEvent('kw:einstellungen', { detail: e }));
}

export function wenigBewegung(): boolean {
  return einstellungen().bewegung === 'reduziert' || matchMedia('(prefers-reduced-motion: reduce)').matches;
}

// Zuletzt geöffneter Schritt pro Mission, für den Wiedereinstieg.
const SKEY = 'kw:schritte';
export function letzterSchritt(mid: string): number {
  try { return JSON.parse(localStorage.getItem(SKEY) ?? '{}')[mid] ?? 0; } catch { return 0; }
}
export function schrittMerken(mid: string, i: number) {
  try {
    const alle = JSON.parse(localStorage.getItem(SKEY) ?? '{}');
    alle[mid] = i;
    localStorage.setItem(SKEY, JSON.stringify(alle));
  } catch { /* egal */ }
}
