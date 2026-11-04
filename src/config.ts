// Zentrale Einstellungen. Hier Name und Texte der Seite ändern.
export const SEITE = {
  name: 'Klingelwerk',
  untertitel: 'Elektronik lernen, indem du eine smarte Türklingel baust',
  repo: '', // optional: Link zum GitHub-Repository, z. B. 'https://github.com/name/klingelwerk'
};

/** Pfad relativ zur Basis der Seite (wichtig für GitHub Pages unter /repo-name/). */
export function url(pfad = ''): string {
  const base = import.meta.env.BASE_URL.replace(/\/$/, '');
  const p = pfad.replace(/^\//, '');
  return `${base}/${p}`;
}
