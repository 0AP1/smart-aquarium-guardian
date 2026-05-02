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