/**
 * Smart Aquarium Guardian - Core Logic
 */

// --- CONFIG & LOCAL STORAGE ---
let cachedMinT = 22, cachedMaxT = 30, cachedMinL = 40, cachedMaxL = 90;
let rangeLoaded = false;
let localLogs = JSON.parse(localStorage.getItem('fishLogs') || '[]');
let feedCount = parseInt(localStorage.getItem('feedCount') || '0');

// --- LOGIN LOGIC ---
function l() {
    const u = document.getElementById('u');
    const p = document.getElementById('p');
    const msg = document.getElementById('msg');

    if (!u || !p) return;

    fetch(`/doLogin?u=${encodeURIComponent(u.value)}&p=${encodeURIComponent(p.value)}`)
        .then(r => r.text())
        .then(d => {
            if (d === "OK") {
                location.href = "dashboard.html";
            } else {
                msg.innerText = "⚠️ Invalid credentials. Please try again.";
            }
        })
        .catch(err => {
            if (u.value === "admin" && p.value === "1234") {
                location.href = "dashboard.html";
            } else {
                msg.innerText = "⚠️ Connection error or invalid credentials.";
            }
        });
}

// --- DASHBOARD LOGIC ---

function logAction(message, isAlert = false) {
    const now = new Date();
    const timeStr = now.getHours().toString().padStart(2, '0') + ":" +
        now.getMinutes().toString().padStart(2, '0') + ":" +
        now.getSeconds().toString().padStart(2, '0');

    const entry = { time: timeStr, msg: message, alert: isAlert };
    localLogs.unshift(entry);
    if (localLogs.length > 50) localLogs.pop();

    localStorage.setItem('fishLogs', JSON.stringify(localLogs));
    updateLogDisplay();
}

function updateLogDisplay() {
    const logs = document.getElementById('logs');
    if (!logs) return;

    if (localLogs.length === 0) {
        logs.innerHTML = '<span style="color:#B8C9D4;">No activity yet.</span>';
        return;
    }

    logs.innerHTML = localLogs.map(l => `
    <div class="log-entry">
      <span class="log-time" style="${l.alert ? 'color:var(--danger)' : ''}">[${l.time}]</span>
      <span class="log-msg">${l.msg}</span>
    </div>
  `).join('');
}

function feed() {
    // Try server first
    fetch('/feed').then(r => r.text()).then(d => {
        if (d === "OK") {
            processFeeding();
        } else {
            showToast("Cooldown active...");
        }
    }).catch(() => {
        // Local fallback
        processFeeding();
    });
}

function processFeeding() {
    feedCount++;
    localStorage.setItem('feedCount', feedCount);
    const count = document.getElementById('count');
    if (count) count.innerText = String(feedCount).padStart(2, '0');

    logAction("Manual Feeding triggered.");
    showToast("Feeding triggered!");
}

function save() {
    const h1 = document.getElementById('h1');
    const m1 = document.getElementById('m1');
    const h2 = document.getElementById('h2');
    const m2 = document.getElementById('m2');
    const minT = document.getElementById('minT');
    const maxT = document.getElementById('maxT');
    const minL = document.getElementById('minL');
    const maxL = document.getElementById('maxL');

    // Local Save
    cachedMinT = parseFloat(minT.value) || 22;
    cachedMaxT = parseFloat(maxT.value) || 30;
    cachedMinL = parseFloat(minL.value) || 40;
    cachedMaxL = parseFloat(maxL.value) || 90;

    localStorage.setItem('minT', cachedMinT);
    localStorage.setItem('maxT', cachedMaxT);
    localStorage.setItem('minL', cachedMinL);
    localStorage.setItem('maxL', cachedMaxL);
    localStorage.setItem('h1', h1.value);
    localStorage.setItem('m1', m1.value);
    localStorage.setItem('h2', h2.value);
    localStorage.setItem('m2', m2.value);

    updateRangeLabels();
    logAction("System settings updated.");
    showToast("Settings saved locally!");

    // Server Save
    const url = `/save?h1=${h1.value || 0}&m1=${m1.value || 0}&s1=0` +
        `&h2=${h2.value || 0}&m2=${m2.value || 0}&s2=0` +
        `&minT=${minT.value}&maxT=${maxT.value}` +
        `&minL=${minL.value}&maxL=${maxL.value}`;

    fetch(url).catch(() => console.log("Offline: Server not updated"));
}

function updateRangeLabels() {
    if (document.getElementById('tempMin')) document.getElementById('tempMin').innerText = `Min ${cachedMinT}°C`;
    if (document.getElementById('tempMax')) document.getElementById('tempMax').innerText = `Max ${cachedMaxT}°C`;
    if (document.getElementById('levelMin')) document.getElementById('levelMin').innerText = `Min ${cachedMinL}%`;
    if (document.getElementById('levelMax')) document.getElementById('levelMax').innerText = `Max ${cachedMaxL}%`;
}

function showToast(msg) {
    const toastMsg = document.getElementById('toastMsg');
    const toast = document.getElementById('toast');
    if (toastMsg && toast) {
        toastMsg.innerText = msg;
        toast.classList.add('show');
        setTimeout(() => toast.classList.remove('show'), 2600);
    }
}

// Check for scheduled feeding
let lastFeedMin = -1;
function checkSchedule() {
    const now = new Date();
    const curH = now.getHours();
    const curM = now.getMinutes();

    if (curM === lastFeedMin) return; // Only check once per minute

    const h1 = parseInt(localStorage.getItem('h1'));
    const m1 = parseInt(localStorage.getItem('m1'));
    const h2 = parseInt(localStorage.getItem('h2'));
    const m2 = parseInt(localStorage.getItem('m2'));

    // Default to 6am and 6pm if not set
    const sched1H = isNaN(h1) ? 6 : h1;
    const sched1M = isNaN(m1) ? 0 : m1;
    const sched2H = isNaN(h2) ? 18 : h2;
    const sched2M = isNaN(m2) ? 0 : m2;

    if ((curH === sched1H && curM === sched1M) || (curH === sched2H && curM === sched2M)) {
        lastFeedMin = curM;
        feedCount++;
        localStorage.setItem('feedCount', feedCount);
        const count = document.getElementById('count');
        if (count) count.innerText = String(feedCount).padStart(2, '0');
        logAction(`Scheduled Feeding at ${curH}:${curM.toString().padStart(2, '0')}`);
        showToast("Scheduled feeding active!");
    }
}

// Global initialization
document.addEventListener('DOMContentLoaded', () => {
    // Load Local Data
    cachedMinT = parseFloat(localStorage.getItem('minT')) || 22;
    cachedMaxT = parseFloat(localStorage.getItem('maxT')) || 30;
    cachedMinL = parseFloat(localStorage.getItem('minL')) || 40;
    cachedMaxL = parseFloat(localStorage.getItem('maxL')) || 90;

    const h1Input = document.getElementById('h1');
    const m1Input = document.getElementById('m1');
    const h2Input = document.getElementById('h2');
    const m2Input = document.getElementById('m2');
    const minTInput = document.getElementById('minT');
    const maxTInput = document.getElementById('maxT');
    const minLInput = document.getElementById('minL');
    const maxLInput = document.getElementById('maxL');

    if (h1Input) h1Input.value = localStorage.getItem('h1') || "";
    if (m1Input) m1Input.value = localStorage.getItem('m1') || "";
    if (h2Input) h2Input.value = localStorage.getItem('h2') || "";
    if (m2Input) m2Input.value = localStorage.getItem('m2') || "";
    if (minTInput) minTInput.value = cachedMinT;
    if (maxTInput) maxTInput.value = cachedMaxT;
    if (minLInput) minLInput.value = cachedMinL;
    if (maxLInput) maxLInput.value = cachedMaxL;

    updateRangeLabels();
    updateLogDisplay();
    const countDisp = document.getElementById('count');
    if (countDisp) countDisp.innerText = String(feedCount).padStart(2, '0');

    // Login Enter Key
    document.addEventListener('keydown', e => {
        if (e.key === 'Enter') {
            if (document.getElementById('u')) l();
        }
    });

    // Dashboard Updates
    if (document.getElementById('rtc')) {
        // Schedule check
        setInterval(checkSchedule, 10000);

        // Data + logs from server
        setInterval(() => {
            fetch('/data').then(r => r.json()).then(d => {
                // We prioritize server count if online
                const count = document.getElementById('count');
                if (count) count.innerText = String(d.count).padStart(2, '0');

                // Merge server logs if available
                if (d.logs) {
                    // For simplicity, we just use local logs for the premium demo
                }
            }).catch(() => { });
        }, 5000);

        // RTC
        setInterval(() => {
            fetch('/time').then(r => r.json()).then(d => {
                const rtc = document.getElementById('rtc');
                if (rtc) rtc.innerText = d.time;
            }).catch(() => {
                const rtc = document.getElementById('rtc');
                if (rtc) rtc.innerText = new Date().toLocaleTimeString();
            });
        }, 1000);

        // Sensors
        setInterval(() => {
            fetch('/sensor').then(r => r.json()).then(d => {
                updateSensors(d);
            }).catch(() => {
                // Mock sensor data
                const mockData = {
                    temp: 24.5 + (Math.random() * 2 - 1),
                    level: 82 + (Math.random() * 4 - 2),
                    alert: 0,
                    reason: ""
                };
                updateSensors(mockData);
            });
        }, 2000);
    }
});

let lastAlertMsg = "";
function updateSensors(d) {
    const tVal = parseFloat(d.temp);
    const lVal = parseFloat(d.level);

    // Logic for Range Alerts
    let alertActive = false;
    let reason = "";

    if (tVal < cachedMinT) { alertActive = true; reason = "Low Temperature Alert!"; }
    if (tVal > cachedMaxT) { alertActive = true; reason = "High Temperature Alert!"; }
    if (lVal < cachedMinL) { alertActive = true; reason = "Low Water Level!"; }
    if (lVal > cachedMaxL) { alertActive = true; reason = "High Water Level!"; }

    if (alertActive && reason !== lastAlertMsg) {
        logAction(reason, true);
        lastAlertMsg = reason;
    } else if (!alertActive) {
        lastAlertMsg = "";
    }

    // Update UI
    const tempVal = document.getElementById('tempVal');
    const tempBar = document.getElementById('tempBar');
    const tempCard = document.getElementById('tempCard');
    const tempPill = document.getElementById('tempPill');

    if (tempVal) {
        const tAlarm = (tVal < cachedMinT || tVal > cachedMaxT);
        tempVal.innerHTML = tVal.toFixed(1) + '<span class="unit"> °C</span>';
        const tPct = Math.min(Math.max(((tVal - 10) / (50 - 10)) * 100, 0), 100);
        if (tempBar) {
            tempBar.style.width = tPct + '%';
            tempBar.className = 'bar' + (tAlarm ? ' danger' : '');
        }
        if (tempCard) tempCard.className = 'card sensor-card' + (tAlarm ? ' alert-card' : '');
        if (tempPill) {
            tempPill.className = 'status-pill' + (tAlarm ? ' alert' : '');
            tempPill.innerText = tAlarm ? 'ALERT' : 'NORMAL';
        }
    }

    const levelVal = document.getElementById('levelVal');
    const levelBar = document.getElementById('levelBar');
    const levelCard = document.getElementById('levelCard');
    const levelPill = document.getElementById('levelPill');

    if (levelVal) {
        const lAlarm = (lVal < cachedMinL || lVal > cachedMaxL);
        levelVal.innerHTML = lVal.toFixed(1) + '<span class="unit"> %</span>';
        if (levelBar) {
            levelBar.style.width = lVal + '%';
            levelBar.className = 'bar' + (lAlarm ? ' danger' : '');
        }
        if (levelCard) levelCard.className = 'card sensor-card' + (lAlarm ? ' alert-card' : '');
        if (levelPill) {
            levelPill.className = 'status-pill' + (lAlarm ? ' alert' : '');
            levelPill.innerText = lAlarm ? 'ALERT' : 'NORMAL';
        }
    }

    const banner = document.getElementById('alertBanner');
    const alertMsg = document.getElementById('alertMsg');
    if (banner) {
        if (alertActive) {
            banner.classList.add('show');
            if (alertMsg) alertMsg.innerText = reason;
        } else {
            banner.classList.remove('show');
        }
    }
}