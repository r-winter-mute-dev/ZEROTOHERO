// Modell eines Linienfolgers für den Regler-Trainer (Einheiten: mm, s).
// Vereinfachungen: zwei analoge Sensoren 60 mm vor der Achse, Gaußförmiges Sensorbild der Linie,
// Motoren reagieren mit 0,12 s Verzögerung, rechter Motor 6 % schwächer, Regeltakt 20 ms.
// Modellwerte zum Verstehen, keine Messwerte eines echten Roboters.
export const B = 140, VOR = 60, SENS = 14, SIG = 9, TAU = 0.12, DT = 0.005, TC = 0.02, VERLOREN = 45;
export const STRECKE: [number, number][] = [
  [350, 0], [(Math.PI / 2) * 300, 1 / 300], [250, 0], [((Math.PI * 2) / 3) * 200, -1 / 200], [300, 0], [(Math.PI / 2) * 250, 1 / 250], [350, 0],
];
export const LAENGE = STRECKE.reduce((a, x) => a + x[0], 0);
function kappa(s: number) { let a = 0; for (const [l, k] of STRECKE) { if (s < a + l) return k; a += l; } return 0; }
const g = (x: number) => Math.exp(-((x / SIG) ** 2));

export interface Einstellung { modus: 'zwei' | 'p' | 'pid'; v: number; U?: number; kp?: number; ki?: number; kd?: number; }
export interface Ergebnis { fertig: boolean; rms: number; maxY: number; t: number; s: number; bahn: [number, number, number][]; }

export function simuliere(p: Einstellung): Ergebnis {
  let s = 0, y = 4, psi = 0, w = 0, wsoll = 0, t = 0, tc = 0, lastE = 0, integ = 0, letzteRichtung = 1;
  let rnd = 3; const zufall = () => { rnd = (rnd * 16807) % 2147483647; return rnd / 2147483647 - 0.5; };
  let sum2 = 0, n = 0, maxY = 0; const bahn: [number, number, number][] = [];
  const kp = p.kp ?? 0, ki = p.ki ?? 0, kd = p.kd ?? 0, U = p.U ?? 0;
  while (s < LAENGE && t < 25) {
    if (t >= tc) {
      tc += TC;
      const ys = y - VOR * Math.sin(psi);
      const bl = g(ys - SENS) + zufall() * 0.04, br = g(ys + SENS) + zufall() * 0.04;
      let e: number;
      if (bl < 0.06 && br < 0.06) e = letzteRichtung; else { e = bl - br; letzteRichtung = Math.sign(e) || letzteRichtung; }
      let u: number;
      if (p.modus === 'zwei') u = Math.abs(e) < 0.05 ? 0 : Math.sign(e) * U;
      else {
        integ = Math.max(-2, Math.min(2, integ + e * TC));
        const d = (e - lastE) / TC;
        u = kp * e + (p.modus === 'pid' ? ki * integ + kd * d : 0);
      }
      lastE = e;
      u = Math.max(-1, Math.min(1, u));
      wsoll = (2 * u * p.v) / B - (0.06 * p.v) / B;
    }
    w += ((wsoll - w) * DT) / TAU;
    y -= p.v * Math.sin(psi) * DT;
    psi += (w - p.v * kappa(s)) * DT;
    s += p.v * Math.cos(psi) * DT;
    t += DT;
    sum2 += y * y; n++; maxY = Math.max(maxY, Math.abs(y));
    if (n % 6 === 0) bahn.push([s, y, t]);
    if (Math.abs(y) > VERLOREN) break;
  }
  bahn.push([s, y, t]);
  return { fertig: s >= LAENGE, rms: Math.sqrt(sum2 / Math.max(1, n)), maxY, t, s, bahn };
}

// Mittellinie der Strecke als Weltkoordinaten (x nach rechts, y nach oben), alle 5 mm.
export function streckenPunkte(): [number, number, number][] {
  const p: [number, number, number][] = [];
  let x = 0, y = 0, th = Math.PI / 2;
  for (let s = 0; s <= LAENGE; s += 5) { p.push([x, y, th]); th += kappa(s) * 5; x += Math.cos(th) * 5; y += Math.sin(th) * 5; }
  return p;
}
