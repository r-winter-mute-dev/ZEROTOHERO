// Gemeinsame Logik für alle Prüfungs-Stationen auf einer Missionsseite.
import { beiAenderung, checkBestanden, checkBestehen, checkUebersprungen, checkUeberspringen, fehlversuch, istFreigeschaltet, laden } from './engine';
import { vorherige } from '../data/kampagne';

export interface Station {
  el: HTMLElement;
  mid: string;
  cid: string;
  hardware: boolean;
  bestanden(): boolean;
  bestehen(text?: string): void;
  fehl(text: string): void;
  melden(text: string, art?: 'gut' | 'fehler' | 'warnung' | ''): void;
  darf(): boolean;
  /** Meldet die Sperre und gibt true zurück, wenn die Mission noch gesperrt ist. */
  gesperrt(): boolean;
}

export function station(el: HTMLElement): Station {
  const mid = el.closest<HTMLElement>('[data-mission]')?.dataset.mission ?? '';
  const cid = el.dataset.kwCheck ?? '';
  const meldungEl = el.querySelector<HTMLElement>('[data-kw-meldung]');
  const s: Station = {
    el, mid, cid,
    hardware: el.dataset.hardware === 'true',
    bestanden: () => checkBestanden(mid, cid),
    melden(text, art = '') {
      if (!meldungEl) return;
      meldungEl.textContent = text;
      meldungEl.className = `meldung ${art}`;
    },
    bestehen(text = 'Bestanden.') {
      if (s.gesperrt()) return;
      checkBestehen(mid, cid);
      s.melden(text, 'gut');
      markieren(el);
    },
    fehl(text) {
      if (!s.bestanden()) fehlversuch(mid, cid);
      s.melden(text, 'fehler');
    },
    darf: () => !mid || istFreigeschaltet(mid),
    gesperrt() {
      if (s.darf()) return false;
      const v = vorherige(mid);
      s.melden(v
        ? `Noch gesperrt: Schließe zuerst Mission ${v.nr} „${v.titel}“ ab. Hast du dort Prüfungen mit Hardware übersprungen, setze dort den Haken „ohne Hardware“.`
        : 'Diese Mission ist noch gesperrt.', 'warnung');
      return true;
    },
  };
  return s;
}

export function markieren(el: HTMLElement) {
  const s = station(el);
  const status = el.querySelector<HTMLElement>('[data-kw-status]');
  const ok = s.bestanden();
  const geskippt = ok && checkUebersprungen(s.mid, s.cid);
  const skip = !ok && s.hardware && laden().ohneHardware;
  const knopf = el.querySelector<HTMLButtonElement>('[data-kw-skip]');
  if (knopf) {
    knopf.hidden = ok || !s.darf();
    if (knopf.hidden) { knopf.dataset.sicher = ''; knopf.textContent = 'Hängt hier etwas? Prüfung überspringen'; }
  }
  el.classList.toggle('bestanden', ok);
  el.classList.toggle('uebersprungen', skip);
  el.classList.toggle('gesperrt', !ok && !s.darf());
  if (status) {
    status.textContent = geskippt ? 'Übersprungen'
      : ok ? 'Bestanden'
      : skip ? 'Übersprungen (ohne Hardware)'
      : !s.darf() ? 'Gesperrt'
      : (status.dataset.art ?? 'Prüfung');
  }
}

export function alleMarkieren() {
  document.querySelectorAll<HTMLElement>('[data-kw-check]').forEach(markieren);
}

if (typeof window !== 'undefined') {
  if (document.readyState === 'loading') document.addEventListener('DOMContentLoaded', alleMarkieren);
  else queueMicrotask(alleMarkieren);
  beiAenderung(alleMarkieren);
  // Überspringen in zwei Schritten, damit niemand aus Versehen tippt.
  document.addEventListener('click', (e) => {
    const knopf = (e.target as HTMLElement).closest<HTMLButtonElement>('[data-kw-skip]');
    if (!knopf) return;
    const el = knopf.closest<HTMLElement>('[data-kw-check]')!;
    const s = station(el);
    if (s.gesperrt()) return;
    if (knopf.dataset.sicher !== '1') {
      knopf.dataset.sicher = '1';
      knopf.textContent = 'Wirklich überspringen? Noch einmal tippen.';
      return;
    }
    checkUeberspringen(s.mid, s.cid);
    s.melden('Übersprungen. Die Mission zählt dann als „im Simulator geschafft“. Du kannst die Prüfung jederzeit nachholen.', 'warnung');
  });
}

export function mischen<T>(a: T[]): T[] {
  const b = [...a];
  for (let i = b.length - 1; i > 0; i--) { const j = Math.floor(Math.random() * (i + 1)); [b[i], b[j]] = [b[j], b[i]]; }
  return b;
}

/** Deutsche Zahleneingabe lesen: "1,85" und "1.85" sind beide erlaubt. */
export function zahl(s: string): number {
  return parseFloat(String(s).replace(/\s/g, '').replace(',', '.'));
}

export function fmt(n: number, stellen = 2): string {
  return n.toLocaleString('de-DE', { maximumFractionDigits: stellen });
}
