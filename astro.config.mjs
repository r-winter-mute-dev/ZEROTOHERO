import { defineConfig } from 'astro/config';
import mdx from '@astrojs/mdx';

// SITE und BASE_PATH setzt der GitHub-Actions-Workflow automatisch
// (aus actions/configure-pages). Lokal läuft die Seite unter "/".
export default defineConfig({
  site: process.env.SITE || 'http://localhost:4321',
  base: process.env.BASE_PATH || '/',
  trailingSlash: 'ignore',
  integrations: [mdx()],
  markdown: {
    shikiConfig: { theme: 'catppuccin-mocha' },
  },
});
