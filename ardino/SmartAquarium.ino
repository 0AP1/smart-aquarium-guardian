#include <WiFi.h>
#include <WebServer.h>
#include <Preferences.h>
#include <Wire.h>
#include "RTClib.h"
#include <ESP32Servo.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// ================= WIFI =================
const char* ssid = "SmartAquarium";
const char* password = "12345678";

WebServer server(80);
Preferences prefs;

// ================= LOGIN =================
String loginUser = "aanchal";
String loginPass = "tbc@2026";
bool loggedIn = false;

// ================= RTC =================
RTC_DS3231 rtc;

// ================= SERVO =================
Servo myServo;

// ================= TEMP =================
#define ONE_WIRE_BUS 4
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
float temperature = 0;

// ================= ULTRASONIC =================
#define TRIG 12
#define ECHO 13
float tankHeight = 16.0;
float waterPercent = 0;

// ================= BUTTON =================
#define BUTTON_PIN 27
bool lastButtonState = HIGH;
unsigned long lastDebounce = 0;
const int debounceDelay = 300;

// ================= ALERT =================
#define BUZZER 19
#define LED 25

float minTemp  = 18;
float maxTemp  = 30;
float minLevel = 50;
float maxLevel = 90;

bool alertState = false;
String alertReason = "";

unsigned long lastBeep = 0;
bool beepState = false;

// ================= SCHEDULE =================
int h1,m1,s1;
int h2,m2,s2;

// ================= SYSTEM =================
int feedCount = 0;
unsigned long lastFeed = 0;
const int COOLDOWN = 20000;

// ================= TIMER =================
unsigned long lastSensor = 0;

// ================= TIME STRING =================
String timeStr(DateTime now){
  char buf[20];
  sprintf(buf,"%02d:%02d:%02d",now.hour(),now.minute(),now.second());
  return String(buf);
}

// ================= DATE STRING =================
String dateStr(DateTime now){
  char buf[15];
  sprintf(buf,"%04d-%02d-%02d",now.year(),now.month(),now.day());
  return String(buf);
}

// ================= DATETIME STRING =================
String dateTimeStr(DateTime now){
  char buf[30];
  sprintf(buf,"%04d-%02d-%02d %02d:%02d:%02d",
    now.year(),now.month(),now.day(),
    now.hour(),now.minute(),now.second());
  return String(buf);
}

// ================= LOG =================
void saveLog(String reason){
  DateTime now = rtc.now();
  String logs = prefs.getString("logs","");
  logs += reason + " at " + dateTimeStr(now) + "\n";
  prefs.putString("logs",logs);
  feedCount++;
  prefs.putInt("count",feedCount);
}

// ================= FEED =================
void feedFish(String reason){
  if(millis() - lastFeed < COOLDOWN) return;
  lastFeed = millis();
  myServo.write(90);
  delay(400);
  myServo.write(40);
  saveLog(reason);
}

// ================= SENSOR =================
void readSensors(){
  sensors.requestTemperatures();
  temperature = sensors.getTempCByIndex(0);

  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);

  long duration = pulseIn(ECHO, HIGH);
  float distance = duration * 0.034 / 2;
  float waterLevel = tankHeight - distance;
  waterPercent = (waterLevel / tankHeight) * 100;
  if(waterPercent > 100) waterPercent = 100;
  if(waterPercent < 0)   waterPercent = 0;
}

// ================= ALERT =================
void checkAlerts(){
  alertState  = false;
  alertReason = "";

  if(temperature < minTemp){
    alertState = true;
    alertReason += "TEMP LOW ";
  }
  if(temperature > maxTemp){
    alertState = true;
    alertReason += "TEMP HIGH ";
  }
  if(waterPercent < minLevel){
    alertState = true;
    alertReason += "LEVEL LOW ";
  }
  if(waterPercent > maxLevel){
    alertState = true;
    alertReason += "LEVEL HIGH ";
  }

  digitalWrite(LED, alertState);

  if(alertState){
    if(millis() - lastBeep >= 500){
      beepState = !beepState;
      digitalWrite(BUZZER, beepState);
      lastBeep = millis();
    }
  } else {
    beepState = false;
    digitalWrite(BUZZER, LOW);
  }
}

// ================= LOGIN PAGE =================
void handleLogin(){
server.send(200,"text/html",R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta name="viewport" content="width=device-width,initial-scale=1">
<link href="https://fonts.googleapis.com/css2?family=DM+Sans:wght@300;400;500;600&family=Space+Grotesk:wght@400;500;600;700&display=swap" rel="stylesheet">
<style>
*{margin:0;padding:0;box-sizing:border-box;}
:root{
  --teal:#009EA8;
  --teal-dark:#007B84;
  --teal-light:#C8EEF2;
  --purple:#9B5DE5;
  --gradient:linear-gradient(135deg,#00D4D4,#9B5DE5);
  --bg:#C8E6F5;
  --card:#ffffff;
  --text:#1A3040;
  --muted:#5A8099;
  --border:#B8D8E8;
  --danger:#F05252;
}
body{
  background:linear-gradient(160deg,#C2E4F4 0%,#D8EEF8 50%,#C8E6F5 100%);
  font-family:'DM Sans',sans-serif;
  min-height:100vh;
  display:flex;
  flex-direction:column;
  align-items:center;
  justify-content:center;
  padding:20px;
  position:relative;
  overflow:hidden;
}
body::before{
  content:'';
  position:fixed;
  top:0;left:0;right:0;bottom:0;
  background:
    radial-gradient(ellipse 70% 50% at 50% 0%, rgba(180,225,245,0.6) 0%, transparent 60%),
    radial-gradient(ellipse 60% 40% at 80% 90%, rgba(155,93,229,0.06) 0%, transparent 60%);
  pointer-events:none;
  z-index:0;
}
.card{
  position:relative;z-index:1;
  background:var(--card);
  border-radius:24px;
  padding:44px 36px 40px;
  width:100%;max-width:380px;
  box-shadow:0 8px 40px rgba(0,120,150,0.12), 0 2px 8px rgba(0,0,0,0.05);
  text-align:center;
}
.logo{
  width:60px;height:60px;
  background:linear-gradient(135deg,#007B84,#009EA8);
  border-radius:14px;
  display:flex;align-items:center;justify-content:center;
  margin:0 auto 18px;
  font-size:26px;
  box-shadow:0 4px 16px rgba(0,158,168,0.35);
}
h1{
  font-family:'Space Grotesk',sans-serif;
  font-size:1.5rem;
  font-weight:700;
  color:var(--text);
  margin-bottom:4px;
}
.sub{
  font-size:0.72rem;
  letter-spacing:2.5px;
  color:var(--teal);
  text-transform:uppercase;
  font-weight:600;
  margin-bottom:32px;
}
.field-wrap{
  text-align:left;
  margin-bottom:14px;
}
.field-label{
  font-size:0.7rem;
  font-weight:600;
  letter-spacing:1.5px;
  text-transform:uppercase;
  color:var(--muted);
  margin-bottom:6px;
  display:flex;
  align-items:center;
  gap:6px;
}
.field-label svg{opacity:0.6;}
.field{
  width:100%;
  padding:13px 16px;
  background:#F4F9FB;
  border:1.5px solid #CCDDE6;
  border-radius:10px;
  color:var(--text);
  font-family:'DM Sans',sans-serif;
  font-size:0.95rem;
  outline:none;
  transition:border 0.25s, box-shadow 0.25s, background 0.25s;
}
.field:focus{
  border-color:var(--teal);
  background:#fff;
  box-shadow:0 0 0 3px rgba(0,158,168,0.12);
}
.field::placeholder{color:#9BBFCE;}
.btn{
  width:100%;
  padding:15px;
  background:linear-gradient(90deg,#00D4D4,#9B5DE5);
  border:none;
  border-radius:12px;
  color:#fff;
  font-family:'Space Grotesk',sans-serif;
  font-size:0.9rem;
  font-weight:600;
  letter-spacing:1.5px;
  cursor:pointer;
  transition:all 0.3s;
  text-transform:uppercase;
  margin-top:10px;
  display:flex;
  align-items:center;
  justify-content:center;
  gap:8px;
  box-shadow:0 4px 20px rgba(0,200,200,0.35);
}
.btn:hover{
  transform:translateY(-1px);
  box-shadow:0 8px 28px rgba(0,200,200,0.45);
}
.btn:active{transform:translateY(0);}
#msg{
  margin-top:14px;
  color:var(--danger);
  font-size:0.82rem;
  font-weight:500;
  letter-spacing:0.5px;
  min-height:18px;
}
.status-bar{
  margin-top:28px;
  display:flex;
  align-items:center;
  justify-content:space-between;
  font-size:0.7rem;
  color:var(--muted);
  letter-spacing:1px;
}
.status-dot{
  width:7px;height:7px;
  border-radius:50%;
  background:#22C55E;
  display:inline-block;
  margin-right:5px;
  box-shadow:0 0 6px rgba(34,197,94,0.6);
}
</style>
</head>
<body>
<div class="card">
  <div class=""></div>
  <h1>Smart Aquarium</h1>
  <div class="sub">Guardian System</div>

  <div class="field-wrap">
    <div class="field-label">
      <svg width="12" height="12" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.5"><path d="M20 21v-2a4 4 0 0 0-4-4H8a4 4 0 0 0-4 4v2"/><circle cx="12" cy="7" r="4"/></svg>
      User ID
    </div>
    <input class="field" id="u" placeholder="Enter ID string" autocomplete="off">
  </div>

  <div class="field-wrap">
    <div class="field-label">
      <svg width="12" height="12" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.5"><rect x="3" y="11" width="18" height="11" rx="2" ry="2"/><path d="M7 11V7a5 5 0 0 1 10 0v4"/></svg>
      Access Key
    </div>
    <input class="field" id="p" type="password" placeholder="">
  </div>

  <button class="btn" onclick="l()">
    <svg width="16" height="16" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.5"><path d="M15 3h4a2 2 0 0 1 2 2v14a2 2 0 0 1-2 2h-4"/><polyline points="10 17 15 12 10 7"/><line x1="15" y1="12" x2="3" y2="12"/></svg>
    Access
  </button>
  <p id="msg"></p>

  <div class="status-bar">
    <span><span class="status-dot"></span>Designed and developed by Aanchal Poudel</span>
    
  </div>
</div>

<script>
function l(){
  fetch(`/doLogin?u=${encodeURIComponent(u.value)}&p=${encodeURIComponent(p.value)}`)
  .then(r=>r.text()).then(d=>{
    if(d=="OK") location.href="/";
    else{ msg.innerText="⚠ Invalid credentials. Please try again."; }
  });
}
document.addEventListener('keydown',e=>{if(e.key==='Enter')l();});
</script>
</body>
</html>
)rawliteral");
}

// ================= LOGIN CHECK =================
void handleDoLogin(){
  if(server.arg("u")==loginUser && server.arg("p")==loginPass){
    loggedIn=true;
    server.send(200,"text/plain","OK");
  } else {
    server.send(200,"text/plain","FAIL");
  }
}

// ================= DASHBOARD =================
void handleRoot(){
if(!loggedIn){
  server.sendHeader("Location","/login");
  server.send(302,"text/plain","");
  return;
}
server.send(200,"text/html",R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta name="viewport" content="width=device-width,initial-scale=1">
<link href="https://fonts.googleapis.com/css2?family=DM+Sans:wght@300;400;500;600&family=Space+Grotesk:wght@400;500;600;700&display=swap" rel="stylesheet">
<style>
*{margin:0;padding:0;box-sizing:border-box;}
:root{
  --teal:#009EA8;
  --teal-dark:#007B84;
  --teal-light:#C8EEF2;
  --purple:#9B5DE5;
  --gradient:linear-gradient(90deg,#00D4D4,#9B5DE5);
  --bg:#C8E6F5;
  --card:#ffffff;
  --text:#1A3040;
  --muted:#5A8099;
  --border:#B8D8E8;
  --danger:#F05252;
  --warn:#F59E0B;
  --ok:#22C55E;
}
*{-webkit-tap-highlight-color:transparent;}
body{
  background:linear-gradient(160deg,#BDE0F4 0%,#CCE9F7 40%,#C4E4F3 100%);
  color:var(--text);
  font-family:'DM Sans',sans-serif;
  min-height:100vh;
  padding:0 0 40px;
  position:relative;
}
body::before{
  content:'';
  position:fixed;
  top:0;left:0;right:0;
  height:260px;
  background:linear-gradient(180deg,rgba(180,225,245,0.5) 0%,transparent 100%);
  pointer-events:none;
  z-index:0;
}

/* Header */
.header{
  position:relative;z-index:1;
  text-align:center;
  padding:28px 20px 20px;
}
.logo-row{
  display:flex;align-items:center;justify-content:center;gap:10px;
  margin-bottom:4px;
}
.logo-icon{
  width:42px;height:42px;
  background:linear-gradient(135deg,#007B84,#009EA8);
  border-radius:12px;
  display:flex;align-items:center;justify-content:center;
  font-size:20px;
  box-shadow:0 4px 14px rgba(0,158,168,0.35);
}
h1{
  font-family:'Space Grotesk',sans-serif;
  font-size:1.1rem;
  font-weight:700;
  letter-spacing:2px;
  text-transform:uppercase;
  color:#007B84;
}
.sub{font-size:0.7rem;letter-spacing:1.5px;color:var(--muted);text-transform:uppercase;}

/* Alert banner */
#alertBanner{
  display:none;
  margin:0 16px 12px;
  background:rgba(240,82,82,0.08);
  border:1px solid rgba(240,82,82,0.3);
  border-radius:12px;
  padding:10px 16px;
  text-align:center;
  color:var(--danger);
  font-size:0.75rem;
  font-weight:600;
  letter-spacing:1px;
  text-transform:uppercase;
  animation:alertPulse 1.2s ease-in-out infinite;
}
#alertBanner.show{display:block;}
@keyframes alertPulse{0%,100%{opacity:1;}50%{opacity:0.5;}}

/* Main layout */
.wrapper{
  position:relative;z-index:1;
  max-width:600px;
  margin:0 auto;
  padding:0 16px;
}

/* Cards */
.card{
  background:var(--card);
  border-radius:20px;
  padding:20px;
  margin-bottom:12px;
  box-shadow:0 2px 16px rgba(0,100,130,0.08), 0 1px 3px rgba(0,0,0,0.04);
  border:1px solid rgba(184,216,232,0.7);
}
.card.alert-card{
  border-color:rgba(240,82,82,0.3);
  box-shadow:0 2px 16px rgba(240,82,82,0.08);
}
.section-label{
  font-size:0.68rem;
  font-weight:700;
  letter-spacing:2px;
  text-transform:uppercase;
  color:var(--muted);
  margin:18px 0 8px 2px;
}

/* Time card */
.time-card{
  text-align:center;
  padding:22px 20px 18px;
}
.time-sublabel{
  font-size:0.65rem;
  letter-spacing:2px;
  text-transform:uppercase;
  color:var(--muted);
  margin-bottom:6px;
}
.time-big{
  font-family:'Space Grotesk',sans-serif;
  font-size:3rem;
  font-weight:700;
  color:#009EA8;
  letter-spacing:4px;
  line-height:1;
}
.date-display{
  font-family:'Space Grotesk',sans-serif;
  font-size:1.05rem;
  font-weight:600;
  color:#5A8099;
  letter-spacing:3px;
  margin-bottom:16px;
}
.time-divider{
  width:40px;
  height:2px;
  background:linear-gradient(90deg,#009EA8,#9B5DE5);
  border-radius:2px;
  margin:10px auto 14px;
  opacity:0.4;
}

/* Sensor grid */
.sensor-grid{display:grid;grid-template-columns:1fr 1fr;gap:12px;}
@media(max-width:380px){.sensor-grid{grid-template-columns:1fr;}}

.sensor-card{padding:18px;}
.sensor-header{
  display:flex;align-items:center;justify-content:space-between;
  margin-bottom:12px;
}
.sensor-name{
  font-size:0.65rem;
  font-weight:700;
  letter-spacing:2px;
  text-transform:uppercase;
  color:var(--muted);
  display:flex;align-items:center;gap:5px;
}
.sensor-icon{font-size:13px;}
.status-pill{
  font-size:0.6rem;
  font-weight:700;
  letter-spacing:1px;
  text-transform:uppercase;
  padding:3px 9px;
  border-radius:20px;
  background:rgba(34,197,94,0.1);
  color:var(--ok);
  border:1px solid rgba(34,197,94,0.2);
}
.status-pill.alert{
  background:rgba(240,82,82,0.1);
  color:var(--danger);
  border-color:rgba(240,82,82,0.2);
}
.sensor-value{
  font-family:'Space Grotesk',sans-serif;
  font-size:2rem;
  font-weight:700;
  color:var(--text);
  margin-bottom:10px;
  line-height:1;
}
.sensor-value .unit{font-size:1rem;font-weight:500;color:var(--muted);}
.sensor-value.ok{color:var(--text);}
.sensor-value.danger{color:var(--danger);}

/* Progress bar */
.bar-wrap{
  background:#D8EEF4;
  border-radius:6px;
  height:6px;
  overflow:hidden;
  margin-bottom:6px;
}
.bar{
  height:100%;
  border-radius:6px;
  background:linear-gradient(90deg,#009EA8,#00C8CC);
  transition:width 1s ease;
}
.bar.danger{background:linear-gradient(90deg,#F05252,#FF8A80);}
.range-row{
  display:flex;justify-content:space-between;
  font-size:0.65rem;color:var(--muted);
}

/* Feed section */
.feed-card{padding:18px 20px;}
.feed-row{
  display:flex;align-items:center;justify-content:space-between;
}
.feed-info .feed-label{
  font-size:0.65rem;font-weight:700;
  letter-spacing:2px;text-transform:uppercase;
  color:var(--muted);margin-bottom:4px;
}
.feed-count{
  font-family:'Space Grotesk',sans-serif;
  font-size:2.2rem;font-weight:700;
  color:var(--text);
  line-height:1;
}
.feed-btn{
  display:flex;align-items:center;gap:7px;
  padding:12px 20px;
  background:var(--card);
  border:1.5px solid #009EA8;
  border-radius:12px;
  color:#009EA8;
  font-family:'Space Grotesk',sans-serif;
  font-size:0.8rem;
  font-weight:600;
  letter-spacing:1px;
  cursor:pointer;
  transition:all 0.25s;
  text-transform:uppercase;
}
.feed-btn:hover{
  background:#E8F7F8;
  box-shadow:0 4px 16px rgba(0,158,168,0.2);
}
.feed-btn:active{transform:scale(0.97);}

/* Schedule */
.sched-grid{display:grid;grid-template-columns:1fr;gap:12px;}

.sched-card{padding:16px 18px;}
.sched-label{
  font-size:0.65rem;font-weight:700;
  letter-spacing:2px;text-transform:uppercase;
  color:var(--muted);margin-bottom:10px;
  display:flex;align-items:center;gap:5px;
}
.time-inputs{display:flex;gap:6px;}
.time-field{
  flex:1;
  text-align:center;
  padding:9px 4px;
  background:#F2F8FB;
  border:1.5px solid #C8DCEA;
  border-radius:10px;
  color:var(--text);
  font-family:'Space Grotesk',sans-serif;
  font-size:0.9rem;
  font-weight:600;
  outline:none;
  transition:border 0.25s, box-shadow 0.25s;
}
.time-field:focus{
  border-color:#009EA8;
  background:#fff;
  box-shadow:0 0 0 3px rgba(0,158,168,0.12);
}
.time-field::placeholder{color:#9BBFCE;font-size:0.75rem;}
.time-sep{
  font-size:0.75rem;color:var(--muted);
  display:flex;align-items:center;padding:0 1px;
}

/* Alert thresholds */
.thresh-grid{display:grid;grid-template-columns:1fr 1fr;gap:12px;}
@media(max-width:380px){.thresh-grid{grid-template-columns:1fr;}}
.thresh-card{padding:16px 18px;}
.thresh-title{
  font-size:0.65rem;font-weight:700;
  letter-spacing:2px;text-transform:uppercase;
  color:var(--muted);margin-bottom:12px;
  display:flex;align-items:center;gap:5px;
}
.minmax-row{display:grid;grid-template-columns:1fr 1fr;gap:8px;}
.input-group{}
.input-group .lbl{
  font-size:0.6rem;font-weight:700;
  letter-spacing:1.5px;text-transform:uppercase;
  color:var(--muted);margin-bottom:4px;
  display:block;
}
.thresh-field{
  width:100%;
  padding:9px 10px;
  background:#F2F8FB;
  border:1.5px solid #C8DCEA;
  border-radius:10px;
  color:var(--text);
  font-family:'Space Grotesk',sans-serif;
  font-size:0.9rem;
  font-weight:600;
  outline:none;
  text-align:center;
  transition:border 0.25s, box-shadow 0.25s;
}
.thresh-field:focus{
  border-color:#009EA8;
  background:#fff;
  box-shadow:0 0 0 3px rgba(0,158,168,0.12);
}

/* Save button */
.save-btn{
  width:100%;
  padding:15px;
  background:linear-gradient(90deg,#009EA8,#007B84);
  border:none;
  border-radius:14px;
  color:#fff;
  font-family:'Space Grotesk',sans-serif;
  font-size:0.85rem;
  font-weight:600;
  letter-spacing:2px;
  cursor:pointer;
  transition:all 0.3s;
  text-transform:uppercase;
  display:flex;align-items:center;justify-content:center;gap:8px;
  box-shadow:0 4px 18px rgba(0,158,168,0.28);
  margin-top:4px;
}
.save-btn:hover{
  transform:translateY(-1px);
  box-shadow:0 8px 24px rgba(0,158,168,0.38);
}
.save-btn:active{transform:translateY(0);}

/* Log */
.log-card{padding:18px 20px;}
.log-label{
  font-size:0.65rem;font-weight:700;
  letter-spacing:2px;text-transform:uppercase;
  color:var(--muted);margin-bottom:10px;
}
.log-box{
  background:#EEF6FA;
  border:1px solid #C8DCEA;
  border-radius:12px;
  padding:14px;
  height:200px;
  overflow-y:auto;
  font-size:0.78rem;
  line-height:2;
  color:var(--muted);
  font-family:'DM Mono','DM Sans',monospace;
}
.log-box::-webkit-scrollbar{width:3px;}
.log-box::-webkit-scrollbar-track{background:transparent;}
.log-box::-webkit-scrollbar-thumb{background:var(--border);border-radius:3px;}
.log-entry{display:flex;gap:8px;flex-wrap:wrap;}
.log-date{color:#9B5DE5;font-weight:600;white-space:nowrap;font-size:0.72rem;}
.log-time{color:#009EA8;font-weight:600;white-space:nowrap;}
.log-msg{color:var(--text);}

/* Toast */
#toast{
  position:fixed;bottom:20px;right:20px;z-index:99;
  background:var(--text);
  border-radius:12px;
  padding:11px 20px;
  font-family:'Space Grotesk',sans-serif;
  font-size:0.75rem;
  color:#fff;
  letter-spacing:1px;
  opacity:0;
  transform:translateY(8px);
  transition:all 0.35s;
  pointer-events:none;
  display:flex;align-items:center;gap:7px;
}
#toast.show{opacity:1;transform:translateY(0);}
.toast-dot{width:6px;height:6px;border-radius:50%;background:var(--ok);}
</style>
</head>
<body>

<div class="header">
  <div class="logo-row">
    <div class="logo-icon"></div>
    <h1>Smart Aquarium Guardian</h1>
  </div>
  <div class="sub">IoT-Based Automated Feeding with Temperature and Water-Level Alerts</div>
</div>

<div class="wrapper">

  <!-- Alert Banner -->
  <div id="alertBanner">⚠ Alert: <span id="alertMsg"></span></div>

  <!-- Time & Date Card -->
  <div class="card time-card">
    <div class="time-sublabel">System Date</div>
    <div class="date-display" id="rtc-date">----/--/--</div>
    <div class="time-divider"></div>
    <div class="time-sublabel">System Time</div>
    <div class="time-big" id="rtc">--:--:--</div>
  </div>

  <!-- Sensors -->
  <div class="sensor-grid">
    <!-- Temperature -->
    <div class="card sensor-card" id="tempCard">
      <div class="sensor-header">
        <div class="sensor-name"><span class="sensor-icon"></span> Water Temp</div>
        <div class="status-pill" id="tempPill">OK</div>
      </div>
      <div class="sensor-value" id="tempVal">--<span class="unit"> °C</span></div>
      <div class="bar-wrap"><div class="bar" id="tempBar" style="width:50%"></div></div>
      <div class="range-row">
        <span id="tempMin">Min --°C</span>
        <span id="tempMax">Max --°C</span>
      </div>
    </div>

    <!-- Water Level -->
    <div class="card sensor-card" id="levelCard">
      <div class="sensor-header">
        <div class="sensor-name"><span class="sensor-icon"></span> Water Level</div>
        <div class="status-pill" id="levelPill">OK</div>
      </div>
      <div class="sensor-value" id="levelVal">--<span class="unit"> %</span></div>
      <div class="bar-wrap"><div class="bar" id="levelBar" style="width:50%"></div></div>
      <div class="range-row">
        <span id="levelMin">Min --%</span>
        <span id="levelMax">Max --%</span>
      </div>
    </div>
  </div>

  <!-- Feeding -->
  <div class="section-label">Feeding</div>
  <div class="card feed-card">
    <div class="feed-row">
      <div class="feed-info">
        <div class="feed-label">Total Feeds Today</div>
        <div class="feed-count" id="count">0</div>
      </div>
      <button class="feed-btn" onclick="feed()">
        <svg width="15" height="15" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.5"><path d="M3 11l19-9-9 19-2-8-8-2z"/></svg>
        Feed Now
      </button>
    </div>
  </div>

  <!-- Schedule -->
  <div class="section-label">Schedule</div>
  <div class="sched-grid">
    <div class="card sched-card">
      <div class="sched-label">
        <svg width="12" height="12" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.5"><circle cx="12" cy="12" r="10"/><polyline points="12 6 12 12 16 14"/></svg>
        Schedule 1
      </div>
      <div class="time-inputs">
        <input class="time-field" id="h1" placeholder="hh" maxlength="2">
        <span class="time-sep">:</span>
        <input class="time-field" id="m1" placeholder="mm" maxlength="2">
        <span class="time-sep">:</span>
        <input class="time-field" id="s1" placeholder="ss" maxlength="2">
      </div>
    </div>
    <div class="card sched-card">
      <div class="sched-label">
        <svg width="12" height="12" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.5"><circle cx="12" cy="12" r="10"/><polyline points="12 6 12 12 16 14"/></svg>
        Schedule 2
      </div>
      <div class="time-inputs">
        <input class="time-field" id="h2" placeholder="hh" maxlength="2">
        <span class="time-sep">:</span>
        <input class="time-field" id="m2" placeholder="mm" maxlength="2">
        <span class="time-sep">:</span>
        <input class="time-field" id="s2" placeholder="ss" maxlength="2">
      </div>
    </div>
  </div>

  <!-- Alert Thresholds -->
  <div class="section-label">Alert Thresholds</div>
  <div class="thresh-grid">
    <div class="card thresh-card">
      <div class="thresh-title">
        <svg width="12" height="12" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.5"><path d="M14 14.76V3.5a2.5 2.5 0 0 0-5 0v11.26a4.5 4.5 0 1 0 5 0z"/></svg>
        Temp Range (°C)
      </div>
      <div class="minmax-row">
        <div class="input-group">
          <span class="lbl">Min</span>
          <input class="thresh-field" id="minT" placeholder="22">
        </div>
        <div class="input-group">
          <span class="lbl">Max</span>
          <input class="thresh-field" id="maxT" placeholder="30">
        </div>
      </div>
    </div>
    <div class="card thresh-card">
      <div class="thresh-title">
        <svg width="12" height="12" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.5"><path d="M12 2.69l5.66 5.66a8 8 0 1 1-11.31 0z"/></svg>
        Level Range (%)
      </div>
      <div class="minmax-row">
        <div class="input-group">
          <span class="lbl">Min</span>
          <input class="thresh-field" id="minL" placeholder="40">
        </div>
        <div class="input-group">
          <span class="lbl">Max</span>
          <input class="thresh-field" id="maxL" placeholder="90">
        </div>
      </div>
    </div>
  </div>

  <div class="card" style="padding:16px 20px;margin-bottom:12px;">
    <button class="save-btn" onclick="save()">
      <svg width="16" height="16" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.5"><path d="M19 21H5a2 2 0 0 1-2-2V5a2 2 0 0 1 2-2h11l5 5v11a2 2 0 0 1-2 2z"/><polyline points="17 21 17 13 7 13 7 21"/><polyline points="7 3 7 8 15 8"/></svg>
      Save Settings
    </button>
  </div>

  <!-- Feed Log -->
  <div class="section-label">Feed Log</div>
  <div class="card log-card">
    <div class="log-label">Activity Log</div>
    <div class="log-box" id="logs">Loading...</div>
  </div>

</div><!-- /wrapper -->

<div id="toast"><div class="toast-dot"></div><span id="toastMsg">Saved</span></div>

<script>
let cachedMinT=22,cachedMaxT=30,cachedMinL=40,cachedMaxL=90;
let rangeLoaded=false;

function feed(){
  fetch('/feed').then(r=>r.text()).then(d=>{
    showToast(d==="OK" ? "Feeding triggered!" : "Cooldown active...");
  });
}

function save(){
  const url=`/save?h1=${h1.value||0}&m1=${m1.value||0}&s1=${s1.value||0}`+
            `&h2=${h2.value||0}&m2=${m2.value||0}&s2=${s2.value||0}`+
            `&minT=${minT.value}&maxT=${maxT.value}`+
            `&minL=${minL.value}&maxL=${maxL.value}`;
  fetch(url).then(()=>{
    cachedMinT=parseFloat(minT.value);
    cachedMaxT=parseFloat(maxT.value);
    cachedMinL=parseFloat(minL.value);
    cachedMaxL=parseFloat(maxL.value);
    updateRangeLabels();
    showToast("Settings saved!");
  });
}

function updateRangeLabels(){
  document.getElementById('tempMin').innerText=`Min ${cachedMinT}°C`;
  document.getElementById('tempMax').innerText=`Max ${cachedMaxT}°C`;
  document.getElementById('levelMin').innerText=`Min ${cachedMinL}%`;
  document.getElementById('levelMax').innerText=`Max ${cachedMaxL}%`;
}

function showToast(msg){
  document.getElementById('toastMsg').innerText=msg;
  const t=document.getElementById('toast');
  t.classList.add('show');
  setTimeout(()=>t.classList.remove('show'),2600);
}

// Format log entries — now with date + time
function formatLogs(raw){
  const lines = raw.split('<br>').filter(l=>l.trim());
  if(lines.length===0){
    return '<span style="color:#B8C9D4;">No activity yet.</span>';
  }
  return lines.map(l=>{
    // Format: "Manual at 2025-05-03 14:32:10"
    const atIdx = l.lastIndexOf(' at ');
    if(atIdx !== -1){
      const action   = l.substring(0, atIdx);
      const datetime = l.substring(atIdx + 4);
      const parts    = datetime.split(' ');
      const datePart = parts[0] || '';
      const timePart = parts[1] || '';
      return `<div class="log-entry">` +
             `<span class="log-date">[${datePart}]</span>` +
             `<span class="log-time">[${timePart}]</span>` +
             `<span class="log-msg">${action}</span>` +
             `</div>`;
    }
    return `<div class="log-entry"><span class="log-msg">${l}</span></div>`;
  }).reverse().join('');
}

// Data + logs
setInterval(()=>{
  fetch('/data').then(r=>r.json()).then(d=>{
    document.getElementById('count').innerText=String(d.count).padStart(2,'0');
    document.getElementById('logs').innerHTML = formatLogs(d.logs||'');
    if(!rangeLoaded && d.minT){
      minT.value=d.minT; maxT.value=d.maxT;
      minL.value=d.minL; maxL.value=d.maxL;
      cachedMinT=d.minT; cachedMaxT=d.maxT;
      cachedMinL=d.minL; cachedMaxL=d.maxL;
      updateRangeLabels();
      rangeLoaded=true;
    }
  });
},1500);

// RTC — date + time
setInterval(()=>{
  fetch('/time').then(r=>r.json()).then(d=>{
    document.getElementById('rtc').innerText = d.time;
    if(d.date) document.getElementById('rtc-date').innerText = d.date;
  });
},1000);

// Sensor
setInterval(()=>{
  fetch('/sensor').then(r=>r.json()).then(d=>{
    const tVal=parseFloat(d.temp);
    const lVal=parseFloat(d.level);
    const isAlert=(d.alert==1);

    // Temp
    const tAlarm=(tVal<cachedMinT||tVal>cachedMaxT);
    document.getElementById('tempVal').innerHTML=tVal.toFixed(1)+'<span class="unit"> °C</span>';
    const tPct=Math.min(Math.max(((tVal-10)/(50-10))*100,0),100);
    document.getElementById('tempBar').style.width=tPct+'%';
    document.getElementById('tempBar').className='bar'+(tAlarm?' danger':'');
    document.getElementById('tempCard').className='card sensor-card'+(tAlarm?' alert-card':'');
    document.getElementById('tempPill').className='status-pill'+(tAlarm?' alert':'');
    document.getElementById('tempPill').innerText=tAlarm?'ALERT':'OK';

    // Level
    const lAlarm=(lVal<cachedMinL||lVal>cachedMaxL);
    document.getElementById('levelVal').innerHTML=lVal.toFixed(1)+'<span class="unit"> %</span>';
    document.getElementById('levelBar').style.width=lVal+'%';
    document.getElementById('levelBar').className='bar'+(lAlarm?' danger':'');
    document.getElementById('levelCard').className='card sensor-card'+(lAlarm?' alert-card':'');
    document.getElementById('levelPill').className='status-pill'+(lAlarm?' alert':'');
    document.getElementById('levelPill').innerText=lAlarm?'ALERT':'OK';

    // Banner
    const banner=document.getElementById('alertBanner');
    if(isAlert){
      banner.classList.add('show');
      document.getElementById('alertMsg').innerText=d.reason||'Check sensors';
    } else {
      banner.classList.remove('show');
    }
  });
},2000);
</script>
</body>
</html>
)rawliteral");
}

// ================= APIs =================
void handleData(){
  String logs = prefs.getString("logs","");
  logs.replace("\n","<br>");
  int count = prefs.getInt("count",0);

  String json = "{\"count\":"+String(count)+
                ",\"logs\":\""+logs+"\""+
                ",\"minT\":"+String(minTemp)+
                ",\"maxT\":"+String(maxTemp)+
                ",\"minL\":"+String(minLevel)+
                ",\"maxL\":"+String(maxLevel)+"}";
  server.send(200,"application/json",json);
}

void handleTime(){
  DateTime now = rtc.now();
  server.send(200,"application/json",
  "{\"time\":\""+timeStr(now)+"\",\"date\":\""+dateStr(now)+"\"}");
}

void handleSensor(){
  String reason = alertReason;
  reason.trim();

  String json = "{\"temp\":"+String(temperature,1)+
                ",\"level\":"+String(waterPercent,1)+
                ",\"alert\":"+String(alertState ? 1 : 0)+
                ",\"reason\":\""+reason+"\"}";
  server.send(200,"application/json",json);
}

// ================= SAVE =================
void handleSave(){
  h1=server.arg("h1").toInt();
  m1=server.arg("m1").toInt();
  s1=server.arg("s1").toInt();
  h2=server.arg("h2").toInt();
  m2=server.arg("m2").toInt();
  s2=server.arg("s2").toInt();

  minTemp  = server.arg("minT").toFloat();
  maxTemp  = server.arg("maxT").toFloat();
  minLevel = server.arg("minL").toFloat();
  maxLevel = server.arg("maxL").toFloat();

  prefs.putInt("h1",h1); prefs.putInt("m1",m1); prefs.putInt("s1",s1);
  prefs.putInt("h2",h2); prefs.putInt("m2",m2); prefs.putInt("s2",s2);

  prefs.putFloat("minTemp",minTemp);
  prefs.putFloat("maxTemp",maxTemp);
  prefs.putFloat("minLevel",minLevel);
  prefs.putFloat("maxLevel",maxLevel);

  server.send(200,"text/plain","Saved");
}

// ================= SETUP =================
void setup(){
  Serial.begin(115200);
  WiFi.softAP(ssid,password);

  prefs.begin("data",false);

  feedCount = prefs.getInt("count",0);
  h1=prefs.getInt("h1",0);  m1=prefs.getInt("m1",0);  s1=prefs.getInt("s1",0);
  h2=prefs.getInt("h2",0);  m2=prefs.getInt("m2",0);  s2=prefs.getInt("s2",0);

  minTemp  = prefs.getFloat("minTemp",22);
  maxTemp  = prefs.getFloat("maxTemp",30);
  minLevel = prefs.getFloat("minLevel",40);
  maxLevel = prefs.getFloat("maxLevel",90);

  rtc.begin();

  myServo.attach(18);
  myServo.write(40);

  sensors.begin();

  pinMode(TRIG,OUTPUT);
  pinMode(ECHO,INPUT);
  pinMode(BUTTON_PIN,INPUT_PULLUP);
  pinMode(BUZZER,OUTPUT);
  pinMode(LED,OUTPUT);

  server.on("/",handleRoot);
  server.on("/login",handleLogin);
  server.on("/doLogin",handleDoLogin);
  server.on("/data",handleData);
  server.on("/time",handleTime);
  server.on("/sensor",handleSensor);
  server.on("/save",handleSave);

  server.on("/feed",[](){
    if(!loggedIn){
      server.send(401,"text/plain","LOGIN REQUIRED");
      return;
    }
    feedFish("Manual");
    server.send(200,"text/plain","OK");
  });

  server.begin();
}

// ================= LOOP =================
void loop(){
  server.handleClient();

  if(millis() - lastSensor > 2000){
    readSensors();
    checkAlerts();
    lastSensor = millis();
  }

  DateTime now = rtc.now();

  if(now.hour()==h1 && now.minute()==m1 && now.second()>=s1 && now.second()<s1+2)
    feedFish("RTC1");

  if(now.hour()==h2 && now.minute()==m2 && now.second()>=s2 && now.second()<s2+2)
    feedFish("RTC2");

  bool current = digitalRead(BUTTON_PIN);
  if(current==LOW && lastButtonState==HIGH){
    if(millis()-lastDebounce>300){
      feedFish("BUTTON");
      lastDebounce=millis();
    }
  }
  lastButtonState=current;
}
