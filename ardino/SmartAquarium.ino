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