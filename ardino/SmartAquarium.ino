const char* ssid = "SmartAquarium";
const char* password = "12345678";
WebServer server(80);
// In setup():
WiFi.softAP(ssid, password);
server.begin();

String loginUser = "admin";
String loginPass = "1234";
bool loggedIn = false;

void handleLogin() { /* serves login HTML */ }
void handleDoLogin() {
  if(server.arg("u")==loginUser && server.arg("p")==loginPass){
    loggedIn = true;
    server.send(200,"text/plain","OK");
  }
}

RTC_DS3231 rtc;
String timeStr(DateTime now){
  char buf[20];
  sprintf(buf,"%02d:%02d:%02d", now.hour(), now.minute(), now.second());
  return String(buf);
}
// In setup():
rtc.begin();

Servo myServo;
const int COOLDOWN = 20000;
unsigned long lastFeed = 0;

void feedFish(String reason){
  if(millis() - lastFeed < COOLDOWN) return;
  lastFeed = millis();
  myServo.write(90);   // open
  delay(400);
  myServo.write(40);   // close
  saveLog(reason);
}
// In setup():
myServo.attach(18);
myServo.write(40);


#define ONE_WIRE_BUS 4
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
float temperature = 0;
// In readSensors():
sensors.requestTemperatures();
temperature = sensors.getTempCByIndex(0);

#define TRIG 12
#define ECHO 13
float tankHeight = 16.0;
float waterPercent = 0;
// In readSensors():
digitalWrite(TRIG, HIGH); delayMicroseconds(10); digitalWrite(TRIG, LOW);
long duration = pulseIn(ECHO, HIGH);
float distance = duration * 0.034 / 2;
float waterLevel = tankHeight - distance;
waterPercent = (waterLevel / tankHeight) * 100;


#define BUZZER 19
#define LED 25
float minTemp=18, maxTemp=30, minLevel=50, maxLevel=90;
bool alertState = false;

void checkAlerts(){
  alertState = false;
  if(temperature < minTemp || temperature > maxTemp) alertState = true;
  if(waterPercent < minLevel || waterPercent > maxLevel) alertState = true;
  digitalWrite(LED, alertState);
  if(alertState){
    if(millis() - lastBeep >= 500){ beepState = !beepState; digitalWrite(BUZZER, beepState); lastBeep = millis(); }
  } else { digitalWrite(BUZZER, LOW); }
}

#define BUTTON_PIN 27
bool lastButtonState = HIGH;
unsigned long lastDebounce = 0;
// In loop():
bool current = digitalRead(BUTTON_PIN);
if(current==LOW && lastButtonState==HIGH){
  if(millis()-lastDebounce > 300){
    feedFish("BUTTON");
    lastDebounce = millis();
  }
}
lastButtonState = current;

int h1,m1,s1, h2,m2,s2;
// In loop():
DateTime now = rtc.now();
if(now.hour()==h1 && now.minute()==m1 && now.second()>=s1 && now.second()<s1+2)
  feedFish("RTC1");
if(now.hour()==h2 && now.minute()==m2 && now.second()>=s2 && now.second()<s2+2)
  feedFish("RTC2");

  Preferences prefs;
void saveLog(String reason){
  DateTime now = rtc.now();
  String logs = prefs.getString("logs","");
  logs += reason + " at " + timeStr(now) + "\n";
  prefs.putString("logs", logs);
  feedCount++;
  prefs.putInt("count", feedCount);
}