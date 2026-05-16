import re

with open('demo.html', 'r', encoding='utf-8') as f:
    demo_html = f.read()

# Extract everything before the JS generation part
head_and_body = demo_html.split('// ── Color helpers ────────────────────────────────────────────────────')[0]

# ESP32 specific JS logic
esp32_js = """// ── Color helpers ────────────────────────────────────────────────────
    const setPreset = (r, g, b) => {
      document.getElementById('cr').value = r;
      document.getElementById('cg').value = g;
      document.getElementById('cb').value = b;
      onColorChange();
    };

    let colorDebounce = null;
    const onColorChange = () => {
      const r = document.getElementById('cr').value;
      const g = document.getElementById('cg').value;
      const b = document.getElementById('cb').value;
      document.getElementById('colorPickerBtn').style.background = `rgb(${r},${g},${b})`;
      document.getElementById('colorPickerBtn').style.boxShadow = `inset 0 0 4px rgba(0,0,0,0.3), 0 0 20px rgba(${r},${g},${b},0.6), 0 0 0 2px rgba(255,255,255,0.8)`;
      
      clearTimeout(colorDebounce);
      colorDebounce = setTimeout(() => {
        fetch(`/setColor?r=${r}&g=${g}&b=${b}`).catch(() => {});
      }, 80);
    }
    ['cr', 'cg', 'cb'].forEach(id => document.getElementById(id).addEventListener('input', onColorChange));
    onColorChange();

    document.getElementById('z1slider').addEventListener('input', e => {
      document.getElementById('val1').textContent = e.target.value;
      fetch(`/setZone?z=1&b=${e.target.value}`).catch(() => {});
    });
    document.getElementById('z2slider').addEventListener('input', e => {
      document.getElementById('val2').textContent = e.target.value;
      fetch(`/setZone?z=2&b=${e.target.value}`).catch(() => {});
    });

    let systemPowered = true;
    let onAnimTimeout = null;

    const togglePower = (isOn) => {
      if (partyInt) toggleParty();
      systemPowered = isOn;
      const wrap = document.getElementById('powerSwitchWrap');
      const status = document.getElementById('powerStatus');
      const badge = document.getElementById('modeBadge');
      
      if (isOn) {
        wrap.className = 'power-switch-wrap is-on';
        status.textContent = 'System is ON';
        badge.innerHTML = '<svg width="14" height="14" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><path d="M18.36 6.64a9 9 0 1 1-12.73 0M12 2v10"/></svg> ON';
        badge.className = 'mode-badge mode-on-anim';
        clearTimeout(onAnimTimeout);
        onAnimTimeout = setTimeout(() => { onAnimTimeout = null; update(); }, 1600);
      } else {
        wrap.className = 'power-switch-wrap is-off';
        status.textContent = 'System is OFF';
        badge.innerHTML = '<svg width="14" height="14" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><path d="M18.36 6.64a9 9 0 1 1-12.73 0M12 2v10"/></svg> OFF';
        badge.className = 'mode-badge mode-off';
      }
      fetch('/' + (isOn ? 'on' : 'off')).catch(() => {});
    }

    const setBrightness = (val) => {
      if (partyInt) toggleParty();
      if (!systemPowered) {
        document.getElementById('powerCheck').checked = true;
        togglePower(true);
      }
      document.getElementById('z1slider').value = val;
      document.getElementById('z2slider').value = val;
      document.getElementById('val1').textContent = val;
      document.getElementById('val2').textContent = val;
      updateRing('ring1', 'lbl1', val);
      updateRing('ring2', 'lbl2', val);
      fetch(`/setZone?z=1&b=${val}`).catch(() => {});
      fetch(`/setZone?z=2&b=${val}`).catch(() => {});
    }

    let flashInt = null;
    const flashZones = () => {
      if (partyInt) toggleParty();
      if (flashInt) { clearInterval(flashInt); flashInt = null; update(); return; }
      let flashState = false;
      flashInt = setInterval(() => {
        const v = flashState ? 255 : 0;
        document.getElementById('z1slider').value = v;
        document.getElementById('z2slider').value = v;
        document.getElementById('val1').textContent = v;
        document.getElementById('val2').textContent = v;
        updateRing('ring1', 'lbl1', v);
        updateRing('ring2', 'lbl2', v);
        flashState = !flashState;
      }, 400);
      setTimeout(() => { if (flashInt) { clearInterval(flashInt); flashInt = null; update(); } }, 4000);
    }

    const cmd = (c) => {
      if (partyInt) toggleParty();
      if (c === 'auto') {
        document.getElementById('powerCheck').checked = true;
        document.getElementById('powerSwitchWrap').className = 'power-switch-wrap is-on';
        document.getElementById('powerStatus').textContent = 'Auto Mode Active';
        systemPowered = true;
      }
      fetch('/' + c).catch(() => {});
    }

    let glowEnabled = true;
    const toggleGlow = () => {
      fetch('/glowToggle')
        .then(r => r.text())
        .then(t => {
          glowEnabled = t === '1';
          const btn = document.getElementById('btnGlow');
          btn.classList.toggle('active', glowEnabled);
        })
        .catch(() => {});
    }

    const randomColor = () => {
      setPreset(Math.floor(Math.random()*256), Math.floor(Math.random()*256), Math.floor(Math.random()*256));
    }

    let partyInt = null;
    const toggleParty = () => {
      const btn = document.getElementById('btnParty');
      if(partyInt) {
        clearInterval(partyInt);
        partyInt = null;
        btn.style.boxShadow = '';
        btn.style.background = '';
        update();
      } else {
        btn.style.boxShadow = '0 0 25px rgba(255,80,200,0.5)';
        btn.style.background = 'rgba(255,80,200,0.15)';
        partyInt = setInterval(randomColor, 600);
        update();
      }
    }

    const getBatteryClass = (v) => {
      if (v <= 10.5) return 'txt-accent3';
      if (v <= 11.0) return 'txt-warn';
      return 'txt-ok';
    }

    const updateHeatmap = (motionHist) => {
      const cells = document.querySelectorAll('.hcell');
      cells.forEach((cell, i) => {
        const val = motionHist[i] || 0;
        if (val) {
          cell.style.background = `rgba(0, 247, 255, ${0.4 + val * 0.6})`;
          cell.style.boxShadow = `0 0 10px rgba(0,247,255,0.5)`;
        } else {
          cell.style.background = 'rgba(255,255,255,0.03)';
          cell.style.boxShadow = 'none';
        }
      });
    }

    const update = async () => {
      // Update Clock
      document.getElementById('clock').textContent = new Date().toLocaleTimeString('en-US', { 
        hour: '2-digit', minute: '2-digit', second: '2-digit', hour12: true 
      });

      let d;
      try {
        const r = await fetch('/data');
        if (!r.ok) return;
        d = await r.json();
      } catch(e) { return; }

      if (document.activeElement !== document.getElementById('z1slider') && !flashInt) {
        document.getElementById('z1slider').value = d.zone1;
        document.getElementById('val1').textContent = d.zone1;
      }
      if (document.activeElement !== document.getElementById('z2slider') && !flashInt) {
        document.getElementById('z2slider').value = d.zone2;
        document.getElementById('val2').textContent = d.zone2;
      }

      updateRing('ring1', 'lbl1', d.zone1);
      updateRing('ring2', 'lbl2', d.zone2);

      const vEl = document.getElementById('stVoltage');
      vEl.textContent = d.voltage + ' V';
      vEl.className = 'stat-value ' + getBatteryClass(d.voltage);

      document.getElementById('stCurrent').textContent = d.current + ' mA';
      const ldrEl = document.getElementById('stLDR');
      if (d.ldr < 2000) {
        ldrEl.innerHTML = '<span class="stat-icon-inline"><svg width="14" height="14" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><path d="M21 12.79A9 9 0 1 1 11.21 3 7 7 0 0 0 21 12.79z"/></svg>Dark</span>';
        ldrEl.className = 'stat-value txt-accent2';
      } else {
        ldrEl.innerHTML = '<span class="stat-icon-inline"><svg width="14" height="14" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><circle cx="12" cy="12" r="5"/><line x1="12" y1="1" x2="12" y2="3"/><line x1="12" y1="21" x2="12" y2="23"/><line x1="4.22" y1="4.22" x2="5.64" y2="5.64"/><line x1="18.36" y1="18.36" x2="19.78" y2="19.78"/><line x1="1" y1="12" x2="3" y2="12"/><line x1="21" y1="12" x2="23" y2="12"/><line x1="4.22" y1="19.78" x2="5.64" y2="18.36"/><line x1="18.36" y1="5.64" x2="19.78" y2="4.22"/></svg>Bright</span>';
        ldrEl.className = 'stat-value txt-warn';
      }
      document.getElementById('stTemp').textContent = (d.temperature !== undefined ? d.temperature : '--') + ' \u00b0C';
      document.getElementById('stHumid').textContent = (d.humidity !== undefined ? d.humidity : '--') + ' %';

      const weatherEl = document.getElementById('stWeather');
      if (d.raining) {
        weatherEl.innerHTML = '<span class="stat-icon-inline"><svg width="14" height="14" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><path d="M20 16.2A4.5 4.5 0 0 0 17.5 8h-1.8A7 7 0 1 0 4 14.9"/><path d="M16 14v6"/><path d="M8 14v6"/><path d="M12 16v6"/></svg>Raining</span>';
        weatherEl.className = 'stat-value txt-accent';
      } else {
        weatherEl.innerHTML = '<span class="stat-icon-inline"><svg width="14" height="14" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><circle cx="12" cy="12" r="5"/><line x1="12" y1="1" x2="12" y2="3"/><line x1="12" y1="21" x2="12" y2="23"/><line x1="4.22" y1="4.22" x2="5.64" y2="5.64"/><line x1="18.36" y1="18.36" x2="19.78" y2="19.78"/><line x1="1" y1="12" x2="3" y2="12"/><line x1="21" y1="12" x2="23" y2="12"/><line x1="4.22" y1="19.78" x2="5.64" y2="18.36"/><line x1="18.36" y1="5.64" x2="19.78" y2="4.22"/></svg>Clear</span>';
        weatherEl.className = 'stat-value txt-ok';
      }

      const motionDot = document.getElementById('motionDot');
      const motionTxt = document.getElementById('stMotion');
      if (d.motion) {
        motionDot.classList.add('active');
        motionTxt.textContent = 'Detected';
      } else {
        motionDot.classList.remove('active');
        motionTxt.textContent = 'None';
      }

      const badge = document.getElementById('modeBadge');
      if (!onAnimTimeout) {
        if (partyInt) {
          badge.innerHTML = '<svg width="14" height="14" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><path d="M12 2v4M12 18v4M4.93 4.93l2.83 2.83M16.24 16.24l2.83 2.83M2 12h4M18 12h4M4.93 19.07l2.83-2.83M16.24 7.76l2.83-2.83"/></svg> PARTY';
          badge.className = 'mode-badge mode-party';
        } else if (d.mode === 'auto') {
          badge.innerHTML = '<svg width="14" height="14" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><polyline points="23 4 23 10 17 10"/><polyline points="1 20 1 14 7 14"/><path d="M3.51 9a9 9 0 0 1 14.85-3.36L23 10M1 14l4.64 4.36A9 9 0 0 0 20.49 15"/></svg> AUTO';
          badge.className = 'mode-badge mode-auto';
        } else {
          // If in override mode, check if target brightness is 0 for "OFF"
          if (d.zone1 == 0 && d.zone2 == 0) {
            badge.innerHTML = '<svg width="14" height="14" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><path d="M18.36 6.64a9 9 0 1 1-12.73 0M12 2v10"/></svg> OFF';
            badge.className = 'mode-badge mode-off';
          } else {
            badge.innerHTML = '<svg width="14" height="14" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><path d="M10.29 3.86L1.82 18a2 2 0 0 0 1.71 3h16.94a2 2 0 0 0 1.71-3L13.71 3.86a2 2 0 0 0-3.42 0z"/><line x1="12" y1="9" x2="12" y2="13"/><line x1="12" y1="17" x2="12.01" y2="17"/></svg> OVERRIDE';
            badge.className = 'mode-badge mode-override';
          }
        }
      }

      const btnGlow = document.getElementById('btnGlow');
      glowEnabled = d.softGlow === 1;
      btnGlow.classList.toggle('active', glowEnabled);

      if (document.activeElement !== document.getElementById('cr') &&
          document.activeElement !== document.getElementById('cg') &&
          document.activeElement !== document.getElementById('cb')) {
        document.getElementById('cr').value = d.glowR;
        document.getElementById('cg').value = d.glowG;
        document.getElementById('cb').value = d.glowB;
        onColorChange();
      }

      drawLineChart('vChart', d.voltageHistory, 'rgb(0,247,255)', 8, 14);
      drawLineChart('mChart', d.motionHistory, 'rgb(123,92,255)', 0, 1);
      updateHeatmap(d.motionHistory);
    }

    update();
    setInterval(update, 1000);
    
    window.addEventListener('load', () => {
      setTimeout(() => {
        const loader = document.getElementById('loadingScreen');
        if (loader) {
          loader.style.opacity = '0';
          loader.style.visibility = 'hidden';
          setTimeout(() => loader.remove(), 600);
        }
      }, 700);
    });
  </script>
</body>
</html>
"""

dashboard_h = f"""#ifndef DASHBOARD_H
#define DASHBOARD_H

const char DASHBOARD_HTML[] PROGMEM = R"rawhtml(
{head_and_body.strip()}
{esp32_js}
)rawhtml";

#endif
"""

dashboard_h = dashboard_h.replace('Autonomous Control Dashboard (Demo Mode)', 'Autonomous Control Dashboard')

with open('dashboard.h', 'w', encoding='utf-8') as f:
    f.write(dashboard_h)

print('dashboard.h generated successfully')
