#ifndef DASHBOARD_H
#define DASHBOARD_H

const char DASHBOARD_HTML[] PROGMEM = R"rawhtml(
<!DOCTYPE html>
<html lang="en">

<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1, maximum-scale=1">
  <meta name="color-scheme" content="dark">
  <title>Smart Street Lighting</title>
  <link href="https://fonts.googleapis.com/css2?family=Outfit:wght@300;400;600;700&display=swap" rel="stylesheet">
  <style>
    /* ── Design Tokens ───────────────────────────────────────────────────── */
    :root {
      --bg: #030509;
      --bg2: #0b111e;
      --card: rgba(14, 21, 35, 0.5);
      --card-border: rgba(0, 247, 255, 0.1);
      --card-border-hover: rgba(0, 247, 255, 0.3);
      --accent: #00f7ff;
      --accent2: #7b5cff;
      --accent3: #ff3366;
      --glow: 0 0 24px rgba(0, 247, 255, 0.25);
      --glow-sm: 0 0 12px rgba(0, 247, 255, 0.35);
      --text: #e2e8f0;
      --text-dim: #8b9bb4;
      --radius: 20px;
      --radius-sm: 12px;
      --transition: 0.3s cubic-bezier(0.25, 0.8, 0.25, 1);
    }

    /* ── Reset & Base ────────────────────────────────────────────────────── */
    *, *::before, *::after {
      box-sizing: border-box;
      margin: 0;
      padding: 0;
    }

    html {
      scroll-behavior: smooth;
    }

    body {
      background: var(--bg);
      color: var(--text);
      font-family: 'Outfit', system-ui, -apple-system, sans-serif;
      min-height: 100vh;
      overflow-x: hidden;
      -webkit-font-smoothing: antialiased;
    }

    /* ── Animated grid background ────────────────────────────────────────── */
    body::before {
      content: '';
      position: fixed;
      inset: -50px;
      background:
        radial-gradient(circle at 15% 50%, rgba(0, 247, 255, 0.05) 0%, transparent 50%),
        radial-gradient(circle at 85% 30%, rgba(123, 92, 255, 0.05) 0%, transparent 50%),
        linear-gradient(rgba(255, 255, 255, 0.02) 1px, transparent 1px),
        linear-gradient(90deg, rgba(255, 255, 255, 0.02) 1px, transparent 1px);
      background-size: 100% 100%, 100% 100%, 50px 50px, 50px 50px;
      pointer-events: none;
      z-index: 0;
      animation: drift 20s linear infinite;
    }
    @keyframes drift { 0% { transform: translate(0,0); } 100% { transform: translate(50px,50px); } }
    
    /* ── Entrance Animations ─────────────────────────────────────────────── */
    @keyframes fadeInUp {
      from { opacity: 0; transform: translateY(24px) scale(0.98); filter: blur(4px); }
      to   { opacity: 1; transform: translateY(0) scale(1); filter: blur(0); }
    }
    header, .stat-pill, .card, footer {
      animation: fadeInUp 0.7s cubic-bezier(0.2, 0.8, 0.2, 1) both;
      will-change: transform, opacity;
    }
    header { animation-delay: 0.05s; }
    .stat-pill:nth-child(1) { animation-delay: 0.10s; }
    .stat-pill:nth-child(2) { animation-delay: 0.15s; }
    .stat-pill:nth-child(3) { animation-delay: 0.20s; }
    .stat-pill:nth-child(4) { animation-delay: 0.25s; }
    .zone1-card   { animation-delay: 0.30s; }
    .zone2-card   { animation-delay: 0.35s; }
    .color-card   { animation-delay: 0.40s; }
    .control-card { animation-delay: 0.45s; }
    .vchart-card  { animation-delay: 0.50s; }
    .mchart-card  { animation-delay: 0.55s; }
    .heat-card    { animation-delay: 0.60s; }
    footer        { animation-delay: 0.65s; }

    /* ── Layout wrapper ──────────────────────────────────────────────────── */
    .app {
      position: relative;
      z-index: 1;
      max-width: 1400px;
      margin: 0 auto;
      padding: 24px;
    }
    @media (max-width: 768px) {
      .app { padding: 16px; }
    }

    /* ── Loading Screen ──────────────────────────────────────────────────── */
    #loadingScreen {
      position: fixed; inset: 0; z-index: 9999;
      background: var(--bg);
      display: flex; align-items: center; justify-content: center;
      transition: opacity 0.6s cubic-bezier(0.8, 0, 0.2, 1), visibility 0.6s;
    }
    .loader-content { display: flex; flex-direction: column; align-items: center; gap: 24px; }
    .blur-pulse {
      color: var(--accent);
      animation: pulse-ring 1.5s ease-in-out infinite;
    }
    .loading-text {
      font-size: 0.85rem; font-weight: 700; letter-spacing: 0.3em; color: var(--accent);
      animation: blink 1.5s infinite;
    }

    /* ── Header ──────────────────────────────────────────────────────────── */
    header {
      display: flex;
      align-items: center;
      justify-content: space-between;
      flex-wrap: wrap;
      gap: 16px;
      padding: 24px 28px;
      margin-bottom: 24px;
      background: var(--card);
      border: 1px solid var(--card-border);
      border-radius: var(--radius);
      backdrop-filter: blur(20px);
      -webkit-backdrop-filter: blur(20px);
      box-shadow: 0 8px 32px rgba(0,0,0,0.2);
    }
    @media (max-width: 600px) {
      header {
        position: sticky;
        top: 10px;
        z-index: 100;
        padding: 16px 20px;
      }
    }

    .header-left {
      display: flex;
      align-items: center;
      gap: 18px;
    }

    .logo-icon {
      width: 52px;
      height: 52px;
      border-radius: 50%;
      background: radial-gradient(circle, rgba(0, 247, 255, 0.15) 0%, transparent 80%);
      border: 2px solid rgba(0, 247, 255, 0.4);
      display: flex;
      align-items: center;
      justify-content: center;
      color: var(--accent);
      box-shadow: inset 0 0 10px rgba(0,247,255,0.2), var(--glow);
      animation: pulse-ring 4s ease-in-out infinite;
    }

    @keyframes pulse-ring {
      0%, 100% { box-shadow: inset 0 0 10px rgba(0,247,255,0.2), 0 0 15px rgba(0, 247, 255, 0.3); }
      50% { box-shadow: inset 0 0 15px rgba(0,247,255,0.4), 0 0 35px rgba(0, 247, 255, 0.6); border-color: rgba(0, 247, 255, 0.7); }
    }

    .header-title {
      line-height: 1.3;
    }

    .header-title h1 {
      font-size: clamp(1.2rem, 3.5vw, 1.6rem);
      font-weight: 700;
      color: #fff;
      text-shadow: 0 0 20px rgba(255,255,255,0.2);
    }

    .header-title p {
      font-size: 0.8rem;
      font-weight: 400;
      color: var(--text-dim);
      letter-spacing: 0.05em;
    }

    .header-right {
      display: flex;
      gap: 16px;
      align-items: center;
      flex-wrap: wrap;
    }

    #clock {
      font-size: 0.95rem;
      font-weight: 600;
      color: var(--text-dim);
      font-variant-numeric: tabular-nums;
      background: rgba(255,255,255,0.03);
      padding: 6px 14px;
      border-radius: 20px;
      border: 1px solid rgba(255,255,255,0.05);
    }

    /* ── Mode badge ──────────────────────────────────────────────────────── */
    .mode-badge {
      padding: 6px 18px;
      border-radius: 20px;
      font-size: 0.75rem;
      font-weight: 700;
      letter-spacing: 0.1em;
      text-transform: uppercase;
      border: 1px solid transparent;
      transition: var(--transition);
      display: flex;
      align-items: center;
      gap: 6px;
    }

    .mode-auto {
      background: rgba(0, 247, 255, 0.08);
      border-color: rgba(0, 247, 255, 0.3);
      color: var(--accent);
      box-shadow: 0 0 15px rgba(0,247,255,0.15);
    }

    .mode-override {
      background: rgba(255, 180, 50, 0.08);
      border-color: rgba(255, 180, 50, 0.3);
      color: #ffb432;
      box-shadow: 0 0 15px rgba(255,180,50,0.15);
    }

    .mode-off {
      background: rgba(255, 51, 102, 0.08);
      border-color: rgba(255, 51, 102, 0.3);
      color: var(--accent3);
      box-shadow: 0 0 15px rgba(255,51,102,0.15);
    }

    .mode-on-anim {
      background: rgba(34, 197, 94, 0.1);
      border-color: rgba(34, 197, 94, 0.5);
      color: #22c55e;
      box-shadow: 0 0 20px rgba(34, 197, 94, 0.3);
      animation: modeSlideIn 0.35s cubic-bezier(0.22, 1, 0.36, 1);
    }

    @keyframes modeSlideIn {
      from { opacity: 0; transform: translateX(-14px) scale(0.92); }
      to   { opacity: 1; transform: translateX(0) scale(1); }
    }

    .mode-party {
      background: linear-gradient(90deg, rgba(255,0,128,0.12), rgba(123,0,255,0.12), rgba(0,220,255,0.12));
      border-color: rgba(255, 80, 200, 0.5);
      color: #ff50c8;
      animation: partyBlink 0.5s infinite alternate;
    }

    @keyframes partyBlink {
      0%   { color: #ff50c8; border-color: rgba(255,80,200,0.7); box-shadow: 0 0 20px rgba(255,80,200,0.5); filter: hue-rotate(0deg); background: linear-gradient(90deg,rgba(255,0,128,0.15),rgba(123,0,255,0.12),rgba(0,220,255,0.08)); }
      100% { color: #50e8ff; border-color: rgba(80,232,255,0.7); box-shadow: 0 0 30px rgba(80,232,255,0.5); filter: hue-rotate(200deg); background: linear-gradient(90deg,rgba(0,220,255,0.15),rgba(123,0,255,0.12),rgba(255,0,128,0.08)); }
    }

    /* ── Status Bar ──────────────────────────────────────────────────────── */
    .status-bar {
      display: grid;
      grid-template-columns: repeat(12, 1fr);
      gap: 16px;
      margin-bottom: 24px;
    }

    /* 4 pills on top row, 3 pills on bottom row */
    .stat-pill:nth-child(-n+4) { grid-column: span 3; }
    .stat-pill:nth-child(n+5) { grid-column: span 4; }

    @media (max-width: 1100px) {
      .stat-pill:nth-child(n) { grid-column: span 6; }
      .stat-pill:nth-child(7) { grid-column: span 12; }
    }
    
    @media (max-width: 700px) {
      .stat-pill:nth-child(n) { grid-column: span 12; }
    }

    .stat-pill {
      min-width: 0;
      display: flex;
      align-items: center;
      gap: 14px;
      background: var(--card);
      border: 1px solid var(--card-border);
      border-radius: var(--radius-sm);
      padding: 16px 20px;
      backdrop-filter: blur(12px);
      -webkit-backdrop-filter: blur(12px);
      transition: all var(--transition);
      box-shadow: 0 4px 20px rgba(0,0,0,0.15);
    }

    .stat-pill:hover {
      border-color: var(--card-border-hover);
      transform: translateY(-2px);
      background: rgba(20, 28, 45, 0.6);
    }

    .stat-icon {
      display: flex;
      align-items: center;
      justify-content: center;
      width: 40px; height: 40px;
      border-radius: 10px;
      background: rgba(255,255,255,0.03);
      color: var(--text-dim);
    }
    
    .stat-pill:hover .stat-icon {
      color: var(--text);
      background: rgba(255,255,255,0.08);
    }

    .stat-info {
      line-height: 1.4;
    }

    .stat-label {
      font-size: 0.7rem;
      color: var(--text-dim);
      text-transform: uppercase;
      letter-spacing: 0.08em;
      font-weight: 600;
    }

    .stat-value {
      font-size: 1.2rem;
      font-weight: 700;
    }

    .stat-icon-inline {
      display: inline-flex;
      align-items: center;
      gap: 6px;
      font-size: 1.1rem;
      font-weight: 700;
    }
    .stat-icon-inline svg { flex-shrink: 0; }

    .txt-accent { color: var(--accent); }
    .txt-accent2 { color: var(--accent2); }
    .txt-accent3 { color: var(--accent3); }
    .txt-warn { color: #ffb432; }
    .txt-ok { color: #22c55e; }

    /* ── Motion indicator animation ──────────────────────────────────────── */
    .motion-dot {
      width: 10px;
      height: 10px;
      border-radius: 50%;
      background: rgba(255,255,255,0.1);
      display: inline-block;
      margin-right: 8px;
      transition: all var(--transition);
    }

    .motion-dot.active {
      background: #22c55e;
      box-shadow: 0 0 12px #22c55e;
      animation: blink 1s infinite;
    }

    @keyframes blink {
      50% { opacity: 0.5; box-shadow: 0 0 4px #22c55e; }
    }

    /* ── Main Grid ───────────────────────────────────────────────────────── */
    .main-grid {
      display: grid;
      grid-template-columns: 1fr 1fr;
      grid-template-areas:
        "zone1   zone2"
        "color   controls"
        "vchart  mchart"
        "heat    heat"
        "footer  footer";
      gap: 20px;
    }

    @media (max-width: 850px) {
      .main-grid {
        grid-template-columns: 1fr;
        grid-template-areas:
          "zone1"
          "zone2"
          "color"
          "controls"
          "vchart"
          "mchart"
          "heat"
          "footer";
      }
    }

    /* ── Card base ───────────────────────────────────────────────────────── */
    .card {
      background: var(--card);
      border: 1px solid var(--card-border);
      border-radius: var(--radius);
      padding: 24px;
      backdrop-filter: blur(16px);
      -webkit-backdrop-filter: blur(16px);
      transition: all var(--transition);
      box-shadow: 0 10px 30px rgba(0,0,0,0.2);
      display: flex;
      flex-direction: column;
    }

    .card:hover {
      border-color: var(--card-border-hover);
      box-shadow: 0 15px 35px rgba(0,0,0,0.3), 0 0 20px rgba(0,247,255,0.05);
    }

    .card-title {
      font-size: 0.8rem;
      font-weight: 700;
      letter-spacing: 0.12em;
      text-transform: uppercase;
      color: var(--text-dim);
      margin-bottom: 20px;
      display: flex;
      align-items: center;
      gap: 8px;
    }

    .card-title svg {
      color: var(--accent);
      opacity: 0.8;
    }

    .card-title span {
      color: #fff;
    }

    /* ── Switch Toggle ───────────────────────────────────────────────────── */
    .switch-wrap {
      display: flex;
      align-items: center;
      gap: 10px;
      cursor: pointer;
      user-select: none;
    }
    .switch-label {
      font-size: 0.75rem;
      font-weight: 700;
      color: var(--text-dim);
      text-transform: uppercase;
      letter-spacing: 0.1em;
      transition: var(--transition);
    }
    .switch-wrap:hover .switch-label { color: var(--accent); }
    
    .switch {
      position: relative;
      display: inline-block;
      width: 42px;
      height: 22px;
    }
    .switch input { opacity: 0; width: 0; height: 0; }
    .slider-toggle {
      position: absolute;
      cursor: pointer;
      inset: 0;
      background: rgba(255, 255, 255, 0.05);
      border: 1px solid var(--card-border);
      transition: .4s cubic-bezier(0.4, 0, 0.2, 1);
      border-radius: 34px;
    }
    .slider-toggle:before {
      position: absolute;
      content: "";
      height: 14px;
      width: 14px;
      left: 3px;
      bottom: 3px;
      background: #8b9bb4;
      transition: .4s cubic-bezier(0.4, 0, 0.2, 1);
      border-radius: 50%;
    }
    input:checked + .slider-toggle {
      background: rgba(0, 247, 255, 0.1);
      border-color: rgba(0, 247, 255, 0.4);
    }
    input:checked + .slider-toggle:before {
      transform: translateX(20px);
      background: var(--accent);
      box-shadow: 0 0 10px var(--accent);
    }

    /* ── Forms & Controls ────────────────────────────────────────────────── */
    .zone1-card { grid-area: zone1; }
    .zone2-card { grid-area: zone2; }
    .color-card { grid-area: color; position: relative; z-index: 50; }
    .control-card { grid-area: controls; }
    .vchart-card { grid-area: vchart; }
    .mchart-card { grid-area: mchart; }
    .heat-card { grid-area: heat; }
    footer { grid-area: footer; }

    /* ── Ring gauge ──────────────────────────────────────────────────────── */
    .ring-wrap {
      display: flex;
      justify-content: center;
      margin-bottom: 20px;
      margin-top: 10px;
    }

    .ring-svg {
      transform: rotate(-90deg);
      overflow: visible;
    }

    .ring-bg {
      fill: none;
      stroke: rgba(255,255,255,0.05);
      stroke-width: 12;
    }

    .ring-prog {
      fill: none;
      stroke-width: 12;
      stroke-linecap: round;
      stroke: var(--accent);
      filter: drop-shadow(0 0 10px var(--accent));
      transition: stroke-dashoffset 0.6s cubic-bezier(0.34, 1.56, 0.64, 1);
      stroke-dasharray: 377;
      stroke-dashoffset: 377;
    }

    .ring-label {
      position: absolute;
      top: 50%;
      left: 50%;
      transform: translate(-50%, -50%);
      font-size: 1.8rem;
      font-weight: 700;
      color: #fff;
      text-shadow: 0 0 15px rgba(0,247,255,0.5);
      pointer-events: none;
      font-variant-numeric: tabular-nums;
    }

    .ring-container {
      position: relative;
      width: 140px;
      height: 140px;
    }

    .ring-container svg {
      position: absolute;
      top: 0;
      left: 0;
      width: 140px;
      height: 140px;
    }

    /* ── Custom Range Slider ─────────────────────────────────────────────── */
    .slider-row {
      margin-top: auto;
      background: rgba(0,0,0,0.2);
      padding: 16px;
      border-radius: var(--radius-sm);
      border: 1px solid rgba(255,255,255,0.03);
    }

    .slider-row label {
      display: flex;
      justify-content: space-between;
      font-size: 0.75rem;
      font-weight: 600;
      color: var(--text-dim);
      margin-bottom: 12px;
      text-transform: uppercase;
      letter-spacing: 0.05em;
    }

    input[type="range"] {
      width: 100%;
      appearance: none;
      height: 6px;
      border-radius: 6px;
      background: rgba(255,255,255,0.1);
      cursor: pointer;
      outline: none;
      transition: background var(--transition);
    }
    input[type="range"]:hover { background: rgba(255,255,255,0.15); }

    input[type="range"]::-webkit-slider-thumb {
      appearance: none;
      width: 20px;
      height: 20px;
      border-radius: 50%;
      background: #fff;
      border: 3px solid var(--accent);
      box-shadow: 0 0 10px rgba(0,247,255,0.4);
      cursor: grab;
      transition: transform 0.2s cubic-bezier(0.34, 1.56, 0.64, 1);
    }
    input[type="range"]::-webkit-slider-thumb:active { cursor: grabbing; transform: scale(1.2); }

    input[type="range"]::-moz-range-thumb {
      width: 14px;
      height: 14px;
      border-radius: 50%;
      background: #fff;
      border: 3px solid var(--accent);
      box-shadow: 0 0 10px rgba(0,247,255,0.4);
      cursor: grab;
      transition: transform 0.2s;
    }
    input[type="range"]::-moz-range-thumb:active { cursor: grabbing; transform: scale(1.2); }

    /* ── Color swatch ────────────────────────────────────────────────────── */
    .preset-grid {
      display: grid; 
      grid-template-columns: repeat(4, 1fr); 
      gap: 10px; 
      margin-bottom: 20px;
    }
    
    .preset-btn {
      padding: 10px 4px;
      border: 1px solid rgba(255,255,255,0.1);
      background: rgba(255,255,255,0.03);
      border-radius: var(--radius-sm);
      color: var(--text);
      font-size: 0.75rem;
      font-weight: 600;
      cursor: pointer;
      transition: all var(--transition);
      display: flex;
      flex-direction: column;
      align-items: center;
      gap: 6px;
    }
    .preset-btn:hover { background: rgba(255,255,255,0.08); transform: translateY(-2px); }
    .preset-color { width: 16px; height: 16px; border-radius: 50%; box-shadow: 0 0 8px currentColor; }

    .color-row {
      display: flex;
      align-items: center;
      gap: 14px;
      margin-bottom: 12px;
      background: rgba(0,0,0,0.2);
      padding: 10px 16px;
      border-radius: var(--radius-sm);
    }

    .color-row label {
      font-size: 0.8rem;
      font-weight: 700;
      color: var(--text-dim);
      min-width: 16px;
    }
    
    .color-row input[type="range"]#cr::-webkit-slider-thumb { border-color: #ff3b30; box-shadow: 0 0 10px rgba(255,59,48,0.4); }
    .color-row input[type="range"]#cg::-webkit-slider-thumb { border-color: #34c759; box-shadow: 0 0 10px rgba(52,199,89,0.4); }
    .color-row input[type="range"]#cb::-webkit-slider-thumb { border-color: #007aff; box-shadow: 0 0 10px rgba(0,122,255,0.4); }

    .color-picker-wrap {
      display: flex;
      align-items: center;
      gap: 16px;
      margin-top: auto;
      padding-top: 16px;
      position: relative;
    }

    .custom-picker-btn {
      width: 48px;
      height: 48px;
      border-radius: 50%;
      border: 3px solid rgba(255, 255, 255, 0.8);
      box-shadow: 0 4px 15px rgba(0, 0, 0, 0.5);
      cursor: pointer;
      background: #00ffff;
      transition: transform 0.2s cubic-bezier(0.34, 1.56, 0.64, 1);
    }

    .custom-picker-btn:hover {
      transform: scale(1.1);
    }

    .wheel-modal {
      position: absolute;
      z-index: 100;
      background: rgba(15, 23, 42, 0.95);
      border: 1px solid rgba(255,255,255,0.1);
      padding: 20px;
      border-radius: var(--radius);
      backdrop-filter: blur(20px);
      -webkit-backdrop-filter: blur(20px);
      box-shadow: 0 20px 50px rgba(0,0,0,0.5), 0 0 0 1px rgba(255,255,255,0.05);
      display: none;
      top: 100%;
      left: 0;
      margin-top: 15px;
      animation: popIn 0.4s cubic-bezier(0.34, 1.56, 0.64, 1);
    }

    @keyframes popIn {
      from { opacity: 0; transform: translateY(-15px) scale(0.9); }
      to { opacity: 1; transform: translateY(0) scale(1); }
    }

    /* ── Buttons ─────────────────────────────────────────────────────────── */
    .btn-grid {
      display: grid;
      grid-template-columns: 1fr 1fr;
      gap: 12px;
      height: 100%;
    }

    .btn {
      padding: 16px 10px;
      border: 1px solid rgba(255,255,255,0.05);
      border-radius: var(--radius-sm);
      background: rgba(255,255,255,0.03);
      color: var(--text);
      font-size: 0.85rem;
      font-weight: 600;
      font-family: inherit;
      cursor: pointer;
      transition: all var(--transition);
      display: flex;
      flex-direction: column;
      align-items: center;
      justify-content: center;
      gap: 8px;
    }

    .btn svg { width: 22px; height: 22px; opacity: 0.8; transition: transform var(--transition); }
    .btn:hover svg { transform: scale(1.1); opacity: 1; }
    
    .btn:active { transform: scale(0.96); }

    .btn-on { border-color: rgba(34, 197, 94, 0.3); color: #22c55e; background: rgba(34, 197, 94, 0.05); }
    .btn-on:hover { background: rgba(34, 197, 94, 0.15); box-shadow: 0 0 20px rgba(34, 197, 94, 0.2); }
    .btn-on svg { color: #22c55e; }

    .btn-off { border-color: rgba(255, 51, 102, 0.3); color: var(--accent3); background: rgba(255, 51, 102, 0.05); }
    .btn-off:hover { background: rgba(255, 51, 102, 0.15); box-shadow: 0 0 20px rgba(255, 51, 102, 0.2); }
    .btn-off svg { color: var(--accent3); }

    .btn-auto { border-color: rgba(0, 247, 255, 0.3); color: var(--accent); background: rgba(0, 247, 255, 0.05); }
    .btn-auto:hover { background: rgba(0, 247, 255, 0.15); box-shadow: 0 0 20px rgba(0, 247, 255, 0.2); }
    .btn-auto svg { color: var(--accent); }

    .btn-glow { border-color: rgba(123, 92, 255, 0.3); color: var(--accent2); background: rgba(123, 92, 255, 0.05); }
    .btn-glow:hover { background: rgba(123, 92, 255, 0.15); box-shadow: 0 0 20px rgba(123, 92, 255, 0.2); }
    .btn-glow.active { background: rgba(123, 92, 255, 0.25); box-shadow: 0 0 25px rgba(123, 92, 255, 0.4); border-color: var(--accent2); }
    .btn-glow svg { color: var(--accent2); }

    .btn-random { border-color: rgba(255, 180, 50, 0.3); color: #ffb432; background: rgba(255, 180, 50, 0.05); }
    .btn-random:hover { background: rgba(255, 180, 50, 0.15); box-shadow: 0 0 20px rgba(255, 180, 50, 0.2); }
    .btn-random svg { color: #ffb432; }

    .btn-party { border-color: rgba(255, 80, 200, 0.3); color: #ff50c8; background: rgba(255, 80, 200, 0.05); grid-column: 1/-1; flex-direction: row; padding: 14px;}
    .btn-party:hover { background: rgba(255, 80, 200, 0.15); box-shadow: 0 0 20px rgba(255, 80, 200, 0.3); }
    .btn-party svg { color: #ff50c8; }

    /* ── Power Toggle Switch ─────────────────────────────────────────────── */
    .power-switch-wrap {
      display: flex;
      align-items: center;
      gap: 16px;
      padding: 16px 18px;
      border-radius: var(--radius-sm);
      background: rgba(255,255,255,0.03);
      border: 1px solid rgba(255,255,255,0.06);
      margin-bottom: 16px;
      transition: all var(--transition);
    }
    .power-switch-wrap.is-on {
      border-color: rgba(34,197,94,0.35);
      background: rgba(34,197,94,0.06);
      box-shadow: 0 0 18px rgba(34,197,94,0.1);
    }
    .power-switch-icon {
      width: 40px; height: 40px; border-radius: 10px;
      display: flex; align-items: center; justify-content: center;
      background: rgba(255,255,255,0.04);
      color: var(--text-dim); flex-shrink: 0;
      transition: all var(--transition);
    }
    .power-switch-wrap.is-on .power-switch-icon { color: #22c55e; background: rgba(34,197,94,0.12); }
    .power-switch-wrap.is-off .power-switch-icon { color: var(--accent3); }
    .power-switch-label { flex: 1; }
    .power-switch-label-main { font-size: 0.9rem; font-weight: 700; }
    .power-switch-label-sub { font-size: 0.72rem; color: var(--text-dim); margin-top: 2px; }
    /* iOS-style toggle */
    .toggle-switch { position: relative; display: inline-flex; align-items: center; cursor: pointer; }
    .toggle-switch input { position: absolute; opacity: 0; width: 0; height: 0; }
    .toggle-track {
      width: 52px; height: 28px; border-radius: 14px;
      background: rgba(255,255,255,0.08);
      border: 1px solid rgba(255,255,255,0.1);
      position: relative; transition: all 0.3s ease;
    }
    .toggle-switch input:checked ~ .toggle-track {
      background: rgba(34,197,94,0.6);
      border-color: rgba(34,197,94,0.8);
      box-shadow: 0 0 14px rgba(34,197,94,0.4);
    }
    .toggle-thumb {
      position: absolute; top: 3px; left: 3px;
      width: 20px; height: 20px; border-radius: 50%;
      background: #fff;
      box-shadow: 0 2px 6px rgba(0,0,0,0.4);
      transition: transform 0.3s cubic-bezier(0.34, 1.56, 0.64, 1);
    }
    .toggle-switch input:checked ~ .toggle-track .toggle-thumb { transform: translateX(24px); }

    /* ── Section label ───────────────────────────────────────────────────── */
    .section-label {
      font-size: 0.65rem; font-weight: 700; letter-spacing: 0.12em;
      color: var(--text-dim); margin-bottom: 10px; margin-top: 4px;
      text-transform: uppercase;
    }

    /* ── Brightness preset row ───────────────────────────────────────────── */
    .preset-bright-row {
      display: grid;
      grid-template-columns: repeat(4, 1fr);
      gap: 8px;
      margin-bottom: 14px;
    }
    .preset-bright-btn {
      padding: 9px 6px;
      border-radius: var(--radius-sm);
      border: 1px solid rgba(255,255,255,0.07);
      background: rgba(255,255,255,0.04);
      color: var(--text-dim);
      font-size: 0.75rem;
      font-weight: 700;
      font-family: inherit;
      cursor: pointer;
      transition: all var(--transition);
      letter-spacing: 0.04em;
    }
    .preset-bright-btn:hover {
      background: rgba(0,247,255,0.1);
      border-color: rgba(0,247,255,0.3);
      color: var(--accent);
      box-shadow: 0 0 12px rgba(0,247,255,0.15);
    }
    .preset-bright-btn:active { transform: scale(0.95); }

    /* ── Charts ──────────────────────────────────────────────────────────── */
    .chart-wrap {
      position: relative;
      height: 200px;
      width: 100%;
      margin-top: auto;
    }

    .chart-wrap canvas {
      display: block;
      width: 100% !important;
      height: 100% !important;
    }

    /* ── Heatmap ─────────────────────────────────────────────────────────── */
    .heatmap-grid {
      display: grid;
      grid-template-columns: repeat(10, 1fr);
      gap: 6px;
      margin-top: 10px;
    }

    .hcell {
      aspect-ratio: 1;
      border-radius: 6px;
      background: rgba(255,255,255,0.03);
      box-shadow: inset 0 0 0 1px rgba(255,255,255,0.02);
      transition: all 0.5s ease;
    }

    /* ── Footer ──────────────────────────────────────────────────────────── */
    footer {
      text-align: center;
      padding: 30px 20px;
      font-size: 0.85rem;
      color: var(--text-dim);
    }

    footer a {
      color: var(--accent);
      text-decoration: none;
      font-weight: 600;
      position: relative;
    }

    footer a::after {
      content: '';
      position: absolute;
      width: 0;
      height: 2px;
      bottom: -4px;
      left: 0;
      background: var(--accent);
      transition: width var(--transition);
      border-radius: 2px;
    }

    footer a:hover::after {
      width: 100%;
    }

    /* ── Scrollbar ───────────────────────────────────────────────────────── */
    ::-webkit-scrollbar { width: 8px; }
    ::-webkit-scrollbar-track { background: var(--bg2); }
    ::-webkit-scrollbar-thumb { background: rgba(0, 247, 255, 0.2); border-radius: 4px; }
    ::-webkit-scrollbar-thumb:hover { background: rgba(0, 247, 255, 0.4); }

    /* ── Disabled State ──────────────────────────────────────────────────── */
    .disabled-control {
      opacity: 0.4;
      pointer-events: none;
      filter: grayscale(100%);
      transition: all 0.3s ease;
    }
  </style>
</head>

<body>
  <div id="loadingScreen">
    <div class="loader-content">
      <svg class="blur-pulse" width="64" height="64" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="1.5" stroke-linecap="round" stroke-linejoin="round"><path d="M13 2L3 14h9l-1 8 10-12h-9l1-8z"/></svg>
      <div class="loading-text">INITIALIZING SYSTEM</div>
    </div>
  </div>
  
  <div class="app">

    <!-- ── Header ─────────────────────────────────────────────────────── -->
    <header>
      <div class="header-left">
        <div class="logo-icon">
          <svg width="28" height="28" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M13 2L3 14h9l-1 8 10-12h-9l1-8z"/></svg>
        </div>
        <div class="header-title">
          <h1>Smart Street Lighting</h1>
          <p>Autonomous Control Dashboard</p>
        </div>
      </div>
      <div class="header-right">
        <span id="clock">--:--:--</span>
        <span id="modeBadge" class="mode-badge mode-auto">
          <svg width="14" height="14" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><polyline points="23 4 23 10 17 10"/><polyline points="1 20 1 14 7 14"/><path d="M3.51 9a9 9 0 0 1 14.85-3.36L23 10M1 14l4.64 4.36A9 9 0 0 0 20.49 15"/></svg>
          AUTO
        </span>
      </div>
    </header>

    <!-- ── Status Bar ─────────────────────────────────────────────────── -->
    <div class="status-bar">
      <div class="stat-pill">
        <div class="stat-icon">
          <svg width="22" height="22" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><rect x="2" y="7" width="16" height="10" rx="2" ry="2"/><line x1="22" y1="11" x2="22" y2="13"/><line x1="6" y1="12" x2="6" y2="12"/><line x1="10" y1="12" x2="10" y2="12"/><line x1="14" y1="12" x2="14" y2="12"/></svg>
        </div>
        <div class="stat-info">
          <div class="stat-label">Voltage</div>
          <div class="stat-value txt-accent" id="stVoltage">—</div>
        </div>
      </div>
      <div class="stat-pill">
        <div class="stat-icon">
          <svg width="22" height="22" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M13 2L3 14h9l-1 8 10-12h-9l1-8z"/></svg>
        </div>
        <div class="stat-info">
          <div class="stat-label">Current</div>
          <div class="stat-value txt-accent2" id="stCurrent">—</div>
        </div>
      </div>
      <div class="stat-pill">
        <div class="stat-icon">
          <svg width="22" height="22" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><circle cx="12" cy="12" r="4"/><path d="M12 2v2"/><path d="M12 20v2"/><path d="M5 5l1.5 1.5"/><path d="M17.5 17.5L19 19"/><path d="M2 12h2"/><path d="M20 12h2"/><path d="M5 19l1.5-1.5"/><path d="M17.5 6.5L19 5"/></svg>
        </div>
        <div class="stat-info">
          <div class="stat-label">Ambient Light</div>
          <div class="stat-value txt-accent" id="stLDR">—</div>
        </div>
      </div>
      <div class="stat-pill">
        <div class="stat-icon">
          <svg width="22" height="22" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M1 12s4-8 11-8 11 8 11 8-4 8-11 8-11-8-11-8z"/><circle cx="12" cy="12" r="3"/></svg>
        </div>
        <div class="stat-info">
          <div class="stat-label">Motion</div>
          <div class="stat-value" style="display: flex; align-items: center;"><span class="motion-dot" id="motionDot"></span><span id="stMotion">None</span></div>
        </div>
      </div>
      <div class="stat-pill">
        <div class="stat-icon">
          <svg width="22" height="22" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M14 14.76V3.5a2.5 2.5 0 0 0-5 0v11.26a4.5 4.5 0 1 0 5 0z"/></svg>
        </div>
        <div class="stat-info">
          <div class="stat-label">Temp</div>
          <div class="stat-value txt-accent" id="stTemp">—</div>
        </div>
      </div>
      <div class="stat-pill">
        <div class="stat-icon">
          <svg width="22" height="22" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M12 2.69l5.66 5.66a8 8 0 1 1-11.31 0z"/></svg>
        </div>
        <div class="stat-info">
          <div class="stat-label">Humidity</div>
          <div class="stat-value txt-accent2" id="stHumid">—</div>
        </div>
      </div>
      <div class="stat-pill">
        <div class="stat-icon">
          <svg width="22" height="22" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M20 16.2A4.5 4.5 0 0 0 17.5 8h-1.8A7 7 0 1 0 4 14.9"/><path d="M16 14v6"/><path d="M8 14v6"/><path d="M12 16v6"/></svg>
        </div>
        <div class="stat-info">
          <div class="stat-label">Weather</div>
          <div class="stat-value" id="stWeather">—</div>
        </div>
      </div>
    </div>

    <!-- ── Main Grid ──────────────────────────────────────────────────── -->
    <div class="main-grid">

      <!-- Zone 1 -->
      <div class="card zone1-card">
        <div class="card-title" style="display: flex; justify-content: space-between; align-items: center;">
          <div style="display: flex; align-items: center; gap: 8px;">
            <svg width="18" height="18" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M9 18h6"/><path d="M10 22h4"/><path d="M12 2v1"/><path d="M12 7v1"/><path d="M12 12v1"/></svg>
            Zone <span>1</span> Brightness
          </div>
          <label class="switch-wrap">
            <span class="switch-label" style="font-size: 0.65rem;">Sync</span>
            <div class="switch" style="transform: scale(0.8); transform-origin: right;">
              <input type="checkbox" id="syncCheck" onchange="handleSyncToggle(this.checked)">
              <span class="slider-toggle"></span>
            </div>
          </label>
        </div>
        <div class="ring-wrap">
          <div class="ring-container">
            <svg class="ring-svg" viewBox="0 0 140 140">
              <circle class="ring-bg" cx="70" cy="70" r="60" />
              <circle class="ring-prog" cx="70" cy="70" r="60" id="ring1" />
            </svg>
            <div class="ring-label" id="lbl1">0%</div>
          </div>
        </div>
        <div class="slider-row">
          <label><span>Override Level</span> <span id="val1" style="color:#fff">0%</span></label>
          <input type="range" min="0" max="100" value="0" id="z1slider" oninput="syncZones(1, this.value); update();">
        </div>
      </div>

      <!-- Zone 2 -->
      <div class="card zone2-card">
        <div class="card-title">
          <svg width="18" height="18" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M9 18h6"/><path d="M10 22h4"/><path d="M12 2v1"/><path d="M12 7v1"/><path d="M12 12v1"/></svg>
          Zone <span>2</span> Brightness
        </div>
        <div class="ring-wrap">
          <div class="ring-container">
            <svg class="ring-svg" viewBox="0 0 140 140">
              <circle class="ring-bg" cx="70" cy="70" r="60" />
              <circle class="ring-prog" cx="70" cy="70" r="60" id="ring2" />
            </svg>
            <div class="ring-label" id="lbl2">0%</div>
          </div>
        </div>
        <div class="slider-row">
          <label><span>Override Level</span> <span id="val2" style="color:#fff">0%</span></label>
          <input type="range" min="0" max="100" value="0" id="z2slider" oninput="syncZones(2, this.value); update();">
        </div>
      </div>

      <!-- Color Control -->
      <div class="card color-card">
        <div class="card-title">
          <svg width="18" height="18" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M12 2C6.5 2 2 6.5 2 12s4.5 10 10 10c.926 0 1.648-.746 1.648-1.688 0-.437-.18-.835-.437-1.125-.29-.289-.438-.652-.438-1.125a1.64 1.64 0 0 1 1.668-1.668h1.996c3.051 0 5.555-2.503 5.555-5.554C21.965 6.012 17.461 2 12 2z"/></svg>
          RGB <span>Glow Color</span>
        </div>
        
        <div class="preset-grid">
          <button class="preset-btn" onclick="setPreset(255,255,255)">
            <div class="preset-color" style="color:#ffffff; background:#ffffff"></div> White
          </button>
          <button class="preset-btn" onclick="setPreset(255,180,50)">
            <div class="preset-color" style="color:#ffb432; background:#ffb432"></div> Fog
          </button>
          <button class="preset-btn" onclick="setPreset(0,180,255)">
            <div class="preset-color" style="color:#00b4ff; background:#00b4ff"></div> Blue
          </button>
          <button class="preset-btn" onclick="setPreset(180,80,255)">
            <div class="preset-color" style="color:#b450ff; background:#b450ff"></div> Violet
          </button>
        </div>

        <div class="color-row">
          <label>R</label>
          <input type="range" min="0" max="255" value="0" id="cr" style="flex:1">
        </div>
        <div class="color-row">
          <label>G</label>
          <input type="range" min="0" max="255" value="255" id="cg" style="flex:1">
        </div>
        <div class="color-row">
          <label>B</label>
          <input type="range" min="0" max="255" value="255" id="cb" style="flex:1">
        </div>
        
        <div class="color-picker-wrap">
          <div class="custom-picker-btn" id="colorPickerBtn"></div>
          <div>
            <div style="font-size:0.85rem; font-weight:600; color:#fff">Color Picker</div>
            <div style="font-size:0.75rem; color:var(--text-dim)">Click to use color wheel</div>
          </div>
          <div class="wheel-modal" id="wheelModal">
            <canvas id="colorWheel" width="180" height="180" style="border-radius:50%; cursor:crosshair; box-shadow:0 0 20px rgba(0,0,0,0.5);"></canvas>
          </div>
        </div>
      </div>

      <!-- Controls -->
      <div class="card control-card">
        <div class="card-title">
          <svg width="18" height="18" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><circle cx="12" cy="12" r="3"/><path d="M19.4 15a1.65 1.65 0 0 0 .33 1.82l.06.06a2 2 0 0 1 0 2.83 2 2 0 0 1-2.83 0l-.06-.06a1.65 1.65 0 0 0-1.82-.33 1.65 1.65 0 0 0-1 1.51V21a2 2 0 0 1-2 2 2 2 0 0 1-2-2v-.09A1.65 1.65 0 0 0 9 19.4a1.65 1.65 0 0 0-1.82.33l-.06.06a2 2 0 0 1-2.83 0 2 2 0 0 1 0-2.83l.06-.06a1.65 1.65 0 0 0 .33-1.82 1.65 1.65 0 0 0-1.51-1H3a2 2 0 0 1-2-2 2 2 0 0 1 2-2h.09A1.65 1.65 0 0 0 4.6 9a1.65 1.65 0 0 0-.33-1.82l-.06-.06a2 2 0 0 1 0-2.83 2 2 0 0 1 2.83 0l.06.06a1.65 1.65 0 0 0 1.82.33H9a1.65 1.65 0 0 0 1-1.51V3a2 2 0 0 1 2-2 2 2 0 0 1 2 2v.09a1.65 1.65 0 0 0 1 1.51 1.65 1.65 0 0 0 1.82-.33l.06-.06a2 2 0 0 1 2.83 0 2 2 0 0 1 0 2.83l-.06.06a1.65 1.65 0 0 0-.33 1.82V9a1.65 1.65 0 0 0 1.51 1H21a2 2 0 0 1 2 2 2 2 0 0 1-2 2h-.09a1.65 1.65 0 0 0-1.51 1z"/></svg>
          System <span>Controls</span>
        </div>

        <!-- Power Toggle Switch -->
        <div class="power-switch-wrap is-on" id="powerSwitchWrap">
          <div class="power-switch-icon">
            <svg width="20" height="20" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M18.36 6.64a9 9 0 1 1-12.73 0M12 2v10"/></svg>
          </div>
          <div class="power-switch-label">
            <div class="power-switch-label-main">System Power</div>
            <div class="power-switch-label-sub" id="powerStatus">System is ON</div>
          </div>
          <label class="toggle-switch">
            <input type="checkbox" id="powerCheck" checked onchange="togglePower(this.checked)">
            <span class="toggle-track"><span class="toggle-thumb"></span></span>
          </label>
        </div>

        <!-- Brightness Presets -->
        <div class="section-label">Brightness Preset</div>
        <div class="preset-bright-row">
          <button class="preset-bright-btn" onclick="setBrightness(100)">MAX</button>
          <button class="preset-bright-btn" onclick="setBrightness(75)">75%</button>
          <button class="preset-bright-btn" onclick="setBrightness(50)">50%</button>
          <button class="preset-bright-btn" onclick="setBrightness(20)">20%</button>
        </div>

        <!-- Mode & Feature Buttons -->
        <div class="section-label">Mode &amp; Features</div>
        <div class="btn-grid">
          <button class="btn btn-auto" id="btnAuto" onclick="cmd('auto')">
            <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><polyline points="23 4 23 10 17 10"/><polyline points="1 20 1 14 7 14"/><path d="M3.51 9a9 9 0 0 1 14.85-3.36L23 10M1 14l4.64 4.36A9 9 0 0 0 20.49 15"/></svg>
            Auto Mode
          </button>
          <button class="btn btn-glow" id="btnGlow" onclick="toggleGlow()">
            <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M21 12.79A9 9 0 1 1 11.21 3 7 7 0 0 0 21 12.79z"/></svg>
            Soft Glow
          </button>
          <button class="btn btn-random" onclick="randomColor()">
            <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><polyline points="16 3 21 3 21 8"/><line x1="4" y1="20" x2="21" y2="3"/><polyline points="21 16 21 21 16 21"/><line x1="15" y1="15" x2="21" y2="21"/></svg>
            Random Color
          </button>
          <button class="btn btn-auto" onclick="flashZones()">
            <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M13 2L3 14h9l-1 8 10-12h-9l1-8z"/></svg>
            Flash
          </button>
          <button class="btn btn-glow" id="btnLightBox" onclick="toggleLightBox()" style="grid-column: 1/-1; flex-direction: row; padding: 14px;">
            <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M12 2C6.48 2 2 6.48 2 12s4.48 10 10 10 10-4.48 10-10S17.52 2 12 2zm0 18c-4.41 0-8-3.59-8-8s3.59-8 8-8 8 3.59 8 8-3.59 8-8 8z"/></svg>
            Toggle Light Box
          </button>
          <button class="btn btn-party" id="btnParty" onclick="toggleParty()">
            <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M12 2v4M12 18v4M4.93 4.93l2.83 2.83M16.24 16.24l2.83 2.83M2 12h4M18 12h4M4.93 19.07l2.83-2.83M16.24 7.76l2.83-2.83"/></svg>
            Party Mode
          </button>
        </div>
      </div>

      <!-- Voltage Chart -->
      <div class="card vchart-card">
        <div class="card-title">
          <svg width="18" height="18" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><polyline points="22 12 18 12 15 21 9 3 6 12 2 12"/></svg>
          Battery <span>Voltage History</span>
        </div>
        <div class="chart-wrap">
          <canvas id="vChart"></canvas>
        </div>
      </div>

      <!-- Motion Chart -->
      <div class="card mchart-card">
        <div class="card-title">
          <svg width="18" height="18" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><polyline points="22 12 18 12 15 21 9 3 6 12 2 12"/></svg>
          Motion <span>Activity History</span>
        </div>
        <div class="chart-wrap">
          <canvas id="mChart"></canvas>
        </div>
      </div>

      <!-- Heatmap -->
      <div class="card heat-card">
        <div class="card-title">
          <svg width="18" height="18" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><rect x="3" y="3" width="18" height="18" rx="2" ry="2"/><line x1="3" y1="9" x2="21" y2="9"/><line x1="3" y1="15" x2="21" y2="15"/><line x1="9" y1="3" x2="9" y2="21"/><line x1="15" y1="3" x2="15" y2="21"/></svg>
          Motion <span>Activity Heatmap</span> <span style="font-size: 0.7rem; margin-left: auto; text-transform: none;">Last 50 samples</span>
        </div>
        <div class="heatmap-grid" id="heat"></div>
      </div>

      <!-- Footer -->
      <footer>
        Made with <svg width="14" height="14" style="vertical-align:-2px; color:var(--accent)" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M13 2L3 14h9l-1 8 10-12h-9l1-8z"/></svg> by <a href="https://github.com/MohamedTarek3M" target="_blank">Mohamed Tarek</a>
        <br><span style="opacity:0.6; font-size:0.75rem; margin-top:8px; display:inline-block">Smart Street Lighting</span>
      </footer>

    </div>
  </div>

  <script>
    // ── Networking Helpers ───────────────────────────────────────────────
    const fastFetch = async (url, options = {}) => {
      const controller = new AbortController();
      const id = setTimeout(() => controller.abort(), 5000); // Increased to 5s
      try {
        const r = await fetch(url, { ...options, signal: controller.signal });
        clearTimeout(id);
        return r;
      } catch (e) {
        clearTimeout(id);
        throw e;
      }
    };

    // ── Optimized Command Functions ──────────────────────────────────────
    let isCmdBusy = false;
    const safeCmd = async (url) => {
      if (isCmdBusy) return; 
      isCmdBusy = true;
      try { await fastFetch(url); } catch(e) {}
      finally { isCmdBusy = false; }
    }

    const cmd = async (c) => {
      if (partyInt) toggleParty();
      const badge = document.getElementById('modeBadge');
      if (c === 'auto') {
        badge.innerHTML = 'AUTO';
        badge.className = 'mode-badge mode-auto';
        document.getElementById('powerCheck').checked = true;
        document.getElementById('powerSwitchWrap').className = 'power-switch-wrap is-on';
      }
      await safeCmd('/' + c);
    }

    const togglePower = async (isOn) => {
      if (partyInt) toggleParty();
      const wrap = document.getElementById('powerSwitchWrap');
      const status = document.getElementById('powerStatus');
      wrap.className = isOn ? 'power-switch-wrap is-on' : 'power-switch-wrap is-off';
      status.textContent = isOn ? 'System is ON' : 'System is OFF';
      await safeCmd('/' + (isOn ? 'on' : 'off'));
    }

    let glowEnabled = true;
    const toggleGlow = async () => {
      glowEnabled = !glowEnabled;
      document.getElementById('btnGlow').classList.toggle('active', glowEnabled);
      await safeCmd('/glowToggle');
    }

    let lightBoxState = false;
    const toggleLightBox = async () => {
      lightBoxState = !lightBoxState;
      const btn = document.getElementById('btnLightBox');
      if (btn) btn.classList.toggle('active', lightBoxState);
      
      if (isCmdBusy) return;
      isCmdBusy = true;
      try { 
        const r = await fastFetch('/toggleLightBox'); 
        if (r.ok) {
          const t = await r.text();
          lightBoxState = (t === '1');
          if (btn) btn.classList.toggle('active', lightBoxState);
        }
      } catch(e) {} finally { isCmdBusy = false; }
    }

    // ── Custom Lightweight Canvas Chart ──────────────────────────────────
    const drawLineChart = (canvasId, data, color, minVal, maxVal) => {
      const canvas = document.getElementById(canvasId);
      if (!canvas) return;
      const ctx = canvas.getContext('2d');
      const dpr = window.devicePixelRatio || 1;
      const rect = canvas.parentElement.getBoundingClientRect();
      canvas.width = rect.width * dpr;
      canvas.height = rect.height * dpr;
      ctx.scale(dpr, dpr);
      const w = rect.width, h = rect.height;
      ctx.clearRect(0, 0, w, h);
      ctx.strokeStyle = 'rgba(255,255,255,0.05)';
      ctx.lineWidth = 1;
      ctx.beginPath();
      for (let i = 0; i <= 4; i++) {
        let y = h - (i / 4) * h;
        ctx.moveTo(0, y); ctx.lineTo(w, y);
      }
      ctx.stroke();
      if (!data || data.length === 0) return;
      const stepX = w / (data.length - 1 || 1);
      const range = maxVal - minVal || 1;
      ctx.beginPath();
      for (let i = 0; i < data.length; i++) {
        let val = Math.max(minVal, Math.min(maxVal, data[i]));
        let x = i * stepX, y = h - ((val - minVal) / range) * h;
        if (i === 0) ctx.moveTo(x, y); else ctx.lineTo(x, y);
      }
      ctx.lineTo(w, h); ctx.lineTo(0, h); ctx.closePath();
      const gradient = ctx.createLinearGradient(0, 0, 0, h);
      gradient.addColorStop(0, color.replace('rgb', 'rgba').replace(')', ',0.25)'));
      gradient.addColorStop(1, color.replace('rgb', 'rgba').replace(')', ',0.0)'));
      ctx.fillStyle = gradient; ctx.fill();
      ctx.beginPath();
      for (let i = 0; i < data.length; i++) {
        let val = Math.max(minVal, Math.min(maxVal, data[i]));
        let x = i * stepX, y = h - ((val - minVal) / range) * h;
        if (i === 0) ctx.moveTo(x, y); else ctx.lineTo(x, y);
      }
      ctx.strokeStyle = color; ctx.lineWidth = 2.5; ctx.lineJoin = 'round'; ctx.lineCap = 'round';
      ctx.stroke();
    };

    // ── UI Components ────────────────────────────────────────────────────
    const heat = document.getElementById('heat');
    for (let i = 0; i < 50; i++) {
      const d = document.createElement('div');
      d.className = 'hcell';
      heat.appendChild(d);
    }

    const CIRC = 2 * Math.PI * 60; 
    const updateRing = (id, lblId, val) => {
      const pct = val / 100;
      document.getElementById(id).style.strokeDashoffset = CIRC * (1 - pct);
      document.getElementById(lblId).textContent = Math.round(val) + '%';
    }

    const cw = document.getElementById('colorWheel');
    const ctxCw = cw.getContext('2d', { willReadFrequently: true });
    const wx = cw.width / 2;
    const conic = ctxCw.createConicGradient(0, wx, wx);
    conic.addColorStop(0, '#f00'); conic.addColorStop(1 / 6, '#ff0');
    conic.addColorStop(2 / 6, '#0f0'); conic.addColorStop(3 / 6, '#0ff');
    conic.addColorStop(4 / 6, '#00f'); conic.addColorStop(5 / 6, '#f0f');
    conic.addColorStop(1, '#f00');
    ctxCw.fillStyle = conic;
    ctxCw.beginPath(); ctxCw.arc(wx, wx, wx, 0, 2 * Math.PI); ctxCw.fill();
    const radial = ctxCw.createRadialGradient(wx, wx, 0, wx, wx, wx);
    radial.addColorStop(0, 'rgba(255,255,255,1)');
    radial.addColorStop(1, 'rgba(255,255,255,0)');
    ctxCw.fillStyle = radial;
    ctxCw.beginPath(); ctxCw.arc(wx, wx, wx, 0, 2 * Math.PI); ctxCw.fill();

    const pickColor = (e) => {
      const rect = cw.getBoundingClientRect();
      const x = Math.max(0, Math.min(cw.width - 1, (e.clientX || e.touches[0].clientX) - rect.left));
      const y = Math.max(0, Math.min(cw.height - 1, (e.clientY || e.touches[0].clientY) - rect.top));
      const dx = x - wx, dy = y - wx;
      if (dx * dx + dy * dy > wx * wx) return; 
      const data = ctxCw.getImageData(x, y, 1, 1).data;
      document.getElementById('cr').value = data[0];
      document.getElementById('cg').value = data[1];
      document.getElementById('cb').value = data[2];
      onColorChange();
    };

    let isDraggingW = false;
    cw.addEventListener('mousedown', (e) => { isDraggingW = true; pickColor(e); });
    window.addEventListener('mouseup', () => { if(isDraggingW) document.getElementById('wheelModal').style.display = 'none'; isDraggingW = false; });
    cw.addEventListener('mousemove', (e) => { if (isDraggingW) pickColor(e); });
    cw.addEventListener('touchstart', (e) => { isDraggingW = true; pickColor(e); e.preventDefault(); }, { passive: false });
    cw.addEventListener('touchmove', (e) => { if (isDraggingW) pickColor(e); e.preventDefault(); }, { passive: false });

    const btnPick = document.getElementById('colorPickerBtn');
    const modWheel = document.getElementById('wheelModal');
    btnPick.addEventListener('click', (e) => { e.stopPropagation(); modWheel.style.display = modWheel.style.display === 'block' ? 'none' : 'block'; });
    window.addEventListener('click', (e) => { if (!modWheel.contains(e.target) && e.target !== btnPick) modWheel.style.display = 'none'; });

    // ── Control Logic ────────────────────────────────────────────────────
    function syncZones(zone, val) {
      document.getElementById('val' + zone).textContent = val + '%';
      if (document.getElementById('syncCheck').checked) {
        let other = (zone === 1) ? 2 : 1;
        document.getElementById('z' + other + 'slider').value = val;
        document.getElementById('val' + other).textContent = val + '%';
      }
    }


    const handleSyncToggle = async (checked) => {
      if (checked) {
        syncZones(1, document.getElementById('z1slider').value);
      } else {
        document.getElementById('z1slider').value = 0; document.getElementById('z2slider').value = 0;
        document.getElementById('val1').textContent = '0%'; document.getElementById('val2').textContent = '0%';
        updateRing('ring1', 'lbl1', 0); updateRing('ring2', 'lbl2', 0);
        await safeCmd(`/setZone?z=1&b=0`);
        await safeCmd(`/setZone?z=2&b=0`);
        cmd('auto');
      }
    }

    const setPreset = (r, g, b, snap = false) => {
      document.getElementById('cr').value = r; document.getElementById('cg').value = g; document.getElementById('cb').value = b;
      onColorChange(snap);
    };

    let colorDebounce = null;
    let isColorBusy = false;
    const onColorChange = (snap = false) => {
      const r = document.getElementById('cr').value, g = document.getElementById('cg').value, b = document.getElementById('cb').value;
      const btn = document.getElementById('colorPickerBtn');
      btn.style.background = `rgb(${r},${g},${b})`;
      btn.style.boxShadow = `inset 0 0 4px rgba(0,0,0,0.3), 0 0 20px rgba(${r},${g},${b},0.6), 0 0 0 2px rgba(255,255,255,0.8)`;
      
      // If snap is true, bypass debounce and send the 'snap=1' parameter to the server
      const doFetch = async () => {
        if (isColorBusy) return;
        isColorBusy = true;
        try { await fastFetch(`/setColor?r=${r}&g=${g}&b=${b}${snap ? '&snap=1' : ''}`); } catch(e) {}
        finally { isColorBusy = false; }
      };

      clearTimeout(colorDebounce);
      if (snap === true) {
        doFetch();
      } else {
        colorDebounce = setTimeout(doFetch, 150); 
      }
    }
    ['cr', 'cg', 'cb'].forEach(id => document.getElementById(id).addEventListener('input', onColorChange));

    let z1Busy = false;
    document.getElementById('z1slider').addEventListener('input', e => {
      const v = e.target.value; document.getElementById('val1').textContent = v + '%';
      syncZones(1, v); const b255 = Math.round(v * 2.55);
      if (z1Busy) return;
      z1Busy = true;
      setTimeout(async () => {
        try { await fastFetch(`/setZone?z=1&b=${b255}`); } catch(e) {}
        finally { z1Busy = false; }
      }, 100);
    });

    let z2Busy = false;
    document.getElementById('z2slider').addEventListener('input', e => {
      const v = e.target.value; document.getElementById('val2').textContent = v + '%';
      syncZones(2, v); const b255 = Math.round(v * 2.55);
      if (z2Busy) return;
      z2Busy = true;
      setTimeout(async () => {
        try { await fastFetch(`/setZone?z=2&b=${b255}`); } catch(e) {}
        finally { z2Busy = false; }
      }, 100);
    });

    const setBrightness = async (val) => {
      if (partyInt) toggleParty();
      document.getElementById('z1slider').value = val; document.getElementById('z2slider').value = val;
      document.getElementById('val1').textContent = val + '%'; document.getElementById('val2').textContent = val + '%';
      updateRing('ring1', 'lbl1', val); updateRing('ring2', 'lbl2', val);
      const b255 = Math.round(val * 2.55);
      await safeCmd(`/setZone?z=1&b=${b255}`);
      await safeCmd(`/setZone?z=2&b=${b255}`);
    }

    let flashInt = null;
    const flashZones = () => {
      if (partyInt) toggleParty();
      safeCmd('/flash'); // Trigger physical LEDs
      if (flashInt) { clearInterval(flashInt); flashInt = null; return; }
      let flashState = false;
      flashInt = setInterval(() => {
        const v = flashState ? 100 : 0;
        updateRing('ring1', 'lbl1', v); updateRing('ring2', 'lbl2', v);
        flashState = !flashState;
      }, 400);
      setTimeout(() => { if (flashInt) { clearInterval(flashInt); flashInt = null; } }, 4000);
    }

    const randomColor = () => { setPreset(Math.floor(Math.random()*256), Math.floor(Math.random()*256), Math.floor(Math.random()*256), true); }

    const toggleParty = async () => {
      const btn = document.getElementById('btnParty');
      try {
        const r = await fastFetch('/partyToggle');
        const state = await r.text();
        if(state === "0") { 
          btn.style.boxShadow = ''; btn.style.background = '';
        } else { 
          btn.style.boxShadow = '0 0 25px rgba(255,80,200,0.5)'; btn.style.background = 'rgba(255,80,200,0.15)'; 
        }
      } catch (e) {}
    }

    const getBatteryClass = (v) => { if (v <= 9.0) return 'txt-accent3'; if (v <= 10.5) return 'txt-warn'; return 'txt-ok'; }

    const updateHeatmap = (motionHist) => {
      const cells = document.querySelectorAll('.hcell');
      cells.forEach((cell, i) => {
        const val = motionHist[i] || 0;
        if (val) { cell.style.background = `rgba(0, 247, 255, ${0.4 + val * 0.6})`; cell.style.boxShadow = `0 0 10px rgba(0,247,255,0.5)`;
        } else { cell.style.background = 'rgba(255,255,255,0.03)'; cell.style.boxShadow = 'none'; }
      });
    }

    // ── Telemetry ────────────────────────────────────────────────────────
    let isStatusBusy = false;
    const fetchStatus = async () => {
      if (isStatusBusy) return;
      isStatusBusy = true;
      try {
        const r = await fastFetch('/status');
        if (!r.ok) throw new Error();
        const d = await r.json();
        
        const vEl = document.getElementById('stVoltage');
        vEl.textContent = d.voltage.toFixed(2) + ' V';
        vEl.className = 'stat-value ' + getBatteryClass(d.voltage);
        document.getElementById('stCurrent').textContent = d.current.toFixed(1) + ' mA';

        const ldrEl = document.getElementById('stLDR');
        if (d.ldr < 500) { ldrEl.innerHTML = 'Dark'; ldrEl.className = 'stat-value txt-accent2';
        } else if (d.ldr < 2000) { ldrEl.innerHTML = 'Dim'; ldrEl.className = 'stat-value txt-warn';
        } else { ldrEl.innerHTML = 'Bright'; ldrEl.className = 'stat-value txt-ok'; }

        document.getElementById('stTemp').textContent = d.temp.toFixed(1) + ' °C';
        document.getElementById('stHumid').textContent = d.humid.toFixed(1) + ' %';
        
        const weatherEl = document.getElementById('stWeather');
        if (d.raining) { weatherEl.innerHTML = 'Raining'; weatherEl.className = 'stat-value txt-accent';
        } else { weatherEl.innerHTML = 'Clear'; weatherEl.className = 'stat-value txt-ok'; }

        const btnParty = document.getElementById('btnParty');
        if (d.party) {
          btnParty.style.boxShadow = '0 0 25px rgba(255,80,200,0.5)'; btnParty.style.background = 'rgba(255,80,200,0.15)';
        } else {
          btnParty.style.boxShadow = ''; btnParty.style.background = '';
        }

        const motionDot = document.getElementById('motionDot');
        const motionTxt = document.getElementById('stMotion');
        if (d.motion) { motionDot.classList.add('active'); motionTxt.textContent = 'Detected';
        } else { motionDot.classList.remove('active'); motionTxt.textContent = 'None'; }

        if (document.activeElement !== document.getElementById('z1slider') && !flashInt) {
          const v100 = Math.round(d.z1 / 2.55);
          document.getElementById('z1slider').value = v100; document.getElementById('val1').textContent = v100 + '%'; updateRing('ring1', 'lbl1', v100);
        }
        if (document.activeElement !== document.getElementById('z2slider') && !flashInt) {
          const v100 = Math.round(d.z2 / 2.55);
          document.getElementById('z2slider').value = v100; document.getElementById('val2').textContent = v100 + '%'; updateRing('ring2', 'lbl2', v100);
        }

        const badge = document.getElementById('modeBadge');
        if (partyInt) { badge.innerHTML = 'PARTY'; badge.className = 'mode-badge mode-party';
        } else if (d.mode === 'auto') { badge.innerHTML = 'AUTO'; badge.className = 'mode-badge mode-auto';
        } else {
          if (d.z1 == 0 && d.z2 == 0) { badge.innerHTML = 'OFF'; badge.className = 'mode-badge mode-off';
          } else { badge.innerHTML = 'OVERRIDE'; badge.className = 'mode-badge mode-override'; }
        }

        // Power Status Sync
        const powerWrap = document.getElementById('powerSwitchWrap');
        const powerCheck = document.getElementById('powerCheck');
        const powerStatus = document.getElementById('powerStatus');
        if (d.power === 1) {
          powerWrap.className = 'power-switch-wrap is-on';
          if (!powerCheck.checked) powerCheck.checked = true;
          powerStatus.textContent = 'System is ON';
        } else {
          powerWrap.className = 'power-switch-wrap is-off';
          if (powerCheck.checked) powerCheck.checked = false;
          powerStatus.textContent = 'System is OFF';
        }

        // Disable controls if system is off
        const controlsDisabled = (d.power === 0);
        document.querySelectorAll('.zone1-card, .zone2-card, .color-card, .preset-bright-row, .btn-grid').forEach(el => {
          if (controlsDisabled) {
            el.classList.add('disabled-control');
          } else {
            el.classList.remove('disabled-control');
          }
        });

        glowEnabled = d.glow === 1; document.getElementById('btnGlow').classList.toggle('active', glowEnabled);
        const btnLightBox = document.getElementById('btnLightBox');
        if (btnLightBox) { lightBoxState = d.box === 1; btnLightBox.classList.toggle('active', lightBoxState); }

        if (document.activeElement !== document.getElementById('cr') && document.activeElement !== document.getElementById('cg') && document.activeElement !== document.getElementById('cb')) {
          const r = d.r, g = d.g, b = d.b;
          document.getElementById('cr').value = r; document.getElementById('cg').value = g; document.getElementById('cb').value = b;
          const btn = document.getElementById('colorPickerBtn');
          if (btn) { btn.style.background = `rgb(${r},${g},${b})`; btn.style.boxShadow = `inset 0 0 4px rgba(0,0,0,0.3), 0 0 20px rgba(${r},${g},${b},0.6), 0 0 0 2px rgba(255,255,255,0.8)`; }
        }
      } catch(e) {} finally { isStatusBusy = false; }
    }

    let isHistoryBusy = false;
    const fetchHistory = async () => {
      if (isHistoryBusy) return;
      isHistoryBusy = true;
      try {
        const r = await fastFetch('/history');
        if (!r.ok) return;
        const d = await r.json();
        drawLineChart('vChart', d.voltageHistory, 'rgb(0,247,255)', 0, 15);
        drawLineChart('mChart', d.motionHistory, 'rgb(123,92,255)', 0, 1);
        updateHeatmap(d.motionHistory);
      } catch(e) {} finally { isHistoryBusy = false; }
    }

    const dismissLoader = () => {
      const loader = document.getElementById('loadingScreen');
      if (loader && loader.style.opacity !== '0') {
        loader.style.opacity = '0'; loader.style.visibility = 'hidden';
        setTimeout(() => { if (loader.parentNode) loader.remove(); }, 600);
      }
    };

    setInterval(() => {
      const el = document.getElementById('clock');
      if (el) el.textContent = new Date().toLocaleTimeString('en-US', { hour: '2-digit', minute: '2-digit', second: '2-digit', hour12: true });
    }, 1000);

    fetchStatus().then(() => { dismissLoader(); fetchHistory(); }).catch(dismissLoader);
    setTimeout(dismissLoader, 3000);
    setInterval(fetchStatus,  1500);
    setInterval(fetchHistory, 10000);
  </script>
</body>
</html>

)rawhtml";

#endif
