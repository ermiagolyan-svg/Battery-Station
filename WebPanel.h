#ifndef WEB_PANEL_H
#define WEB_PANEL_H

const char INDEX_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html><html lang="fa" dir="rtl">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width,initial-scale=1,viewport-fit=cover">
<title>ایستگاه مانیتورینگ هوشمند BMS</title>
<style>
 *{box-sizing:border-box;margin:0;padding:0;font-family:-apple-system,BlinkMacSystemFont,'Segoe UI',Roboto,sans-serif;}
 body{background:#060813;color:#f1f5f9;min-height:100vh;padding:20px 10px;}
 .container{max-width:500px;margin:0 auto;}
 .header{text-align:center;font-size:22px;font-weight:800;background:linear-gradient(135deg,#38bdf8,#818cf8);-webkit-background-clip:text;-webkit-text-fill-color:transparent;padding:10px 0 20px;}
 .card{background:rgba(15,23,42,0.6);backdrop-filter:blur(12px);border:1px solid rgba(255,255,255,0.06);border-radius:24px;padding:20px;margin-bottom:16px;box-shadow:0 15px 35px rgba(0,0,0,0.4);}
 .card-title{font-size:13px;color:#94a3b8;font-weight:700;margin-bottom:15px;display:flex;align-items:center;gap:8px;border-bottom:1px solid rgba(255,255,255,0.05);padding-bottom:8px;}
 
 .flow-grid {display:grid;grid-template-columns:1fr auto 1fr auto 1fr;align-items:center;text-align:center;gap:4px;margin:10px 0;}
 .flow-node {background:#1e293b;padding:10px 6px;border-radius:12px;font-size:11px;font-weight:600;border:1px solid rgba(255,255,255,0.05);}
 .flow-node span {display:block;font-size:13px;color:#fff;margin-top:4px;font-family:monospace;}
 .flow-arrow {font-size:16px;color:#475569;transition:color 0.3s;}
 .flow-arrow.active {color:#10b981;animation:blink 1s infinite;}
 @keyframes blink {50% {opacity: 0.4;}}

 .bat-bar{background:#1e293b;border-radius:14px;height:28px;overflow:hidden;position:relative;border:1px solid #334155;}
 .bat-fill{height:100%;width:0%;border-radius:13px;transition:width 0.8s cubic-bezier(0.4, 0, 0.2, 1), background 0.5s;}
 .bat-text{position:absolute;left:50%;top:50%;transform:translate(-50%,-50%);font-size:14px;font-weight:800;color:#fff;text-shadow:0 1px 4px rgba(0,0,0,0.8);}
 
 .stats-grid{display:grid;grid-template-columns:1fr 1fr;gap:12px;}
 .stat-box{background:rgba(30,41,59,0.4);border-radius:16px;padding:14px;border:1px solid rgba(255,255,255,0.03);}
 .stat-value{font-size:24px;font-weight:800;color:#fff;font-family:monospace;}
 .stat-label{font-size:11px;color:#64748b;margin-top:4px;font-weight:600;}
 
 .ctrl-row{display:flex;justify-content:space-between;align-items:center;padding:12px 4px;border-bottom:1px solid rgba(255,255,255,0.04);}
 .ctrl-row:last-child{border-bottom:none;}
 .switch{position:relative;display:inline-block;width:50px;height:26px;}
 .switch input{opacity:0;width:0;height:0;}
 .slider{position:absolute;cursor:pointer;top:0;left:0;right:0;bottom:0;background-color:#334155;transition:.3s;border-radius:34px;}
 .slider:before{position:absolute;content:"";height:20px;width:20px;left:3px;bottom:3px;background-color:#fff;transition:.3s;border-radius:50%;}
 input:checked + .slider{background-color:#3b82f6;}
 input:checked + .slider:before{transform:translateX(24px);}
 
 .time-badge {color:#fff;padding:8px 12px;border-radius:12px;font-size:12px;font-weight:bold;text-align:center;margin-top:10px;transition: background 0.3s;}
 .footer{text-align:center;font-size:11px;color:#475569;margin-top:25px;}

 input[type=range] { -webkit-appearance: none; width: 100%; background: transparent; }
 input[type=range]::-webkit-slider-thumb { -webkit-appearance: none; height: 18px; width: 18px; border-radius: 50%; background: #38bdf8; cursor: pointer; margin-top: -6px; }
 input[type=range]::-webkit-slider-runnable-track { width: 100%; height: 6px; cursor: pointer; background: #334155; border-radius: 3px; }
</style>
</head>
<body>
<div class="container">
 <div class="header">⚡ سیستم پایش پیشرفته BMS PRO</div>

 <div class="card">
  <div class="card-title">🌐 دیاگرام زنده جریان توان سیستم</div>
  <div class="flow-grid">
   <div class="flow-node">ورودی DC (Ch2)<span id="f_in">0.0W</span></div>
   <div class="flow-arrow" id="a_in">➔</div>
   <div class="flow-node">ماژول شارژر<span id="ip_txt">--</span></div>
   <div class="flow-arrow" id="a_bat">➔</div>
   <div class="flow-node">باتری پک (Ch1)<span id="f_bat">0.0W</span></div>
  </div>
 </div>

 <div class="card">
  <div class="card-title">🔋 مخزن انرژی سلول‌ها (Ch1)</div>
  <div class="bat-bar">
   <div class="bat-fill" id="batFill"></div>
   <div class="bat-text" id="batPct">-- %</div>
  </div>
  <div class="time-badge" id="timeEstimate">در حال محاسبه وضعیت...</div>
 </div>
 
 <div class="card">
  <div class="card-title">⚙️ کنترل‌گر هوشمند شارژ (IP2365)</div>
  <div class="ctrl-row">
   <span>وضعیت پایش I2C</span>
   <span id="ip_status_txt" style="font-size:12px; font-weight:bold; color:#38bdf8;">در حال بررسی...</span>
  </div>
  <div class="ctrl-row">
   <span>کنترل اتوماتیک حرارتی (Auto)</span>
   <label class="switch"><input type="checkbox" id="chkAuto" onchange="setCharger()"><span class="slider"></span></label>
  </div>
  <div class="ctrl-row" style="flex-direction:column; align-items:flex-start;">
   <div style="display:flex; justify-content:space-between; width:100%; margin-bottom:12px;">
    <span>تنظیم جریان (حالت دستی)</span>
    <span id="curVal" style="font-weight:bold; font-family:monospace; font-size:16px;">1.15 A</span>
   </div>
   <input type="range" id="curSlider" min="0.1" max="2.0" step="0.1" value="1.15" onchange="setCharger()" oninput="document.getElementById('curVal').innerText = parseFloat(this.value).toFixed(2) + ' A'">
  </div>
 </div>

 <div class="card">
  <div class="card-title">🌡️ وضعیت حرارتی سیستم</div>
  <div class="stats-grid">
   <div class="stat-box">
    <div class="stat-value" id="t_b1">--</div><div class="stat-label">دمای بانک ۱ (°C)</div>
   </div>
   <div class="stat-box">
    <div class="stat-value" id="t_b2">--</div><div class="stat-label">دمای بانک ۲ (°C)</div>
   </div>
   <div class="stat-box" style="grid-column: 1/-1; background: rgba(239,68,68,0.05); border: 1px solid rgba(239,68,68,0.15)">
    <div class="stat-value" id="t_chg" style="color:#ef4444;">--</div><div class="stat-label" style="color:#fca5a5;">دمای شارژر / هیت‌سینک (°C)</div>
   </div>
  </div>
 </div>

 <div class="card">
  <div class="card-title">🔌 پارامترهای برق مستقیم ورودی (Ch2)</div>
  <div class="stats-grid">
   <div class="stat-box">
    <div class="stat-value" id="v2">--</div><div class="stat-label">ولتاژ ورودی (V)</div>
   </div>
   <div class="stat-box">
    <div class="stat-value" id="i2">--</div><div class="stat-label">جریان ورودی (A)</div>
   </div>
  </div>
 </div>

 <div class="card">
  <div class="card-title">📱 خروجی شارژر موبایل (Ch3)</div>
  <div class="stats-grid">
   <div class="stat-box">
    <div class="stat-value" id="v3">--</div><div class="stat-label">ولتاژ شارژر (V)</div>
   </div>
   <div class="stat-box">
    <div class="stat-value" id="i3">--</div><div class="stat-label">جریان خروجی شارژر (A)</div>
   </div>
  </div>
 </div>

 <div class="card">
  <div class="card-title">📈 دیتای پک باتری و رله‌ها (Ch1)</div>
  <div class="stats-grid" style="margin-bottom: 15px;">
   <div class="stat-box">
    <div class="stat-value" id="v1">--</div><div class="stat-label">ولتاژ لحظه‌ای پک (V)</div>
   </div>
   <div class="stat-box">
    <div class="stat-value" id="i1">--</div><div class="stat-label">جریان (شارژ/دیسشارژ) (A)</div>
   </div>
   <div class="stat-box" style="grid-column:1/-1">
    <div class="stat-value" id="mah" style="color:#38bdf8;">--</div><div class="stat-label">ظرفیت مصرف شده (mAh)</div>
   </div>
  </div>
  
  <div class="ctrl-row">
   <span>💡 مدار خروجی اول (Load 1)</span>
   <label class="switch"><input type="checkbox" id="chk1" onchange="toggle(1)"><span class="slider"></span></label>
  </div>
  <div class="ctrl-row">
   <span>💡 مدار خروجی دوم (Load 2)</span>
   <label class="switch"><input type="checkbox" id="chk2" onchange="toggle(2)"><span class="slider"></span></label>
  </div>
 </div>

 <div class="footer">سیستم مدیریت هوشمند هوبر پلاس · آی‌سی پایش INA3221</div>
</div>

<script>
function getBatColor(p){
 if(p<20) return '#ef4444';
 if(p<55) return '#f59e0b';
 return '#10b981';
}

function formatTime(mins) {
 if(mins < 0) return "محاسبه پایداری...";
 let h = Math.floor(mins / 60);
 let m = Math.round(mins % 60);
 return (h > 0 ? h + " ساعت و " : "") + m + " دقیقه باقیمانده";
}

function setCharger() {
 let isAuto = document.getElementById('chkAuto').checked ? 1 : 0;
 let val = document.getElementById('curSlider').value;
 fetch('/chargerConfig?auto=' + isAuto + '&current=' + val).then(()=>loadData());
}

function loadData(){
 fetch('/data').then(r=>r.json()).then(d=>{
  document.getElementById('v1').textContent = d.v1.toFixed(2);
  document.getElementById('i1').textContent = d.i1.toFixed(2);
  document.getElementById('f_bat').textContent = d.p1.toFixed(1) + "W";
  document.getElementById('mah').textContent = Math.round(d.mah);
  
  document.getElementById('v2').textContent = d.v2.toFixed(2);
  document.getElementById('i2').textContent = d.i2.toFixed(2);
  document.getElementById('f_in').textContent = d.p2.toFixed(1) + "W";

  document.getElementById('v3').textContent = d.v3.toFixed(2);
  document.getElementById('i3').textContent = d.i3.toFixed(2);

  document.getElementById('t_b1').textContent = d.tb1.toFixed(1);
  document.getElementById('t_b2').textContent = d.tb2.toFixed(1);
  document.getElementById('t_chg').textContent = d.tchg.toFixed(1);

  document.getElementById('batPct').textContent = d.pct.toFixed(0)+' %';
  var fill = document.getElementById('batFill');
  fill.style.width = d.pct+'%';
  fill.style.background = getBatColor(d.pct);
  
  var badge = document.getElementById('timeEstimate');
  if(d.dc_in === 1) {
    if(d.charging === 1) {
      badge.textContent = "⏳ اتصال برق مستقیم | در حال شارژ باتری: " + formatTime(d.t_min);
      badge.style.background = "#1e3a8a"; 
    } else {
      badge.textContent = "🔌 حالت UPS فعال | تغذیه مستقیم بار (بای‌پاس و استراحت باتری)";
      badge.style.background = "#065f46"; 
    }
  } else {
    badge.textContent = "🔋 دیسشارژ؛ تامین انرژی از باتری: " + formatTime(d.t_min);
    badge.style.background = "#991b1b"; 
  }

  document.getElementById('a_in').className = (d.p2 > 0.5) ? 'flow-arrow active' : 'flow-arrow';
  document.getElementById('a_bat').className = (d.charging === 1) ? 'flow-arrow active' : 'flow-arrow';

  document.getElementById('chk1').checked = d.r1 === 1;
  document.getElementById('chk2').checked = d.r2 === 1;

  document.getElementById('chkAuto').checked = (d.ip_mod === 1);
  if(document.activeElement !== document.getElementById('curSlider')) {
     document.getElementById('curSlider').value = d.ip_cur;
     document.getElementById('curVal').innerText = parseFloat(d.ip_cur).toFixed(2) + ' A';
  }

  let ipStTxt = "آفلاین / قطعی برق";
  document.getElementById('ip_txt').innerText = "OFF";
  if (d.ip_on === 1) {
     if (d.ip_st === 2) { ipStTxt = "✅ فول شارژ (16.4V)"; document.getElementById('ip_txt').innerText = "FULL"; }
     else if (d.ip_st === 1) { ipStTxt = "⏳ توقف موقت (سنجش افت ولتاژ)"; document.getElementById('ip_txt').innerText = "MEASURE"; }
     else { ipStTxt = "⚡ تزریق جریان (" + d.ip_tgt.toFixed(2) + "A)"; document.getElementById('ip_txt').innerText = "ACTIVE"; }
  }
  document.getElementById('ip_status_txt').innerText = ipStTxt;

 });
}

function toggle(n){fetch('/relay?ch='+n).then(()=>loadData());}
window.addEventListener('load', loadData);
setInterval(loadData, 1500);
</script>
</body></html>
)rawliteral";

#endif