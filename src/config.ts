// Zentrale Einstellungen. Hier Name und Texte der Seite ändern.
export const SEITE = {
  name: '0 to Hero',
  untertitel: 'Dein Einstieg in ein digitales Zeitalter',
  mission: 'Diese Website wurde gebaut, um jedem Menschen die Möglichkeit zu geben, zu verstehen, was um ihn herum passiert: von der blinkenden LED bis zum selbst lernenden Roboterarm.',
  repo: 'https://github.com/r-winter-mute-dev/ZEROTOHERO', // Link zum GitHub-Repository
};

/** Pfad relativ zur Basis der Seite (wichtig für GitHub Pages unter /repo-name/). */
export function url(pfad = ''): string {
  const base = import.meta.env.BASE_URL.replace(/\/$/, '');
  const p = pfad.replace(/^\//, '');
  return `${base}/${p}`;
}
