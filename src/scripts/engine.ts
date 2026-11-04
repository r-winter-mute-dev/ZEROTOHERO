// Spiel-Engine: Fortschritt, Erfahrungspunkte, Freischaltung, Export/Import.
// Alles bleibt im Browser (localStorage). Kein Server, keine Cookies.
import { ALLE_MISSIONEN, KAMPAGNE, LEVEL, vorherige, type TeilKey } from '../data/kampagne';

const KEY = 'klingelwerk:v1';

export type Status = 'real' | 'simuliert';

export interface CheckInfo { id: string; hardware?: boolean; }

export interface Zustand {
  v: 1;
  ohneHardware: boolean;
  missionen: Record<string, { status: Status; xp: number; am: string }>;
  checks: Record<string, Record<string, { ok: true; ersterVersuch: boolean }>>;
  fehlversuche: Record<string, number>;
  abzeichen: string[];
}

const leer = (): Zustand => ({ v: 1, ohneHardware: false, missionen: {}, checks: {}, fehlversuche: {}, abzeichen: [] });

let cache: Zustand | null = null;

export function laden(): Zustand {
  if (cache) return cache;
  try {
    const raw = localStorage.getItem(KEY);
    cache = raw ? { ...leer(), ...JSON.parse(raw) } : leer();
  } catch {
    cache = leer();
  }
  return cache!;
}

function speichern(z: Zustand) {
  cache = z;
  try { localStorage.setItem(KEY, JSON.stringify(z)); } catch { /* Speicher nicht verfügbar: Fortschritt gilt nur bis zum Neuladen */ }
  window.dispatchEvent(new CustomEvent('kw:change', { detail: z }));
}

export function beiAenderung(cb: (z: Zustand) => void) {
  window.addEventListener('kw:change', (e) => cb((e as CustomEvent).detail));
}

// ---------- Missionen registrieren ----------

interface MissionMeta { id: string; xp: number; checks: CheckInfo[]; abzeichen?: string; }
const registry = new Map<string, MissionMeta>();

export function missionRegistrieren(meta: MissionMeta) {
  registry.set(meta.id, meta);
}

export function istFertig(id: string): boolean { return !!laden().missionen[id]; }

export function istFreigeschaltet(id: string): boolean {
  const v = vorherige(id);
  return !v || istFertig(v.id);
}

export function checkBestanden(mid: string, cid: string): boolean {
  return !!laden().checks[mid]?.[cid];
}

export function fehlversuch(mid: string, cid: string) {
  const z = laden();
  const k = `${mid}/${cid}`;
  z.fehlversuche[k] = (z.fehlversuche[k] || 0) + 1;
  speichern(z);
}

export function checkBestehen(mid: string, cid: string) {
  const z = laden();
  const k = `${mid}/${cid}`;
  z.checks[mid] = z.checks[mid] || {};
  if (!z.checks[mid][cid]) z.checks[mid][cid] = { ok: true, ersterVersuch: !z.fehlversuche[k] };
  speichern(z);
  auswerten(mid);
}

export function setOhneHardware(an: boolean) {
  const z = laden();
  z.ohneHardware = an;
  speichern(z);
  for (const id of registry.keys()) auswerten(id);
}

/** Prüft, ob eine Mission jetzt fertig ist, und vergibt Punkte. */
function auswerten(mid: string) {
  const meta = registry.get(mid);
  if (!meta) return;
  const z = laden();
  const ok = (c: CheckInfo) => !!z.checks[mid]?.[c.id];
  const alle = meta.checks.every(ok);
  const ohneHw = meta.checks.filter((c) => !c.hardware).every(ok) && z.ohneHardware;
  if (!alle && !ohneHw) return;

  const status: Status = alle ? 'real' : 'simuliert';
  const vorher = z.missionen[mid];
  if (vorher && (vorher.status === 'real' || status === 'simuliert')) return;

  const ersterVersuch = meta.checks.filter(ok).every((c) => z.checks[mid][c.id].ersterVersuch);
  const xp = meta.xp + (ersterVersuch ? 10 : 0);
  z.missionen[mid] = { status, xp: Math.max(xp, vorher?.xp || 0), am: new Date().toISOString() };
  if (meta.abzeichen && !z.abzeichen.includes(meta.abzeichen)) z.abzeichen.push(meta.abzeichen);
  speichern(z);
  window.dispatchEvent(new CustomEvent('kw:mission-fertig', { detail: { id: mid, status, xp, ersterVersuch, upgrade: !!vorher } }));
}

// ---------- Auswertung für Anzeige ----------

export function xpGesamt(z = laden()): number {
  return Object.values(z.missionen).reduce((s, x) => s + x.xp, 0);
}

export function level(xp: number) {
  let i = 0;
  LEVEL.forEach((l, j) => { if (xp >= l.ab) i = j; });
  const naechstes = LEVEL[i + 1];
  return { nr: i + 1, name: LEVEL[i].name, naechstesAb: naechstes?.ab, naechsterName: naechstes?.name };
}

export type TeilZustand = 'gesperrt' | 'aktiv' | 'simuliert' | 'real';

export function teilZustand(teil: TeilKey, z = laden()): { zustand: TeilZustand; fertig: number; gesamt: number } {
  const akt = KAMPAGNE.find((a) => a.teil === teil)!;
  const gesamt = akt.missionen.length;
  const erledigt = akt.missionen.filter((x) => z.missionen[x.id]);
  const fertig = erledigt.length;
  if (fertig === gesamt) {
    return { zustand: erledigt.every((x) => z.missionen[x.id].status === 'real') ? 'real' : 'simuliert', fertig, gesamt };
  }
  const offen = akt.missionen.some((x) => istFreigeschaltet(x.id));
  return { zustand: fertig > 0 || offen ? 'aktiv' : 'gesperrt', fertig, gesamt };
}

export function aktuelleMission() {
  return ALLE_MISSIONEN.find((x) => !istFertig(x.id)) ?? ALLE_MISSIONEN[ALLE_MISSIONEN.length - 1];
}

// ---------- Export / Import ----------

function pruefsumme(s: string): string {
  let h = 2166136261;
  for (let i = 0; i < s.length; i++) { h ^= s.charCodeAt(i); h = Math.imul(h, 16777619); }
  return (h >>> 0).toString(36);
}

export function exportCode(): string {
  const json = JSON.stringify(laden());
  const b64 = btoa(unescape(encodeURIComponent(json)));
  return `KW1.${b64}.${pruefsumme(b64)}`;
}

export function importCode(code: string): { ok: boolean; fehler?: string } {
  const teile = code.trim().split('.');
  if (teile.length !== 3 || teile[0] !== 'KW1') return { ok: false, fehler: 'Das ist kein Fortschritts-Code von dieser Seite. Er beginnt immer mit „KW1.“' };
  if (pruefsumme(teile[1]) !== teile[2]) return { ok: false, fehler: 'Der Code ist unvollständig. Kopiere ihn noch einmal komplett.' };
  try {
    const z = JSON.parse(decodeURIComponent(escape(atob(teile[1]))));
    if (z.v !== 1) throw new Error();
    speichern({ ...leer(), ...z });
    return { ok: true };
  } catch {
    return { ok: false, fehler: 'Der Code ließ sich nicht lesen. Kopiere ihn noch einmal komplett.' };
  }
}

export function zuruecksetzen() {
  speichern(leer());
  try { localStorage.removeItem('kw:schritte'); localStorage.removeItem('kw:checklisten'); } catch { /* egal */ }
}
