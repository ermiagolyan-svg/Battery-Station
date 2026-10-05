#ifndef WEB_PANEL_H
#define WEB_PANEL_H

const char INDEX_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="fa" dir="rtl">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1, viewport-fit=cover, maximum-scale=1, user-scalable=no">
  <title>ایستگاه مانیتورینگ هوشمند BMS</title>
  <link href="https://cdn.jsdelivr.net/gh/rastikerdar/vazirmatn@v33.0.0/Vazirmatn-font-face.css" rel="stylesheet" type="text/css" />
  <link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/6.4.0/css/all.min.css">
  
  <style>
    * { box-sizing: border-box; margin: 0; padding: 0; font-family: 'Vazirmatn', sans-serif; }
    body {
      background: #0b0f19; color: #f1f5f9; min-height: 100vh; padding: 15px;
      background-image: radial-gradient(circle at 15% 50%, rgba(56, 189, 248, 0.05), transparent 25%),
                        radial-gradient(circle at 85% 30%, rgba(129, 140, 248, 0.05), transparent 25%);
    }
    .container { max-width: 500px; margin: 0 auto; }
    
    .header {
      text-align: center; font-size: 20px; font-weight: 800;
      background: linear-gradient(135deg, #38bdf8, #818cf8);
      -webkit-background-clip: text; -webkit-text-fill-color: transparent;
      padding: 10px 0 20px; letter-spacing: -0.5px;
    }
    
    .card {
      background: rgba(15, 23, 42, 0.6); backdrop-filter: blur(16px); -webkit-backdrop-filter: blur(16px);
      border: 1px solid rgba(255, 255, 255, 0.08); border-radius: 20px; padding: 18px; margin-bottom: 16px;
      box-shadow: 0 10px 30px rgba(0, 0, 0, 0.5), inset 0 1px 0 rgba(255,255,255,0.05);
      transition: transform 0.3s ease, box-shadow 0.3s ease;
    }
    .card-title {
      font-size: 14px; color: #94a3b8; font-weight: 700; margin-bottom: 15px;
      display: flex; align-items: center; gap: 8px; border-bottom: 1px solid rgba(255, 255, 255, 0.05); padding-bottom: 10px;
    }
    .card-title i { color: #38bdf8; }
    
    .relay-container { display: flex; gap: 10px; margin-bottom: 16px; }
    .relay-btn {
      flex: 1; background: rgba(30, 41, 59, 0.5); border: 1px solid rgba(255, 255, 255, 0.05);
      border-radius: 16px; padding: 15px 10px; display: flex; flex-direction: column;
      align-items: center; justify-content: center; gap: 8px; cursor: pointer; transition: all 0.3s ease;
      box-shadow: inset 0 2px 4px rgba(255,255,255,0.02), 0 4px 10px rgba(0,0,0,0.3);
      -webkit-tap-highlight-color: transparent;
    }
    .relay-btn:active { transform: scale(0.95); }
    .relay-btn.active {
      background: rgba(56, 189, 248, 0.15); border-color: rgba(56, 189, 248, 0.5);
      box-shadow: inset 0 2px 10px rgba(56,189,248,0.2), 0 0 15px rgba(56,189,248,0.2);
    }
    .relay-icon { font-size: 24px; color: #64748b; transition: all 0.3s ease; }
    .relay-btn.active .relay-icon { color: #eab308; text-shadow: 0 0 10px rgba(234, 179, 8, 0.6); }
    .relay-label { font-size: 12px; font-weight: 600; color: #cbd5e1; }
    .relay-btn.active .relay-label { color: #38bdf8; }

    .bat-container { display: flex; align-items: center; justify-content: center; gap: 15px; margin: 10px 0; }
    .bat-body {
      width: 120px; height: 60px; border: 3px solid #475569; border-radius: 12px; position: relative;
      padding: 3px; background: rgba(15, 23, 42, 0.8); box-shadow: inset 0 0 10px rgba(0,0,0,0.5); overflow: hidden;
    }
    .bat-body::after {
      content: ''; position: absolute; right: -8px; top: 50%; transform: translateY(-50%);
      width: 6px; height: 20px; background: #475569; border-radius: 0 4px 4px 0;
    }
    .bat-fill {
      height: 100%; width: 0%; border-radius: 6px; 
      transition: width 1s cubic-bezier(0.4, 0, 0.2, 1), background 0.5s, border-radius 0.3s;
      position: relative; overflow: hidden;
    }
    .bat-fill::before {
      content: ''; position: absolute; top: 0; left: 0; right: 0; bottom: 0;
      background: linear-gradient(90deg, rgba(255,255,255,0) 0%, rgba(255,255,255,0.2) 50%, rgba(255,255,255,0) 100%);
      animation: shimmer 2s infinite linear;
    }
    @keyframes shimmer { 0% { transform: translateX(-100%); } 100% { transform: translateX(100%); } }
    .bat-text-large { font-size: 28px; font-weight: 800; font-family: monospace; color: #fff; display: flex; align-items: baseline; }
    .bat-text-large span { font-size: 14px; color: #94a3b8; margin-right: 4px; }
    
    .flow-diagram { width: 100%; height: 80px; position: relative; margin: 10px 0; }
    .flow-node {
      position: absolute; background: #1e293b; border: 1px solid rgba(255,255,255,0.1);
      border-radius: 12px; padding: 8px 5px; text-align: center; width: 80px; z-index: 2;
      box-shadow: 0 4px 6px rgba(0,0,0,0.3);
    }
    .flow-node-title { font-size: 10px; color: #94a3b8; }
    .flow-node-val { font-size: 12px; font-weight: bold; color: #fff; font-family: monospace; margin-top: 2px;}
    #node-in { left: 0; top: 50%; transform: translateY(-50%); }
    #node-chg { left: 50%; top: 50%; transform: translate(-50%, -50%); border-color: #38bdf8; }
    #node-bat { right: 0; top: 50%; transform: translateY(-50%); }
    .flow-svg { position: absolute; top: 0; left: 0; width: 100%; height: 100%; z-index: 1; pointer-events: none; }
    .flow-line { stroke: #334155; stroke-width: 3; fill: none; stroke-linecap: round; }
    .flow-particle {
      stroke: #10b981; stroke-width: 3; fill: none; stroke-linecap: round; stroke-dasharray: 4, 12;
      animation: flowAnim 1s linear infinite; opacity: 0; transition: opacity 0.3s, stroke 0.3s;
    }
    .flow-particle.active { opacity: 1; }
    @keyframes flowAnim { from { stroke-dashoffset: 16; } to { stroke-dashoffset: 0; } }
    
    .stats-grid { display: grid; grid-template-columns: 1fr 1fr; gap: 12px; }
    @media (max-width: 340px) { .stats-grid { grid-template-columns: 1fr; } }
    
    .stat-box {
      background: rgba(30, 41, 59, 0.4); border-radius: 16px; padding: 12px;
      border: 1px solid rgba(255, 255, 255, 0.03); display: flex; flex-direction: column; justify-content: center;
    }
    .stat-value { font-size: 20px; font-weight: 800; color: #fff; font-family: monospace; margin-bottom: 4px; }
    .stat-label { font-size: 11px; color: #64748b; font-weight: 600; }
    
    .ctrl-row { display: flex; justify-content: space-between; align-items: center; padding: 12px 4px; border-bottom: 1px solid rgba(255, 255, 255, 0.04); }
    .ctrl-row:last-child { border-bottom: none; }
    
    .switch { position: relative; display: inline-block; width: 46px; height: 24px; }
    .switch input { opacity: 0; width: 0; height: 0; }
    .slider {
      position: absolute; cursor: pointer; top: 0; left: 0; right: 0; bottom: 0;
      background-color: #0f172a; transition: .3s; border-radius: 24px; border: 1px solid #334155;
      box-shadow: inset 0 2px 4px rgba(0,0,0,0.5);
    }
    .slider:before {
      position: absolute; content: ""; height: 18px; width: 18px; left: 2px; bottom: 2px;
      background: linear-gradient(180deg, #e2e8f0, #94a3b8); transition: .3s; border-radius: 50%; box-shadow: 0 2px 4px rgba(0,0,0,0.3);
    }
    input:checked + .slider { background-color: rgba(56, 189, 248, 0.2); border-color: #38bdf8; }
    input:checked + .slider:before { transform: translateX(22px); background: #38bdf8; box-shadow: 0 0 8px rgba(56,189,248,0.8); }
    
    input[type=range] { -webkit-appearance: none; width: 100%; background: transparent; margin-top: 10px; }
    input[type=range]::-webkit-slider-thumb {
      -webkit-appearance: none; height: 24px; width: 24px; border-radius: 50%;
      background: #38bdf8; border: 3px solid #0f172a; cursor: pointer; margin-top: -8px; box-shadow: 0 0 10px rgba(56,189,248,0.4);
    }
    input[type=range]::-webkit-slider-runnable-track {
      width: 100%; height: 8px; cursor: pointer; background: #1e293b; border-radius: 4px;
      box-shadow: inset 0 1px 3px rgba(0,0,0,0.5); border: 1px solid rgba(255,255,255,0.05);
    }
    
    .time-badge {
      color: #fff; padding: 10px 12px; border-radius: 12px; font-size: 11px; font-weight: bold;
      text-align: center; margin-top: 15px; transition: background 0.3s; border: 1px solid rgba(255,255,255,0.1);
    }
    .footer { text-align: center; font-size: 11px; color: #475569; margin-top: 25px; margin-bottom: 20px;}
  </style>
</head>
<body>

<div class="container">
  <div class="header">⚡ BMS PRO MONITOR</div>

  <div class="relay-container">
    <div class="relay-btn" id="btnRelay1" onclick="toggleRelay(1)">
      <i class="fa-solid fa-lightbulb relay-icon" id="iconR1"></i>
      <div class="relay-label">خروجی اول (L1)</div>
    </div>
    <div class="relay-btn" id="btnRelay2" onclick="toggleRelay(2)">
      <i class="fa-solid fa-lightbulb relay-icon" id="iconR2"></i>
      <div class="relay-label">خروجی دوم (L2)</div>
    </div>
  </div>

  <div class="card">
    <div class="card-title"><i class="fa-solid fa-battery-half"></i> وضعیت مخزن انرژی (Ch1)</div>
    <div class="bat-container">
        <div class="bat-text-large" id="batPctText">--<span>%</span></div>
        <div class="bat-body">
            <div class="bat-fill" id="batFill"></div>
        </div>
    </div>
    <div class="time-badge" id="timeEstimate">در حال محاسبه وضعیت...</div>
  </div>

  <div class="card">
    <div class="card-title"><i class="fa-solid fa-network-wired"></i> گراف جریان توان زنده</div>
    <div class="flow-diagram">
        <svg class="flow-svg">
            <line x1="15%" y1="50%" x2="45%" y2="50%" class="flow-line" />
            <line x1="15%" y1="50%" x2="45%" y2="50%" class="flow-particle" id="flow-in" />
            <line x1="55%" y1="50%" x2="85%" y2="50%" class="flow-line" />
            <line x1="55%" y1="50%" x2="85%" y2="50%" class="flow-particle" id="flow-bat" />
        </svg>
        <div class="flow-node" id="node-in">
            <div class="flow-node-title">ورودی DC</div>
            <div class="flow-node-val" id="f_in">0.0W</div>
        </div>
        <div class="flow-node" id="node-chg">
            <div class="flow-node-title">شارژر</div>
            <div class="flow-node-val" id="ip_txt">--</div>
        </div>
        <div class="flow-node" id="node-bat">
            <div class="flow-node-title">باتری پک</div>
            <div class="flow-node-val" id="f_bat">0.0W</div>
        </div>
    </div>
  </div>

  <div class="card">
    <div class="card-title"><i class="fa-solid fa-microchip"></i> کنترل‌گر توان (IP2365)</div>
    <div class="ctrl-row">
      <span>وضعیت پایش:</span>
      <span id="ip_status_txt" style="font-size:12px; font-weight:bold; color:#38bdf8; background:rgba(56,189,248,0.1); padding:4px 8px; border-radius:8px;">بررسی...</span>
    </div>
    <div class="ctrl-row">
      <span>مدیریت حرارتی اتوماتیک</span>
      <label class="switch"><input type="checkbox" id="chkAuto" onchange="setCharger()"><span class="slider"></span></label>
    </div>
    <div class="ctrl-row" style="flex-direction:column; align-items:flex-start;">
      <div style="display:flex; justify-content:space-between; width:100%;">
        <span style="font-size:13px; color:#cbd5e1;">محدودیت جریان شارژ</span>
        <span id="curVal" style="font-weight:bold; font-family:monospace; font-size:16px; color:#38bdf8;">1.15 A</span>
      </div>
      <input type="range" id="curSlider" min="0.1" max="3.0" step="0.1" value="1.15" onchange="setCharger()" oninput="document.getElementById('curVal').innerText = parseFloat(this.value).toFixed(2) + ' A'">
    </div>
  </div>

  <div class="card">
    <div class="card-title"><i class="fa-solid fa-temperature-half"></i> وضعیت حرارتی (°C)</div>
    <div class="stats-grid">
      <div class="stat-box">
        <div class="stat-value" id="t_b1">--</div><div class="stat-label">سنسور بانک ۱</div>
      </div>
      <div class="stat-box">
        <div class="stat-value" id="t_b2">--</div><div class="stat-label">سنسور بانک ۲</div>
      </div>
      <div class="stat-box" style="grid-column: 1/-1; background: rgba(239,68,68,0.05); border-color: rgba(239,68,68,0.15)">
        <div class="stat-value" id="t_chg" style="color:#ef4444;">--</div><div class="stat-label" style="color:#fca5a5;">دمای هیت‌سینک شارژر</div>
      </div>
    </div>
  </div>

  <div class="card">
    <div class="card-title"><i class="fa-solid fa-bolt"></i> مانیتورینگ ولتاژ / جریان</div>
    
    <div style="font-size: 12px; color: #94a3b8; margin-bottom: 8px;">ورودی آداپتور (DC IN)</div>
    <div class="stats-grid" style="margin-bottom: 15px;">
      <div class="stat-box">
        <div class="stat-value" id="v2">--</div><div class="stat-label">ولتاژ (V)</div>
      </div>
      <div class="stat-box">
        <div class="stat-value" id="i2">--</div><div class="stat-label">جریان (A)</div>
      </div>
    </div>

    <div style="font-size: 12px; color: #94a3b8; margin-bottom: 8px;">پک باتری لیتیوم (BAT)</div>
    <div class="stats-grid" style="margin-bottom: 15px;">
      <div class="stat-box">
        <div class="stat-value" id="v1">--</div><div class="stat-label">ولتاژ (V)</div>
      </div>
      <div class="stat-box">
        <div class="stat-value" id="i1">--</div><div class="stat-label">جریان (A)</div>
      </div>
    </div>
    
    <div style="font-size: 12px; color: #94a3b8; margin-bottom: 8px;">خروجی رگولاتور موبایل (USB)</div>
    <div class="stats-grid">
      <div class="stat-box">
        <div class="stat-value" id="v3">--</div><div class="stat-label">ولتاژ (V)</div>
      </div>
      <div class="stat-box">
        <div class="stat-value" id="i3">--</div><div class="stat-label">جریان (A)</div>
      </div>
    </div>
  </div>

  <div class="footer">Huber Plus BMS Engine · V2 UI</div>
</div>

<script>
function getBatColor(p){
  if(p < 20) return 'linear-gradient(90deg, #ef4444, #b91c1c)'; 
  if(p < 55) return 'linear-gradient(90deg, #f59e0b, #d97706)'; 
  return 'linear-gradient(90deg, #10b981, #059669)'; 
}

function formatTime(mins) {
  if(mins < 0) return "تحلیل پایداری...";
  let h = Math.floor(mins / 60);
  let m = Math.round(mins % 60);
  return (h > 0 ? h + " ساعت و " : "") + m + " دقیقه باقیمانده";
}

function toggleRelay(ch) {
  if (navigator.vibrate) navigator.vibrate(50);
  fetch('/relay?ch=' + ch).then(()=>loadData());
  
  // پیش‌نمایش سریع استاتوس
  const btn = document.getElementById('btnRelay' + ch);
  btn.classList.toggle('active');
}

function setCharger() {
  if (navigator.vibrate) navigator.vibrate(50);
  let isAuto = document.getElementById('chkAuto').checked ? 1 : 0;
  let val = document.getElementById('curSlider').value;
  fetch('/chargerConfig?auto=' + isAuto + '&current=' + val).then(()=>loadData());
}

function loadData(){
  fetch('/data').then(r=>r.json()).then(d=>{
    document.getElementById('v1').textContent = d.v1.toFixed(2);
    document.getElementById('i1').textContent = d.i1.toFixed(2);
    document.getElementById('f_bat').textContent = d.p1.toFixed(1) + "W";
    
    document.getElementById('v2').textContent = d.v2.toFixed(2);
    document.getElementById('i2').textContent = d.i2.toFixed(2);
    document.getElementById('f_in').textContent = d.p2.toFixed(1) + "W";

    document.getElementById('v3').textContent = d.v3.toFixed(2);
    document.getElementById('i3').textContent = d.i3.toFixed(2);

    document.getElementById('t_b1').textContent = d.tb1.toFixed(1);
    document.getElementById('t_b2').textContent = d.tb2.toFixed(1);
    document.getElementById('t_chg').textContent = d.tchg.toFixed(1);

    document.getElementById('batPctText').innerHTML = d.pct.toFixed(0) + '<span>%</span>';
    var fill = document.getElementById('batFill');
    fill.style.width = d.pct + '%';
    fill.style.background = getBatColor(d.pct);
    fill.style.borderRadius = d.pct > 95 ? "6px 4px 4px 6px" : "6px";
    
    var badge = document.getElementById('timeEstimate');
    if(d.dc_in === 1) {
      if(d.charging === 1) {
        badge.textContent = "⏳ آداپتور متصل | در حال شارژ: " + formatTime(d.t_min);
        badge.style.background = "rgba(30, 58, 138, 0.5)"; 
        badge.style.borderColor = "rgba(56, 189, 248, 0.3)";
      } else {
        badge.textContent = "🔌 حالت UPS | باتری در استراحت (بای‌پاس)";
        badge.style.background = "rgba(6, 95, 70, 0.5)";  
        badge.style.borderColor = "rgba(16, 185, 129, 0.3)";
      }
    } else {
      badge.textContent = "🔋 دیسشارژ | زمان باقیمانده باتری: " + formatTime(d.t_min);
      badge.style.background = "rgba(153, 27, 27, 0.5)";  
      badge.style.borderColor = "rgba(239, 68, 68, 0.3)";
    }

    let flowIn = document.getElementById('flow-in');
    let flowBat = document.getElementById('flow-bat');
    
    if (d.p2 > 0.5) { flowIn.classList.add('active'); } else { flowIn.classList.remove('active'); }
    if (d.charging === 1 || d.i1 < -0.2) { 
        flowBat.classList.add('active'); 
        if(d.i1 < -0.2) flowBat.style.stroke = "#f59e0b"; 
        else flowBat.style.stroke = "#10b981"; 
    } else { 
        flowBat.classList.remove('active'); 
    }

    document.getElementById('btnRelay1').className = (d.r1 === 1) ? 'relay-btn active' : 'relay-btn';
    document.getElementById('btnRelay2').className = (d.r2 === 1) ? 'relay-btn active' : 'relay-btn';

    document.getElementById('chkAuto').checked = (d.ip_mod === 1);
    if(document.activeElement !== document.getElementById('curSlider')) {
       document.getElementById('curSlider').value = d.ip_cur;
       document.getElementById('curVal').innerText = parseFloat(d.ip_cur).toFixed(2) + ' A';
    }

    let ipStTxt = "آفلاین / قطعی برق";
    document.getElementById('ip_txt').innerText = "OFF";
    if (d.ip_on === 1) {
       if (d.ip_st === 2) { 
           ipStTxt = "✅ فول شارژ (Float)"; 
           document.getElementById('ip_txt').innerText = "FULL"; 
       }
       else if (d.ip_st === 1) { 
           ipStTxt = "⏳ توقف (سنجش ولتاژ)"; 
           document.getElementById('ip_txt').innerText = "MEASURE"; 
       }
       else { 
           ipStTxt = "⚡ تزریق (" + d.ip_tgt.toFixed(2) + "A)"; 
           document.getElementById('ip_txt').innerText = "ACTIVE"; 
       }
    }
    document.getElementById('ip_status_txt').innerText = ipStTxt;
  });
}

window.addEventListener('load', loadData);
setInterval(loadData, 1500);
</script>
</body>
</html>
)rawliteral";

#endif