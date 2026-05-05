# Smart Aquarium Guardian (IoT based Project)

Then paste the full content below.
# Smart Aquarium Guardian## IoT-Based Automated Feeding with Temperature and Water-Level AlertsSmart Aquarium Guardian is an ESP32-based IoT aquarium support system developed as a Level 6 Production Project. The project automates fish feeding, monitors water temperature, measures water level, triggers local alerts for unsafe conditions, and provides a local device-hosted web dashboard for user interaction.The system is designed as a local-first IoT prototype. It works without cloud dependency by using the ESP32 Access Point mode and a dashboard hosted directly from the ESP32. A user can connect to the ESP32 hotspot, open the local dashboard in a browser, view live aquarium status, configure settings, and manually trigger feeding.---## Project Title**Smart Aquarium Guardian: IoT-Based Automated Feeding with Temperature and Water-Level Alerts**---## Author**Aanchal Poudel**  BSc (Hons) Computing  Level 6 Production Project  The British College / Leeds Beckett University---## Project OverviewAquarium care requires regular feeding and continuous monitoring of safe water conditions. In real life, feeding fish can become difficult when the owner is away from home, and unsafe water temperature or water-level conditions may not be noticed immediately.Smart Aquarium Guardian solves this problem by combining:- automated feeding- manual feeding control- temperature monitoring- water-level monitoring- local buzzer and LED alerts- ESP32-hosted web dashboard- local settings and log storageThe project is built around an ESP32 microcontroller. The ESP32 reads sensor values, controls the feeder servo motor, activates local alerts, stores settings and logs locally, and hosts the dashboard through its own Wi-Fi hotspot.---## Aim of the ProjectThe aim of this project is to design and build an IoT-based aquarium support system that automates fish feeding using a servo-driven dispensing mechanism and monitors basic aquarium safety conditions. The system measures water temperature and water level, triggers local alerts using a buzzer and LED when unsafe conditions are detected, and displays live system information through a device-hosted local web dashboard.---## Main ObjectivesThe project objectives are:- To build an ESP32-based smart aquarium support prototype.- To automate fish feeding using a servo motor feeder.- To support scheduled feeding using RTC-based time.- To provide manual feeding through both the dashboard and a physical button.- To monitor water temperature using a DS18B20 waterproof temperature sensor.- To monitor water level using an ultrasonic water-level sensor.- To trigger buzzer and LED alerts when water temperature or water level goes outside safe thresholds.- To provide a local dashboard hosted by the ESP32.- To allow users to configure feeding schedules and alert thresholds from the dashboard.- To store settings, feed count, and activity logs locally using ESP32 Preferences/NVS.- To support testing and evaluation using screenshots, logs, and prototype evidence.---## Key Features### 1. Local ESP32 HotspotThe ESP32 creates its own Wi-Fi hotspot.```textSSID: SmartAquariumDefault local IP: 192.168.4.1
The user connects a phone or laptop to this hotspot and opens the dashboard using a web browser.

2. Login System
The system includes a simple dashboard login page.
Username: aanchalPassword: tbc@2026
After successful login, the user can access the main dashboard.

3. Local Web Dashboard
The dashboard is hosted directly by the ESP32 WebServer.
The dashboard displays:


RTC date and time


water temperature


water-level percentage


OK or ALERT status


active alert reason


total feed count


schedule settings


temperature threshold settings


water-level threshold settings


feed/activity log


manual Feed Now button


The dashboard is written using embedded HTML, CSS, and JavaScript inside the Arduino .ino file.

4. Automatic Feeding
The system supports two scheduled feeding times.
The DS3231 RTC module provides real-time clock data, and the ESP32 checks whether the current time matches the saved feeding schedule.
When a scheduled feeding time is reached:


the servo motor rotates


fish food is dispensed


feed count is updated


the event is saved in the activity log


A cooldown mechanism is used to prevent repeated feeding within a short time.

5. Manual Feeding
Manual feeding is supported in two ways:


from the dashboard using the Feed Now button


from the physical manual feed button connected to the ESP32


The manual feed action also uses a cooldown delay to reduce the risk of accidental repeated feeding.

6. Temperature Monitoring
The DS18B20 waterproof temperature sensor is used to monitor aquarium water temperature.
The temperature value is shown on the dashboard in degrees Celsius.
The user can set:


minimum safe temperature


maximum safe temperature


If the temperature goes outside the configured safe range, the system triggers an alert.

7. Water-Level Monitoring
The final implementation uses an ultrasonic water-level sensor to measure water level as a percentage.
The ultrasonic sensor uses:
TRIG GPIO 12ECHO GPIO 13
The measured distance is converted into water-level percentage based on the prototype tank height.
The user can set:


minimum safe water-level percentage


maximum safe water-level percentage


If the water level goes below or above the configured safe range, the system triggers an alert.

8. Buzzer and LED Alerts
The system uses local alert outputs:
Buzzer: GPIO 19LED: GPIO 25
Alerts are triggered when:


water temperature is too low


water temperature is too high


water level is too low


water level is too high


The dashboard also displays the alert reason.
Example alert messages:
TEMP LOWTEMP HIGHLEVEL LOWLEVEL HIGH

9. Local Settings and Logs
The project does not use a traditional database such as MySQL, MongoDB, Firebase, or SQLite.
Instead, settings and logs are stored locally on the ESP32 using:
Preferences / NVS
Stored data includes:


feeding schedule 1


feeding schedule 2


minimum and maximum temperature thresholds


minimum and maximum water-level thresholds


feed count


activity logs


This supports the local-first design and allows the prototype to work without internet access.

Final Implementation Note
The original project specification planned to use two float switches for low and high water-level detection.
However, in the final prototype, an ultrasonic water-level sensor was used instead. This change was made because the available float switch was too large for the small 5 to 6 litre prototype aquarium.
This was a practical engineering adaptation, not a mistake.
The ultrasonic sensor was more suitable because:


it is non-contact


it fits the small prototype aquarium better


it can measure water level as a percentage


it still supports unsafe water-level detection


it meets the core requirement of water-level monitoring and alerting



Hardware Components
ComponentPurposeESP32 Development BoardMain controller, Wi-Fi hotspot, web server, sensor and actuator controlDS18B20 Waterproof Temperature SensorMeasures aquarium water temperatureUltrasonic Water-Level SensorMeasures water level percentageDS3231 RTC ModuleProvides real-time clock data for scheduled feedingServo MotorRotates feeder mechanism to dispense fish foodManual Feed ButtonAllows physical manual feedingBuzzerAudible alert for unsafe conditionsLEDVisual alert for unsafe conditionsJumper Wires / BreadboardPrototype wiring and circuit connectionPower Supply / USB CablePowers ESP32 and supports firmware uploadAquarium PrototypeSmall 5 to 6 litre aquarium setup used for demonstration

GPIO Pin Configuration
HardwareESP32 PinDS18B20 Temperature SensorGPIO 4Ultrasonic Sensor TRIGGPIO 12Ultrasonic Sensor ECHOGPIO 13Servo Motor FeederGPIO 18BuzzerGPIO 19LEDGPIO 25Manual Feed ButtonGPIO 27DS3231 RTCI2C

Software and Libraries
The project is written in Arduino-style Embedded C/C++ for ESP32.
Main libraries used:
#include <WiFi.h>#include <WebServer.h>#include <Preferences.h>#include <Wire.h>#include "RTClib.h"#include <ESP32Servo.h>#include <OneWire.h>#include <DallasTemperature.h>
Library Purposes
LibraryPurposeWiFi.hCreates ESP32 Access Point hotspotWebServer.hHosts local dashboard and API routesPreferences.hStores settings and logs in ESP32 NVSWire.hSupports I2C communicationRTClib.hReads DS3231 RTC date and timeESP32Servo.hControls servo feeder motorOneWire.hSupports DS18B20 sensor communicationDallasTemperature.hReads DS18B20 temperature data

Project Architecture
The system follows a local embedded IoT architecture.
User Phone/Laptop        |        | Connects to ESP32 hotspot        |ESP32 Access Point        |ESP32 Web Server and Dashboard        |ESP32 Firmware Logic        |------------------------------------------------| Sensor Monitoring | Feeding Control | Alerts |------------------------------------------------        |Preferences / NVS Local Storage
The ESP32 acts as the central controller.
It performs the following roles:


creates a Wi-Fi hotspot


hosts the web dashboard


reads temperature data


reads water-level data


reads RTC time


receives manual feed button input


controls servo feeding


controls buzzer and LED alerts


stores settings and logs locally



Frontend and Backend Explanation
This project has both frontend and backend, but in an embedded IoT form.
Frontend
The frontend is the dashboard that the user sees in the browser.
It is written using:


HTML


CSS


JavaScript


It includes:


login page


dashboard interface


sensor cards


feeding controls


schedule inputs


threshold settings


alert banner


activity log display


Backend
The backend is the ESP32 firmware and WebServer logic.
It is written in Arduino-style Embedded C/C++.
It handles:


route handling


login checking


sensor reading


feeding logic


alert logic


schedule checking


Preferences/NVS storage


JSON API responses for dashboard updates


The backend is not a cloud server. It runs inside the ESP32.

Main Source Code File
The main implementation file is:
Smart_Aquarium_Guardian.ino
This file includes:


all ESP32 firmware logic


embedded dashboard frontend


sensor reading functions


alert checking functions


feeding functions


server routes


storage handling


setup and loop logic



Important Functions in the Code
FunctionPurposetimeStr()Converts RTC time into readable HH:MM:SS formatdateStr()Converts RTC date into readable YYYY-MM-DD formatdateTimeStr()Creates timestamp for logssaveLog()Stores feeding and alert-related events in Preferences/NVSfeedFish()Rotates servo feeder and records feeding eventreadSensors()Reads DS18B20 temperature and ultrasonic water-level datacheckAlerts()Checks temperature and water level against thresholdshandleLogin()Serves login pagehandleDoLogin()Checks dashboard username and passwordhandleRoot()Serves main dashboard pagehandleData()Sends feed count, logs, and settings to dashboardhandleTime()Sends RTC date and time to dashboardhandleSensor()Sends temperature, water level, and alert statehandleSave()Saves updated schedules and thresholdssetup()Initialises Wi-Fi, storage, sensors, RTC, servo, pins, and routesloop()Runs server, sensor monitoring, alerts, scheduled feeding, and button feeding

Web Server Routes
RoutePurpose/loginDisplays login page/doLoginValidates login credentials/Displays dashboard after login/dataSends feed count, logs, and saved settings as JSON/timeSends RTC date and time as JSON/sensorSends temperature, water level, alert state, and reason as JSON/saveSaves schedule and threshold settings/feedTriggers manual feeding from dashboard

Dashboard Data Flow
The dashboard uses JavaScript fetch() requests to get live data from the ESP32.
Examples:
/data     -> feed count, logs, thresholds/time     -> RTC date and time/sensor   -> temperature, water level, alert state/feed     -> manual feeding action/save     -> save schedules and thresholds
This allows the dashboard to update without refreshing the whole page.

System Workflow


The ESP32 is powered on.


ESP32 starts the SmartAquarium Wi-Fi hotspot.


The ESP32 starts the local web server.


The user connects a phone or laptop to the hotspot.


The user opens the dashboard at 192.168.4.1.


The user logs in using the dashboard credentials.


ESP32 reads temperature and water-level data.


The dashboard displays live sensor readings.


ESP32 checks whether values are inside safe threshold ranges.


If unsafe values are detected, buzzer and LED alerts are triggered.


ESP32 checks RTC time for scheduled feeding.


If scheduled time is reached, the servo feeder dispenses food.


User can also trigger manual feeding from the dashboard or button.


Feeding events and settings are stored locally using Preferences/NVS.


The system continues running while powered on.



Alert Logic
The system checks the following conditions:
if temperature < minimum temperature:    TEMP LOW alertif temperature > maximum temperature:    TEMP HIGH alertif water level < minimum level:    LEVEL LOW alertif water level > maximum level:    LEVEL HIGH alert
When an alert is active:


LED turns on


buzzer beeps repeatedly


dashboard shows alert message


alert reason is displayed


When conditions return to normal:


buzzer turns off


LED turns off


dashboard returns to OK status



Feeding Logic
The feeding system supports both scheduled and manual feeding.
Scheduled Feeding
The user can set two feeding schedules from the dashboard.
The ESP32 compares the current RTC time with saved schedule values.
If the time matches:


servo rotates


food is dispensed


event is logged as RTC1 or RTC2


Manual Feeding
Manual feeding can be triggered by:


dashboard Feed Now button


physical button connected to GPIO 27


Manual events are logged as:
ManualBUTTON
Cooldown Protection
A cooldown period is used to prevent repeated feeding.
Cooldown: 20 seconds
This helps reduce accidental overfeeding.

Local Storage
The project uses ESP32 Preferences/NVS.
Saved values include:
Saved ValuePurposeh1, m1, s1Feeding schedule 1h2, m2, s2Feeding schedule 2minTempMinimum safe temperaturemaxTempMaximum safe temperatureminLevelMinimum safe water levelmaxLevelMaximum safe water levelcountFeed countlogsActivity/feed log
This makes the system remember settings after restart.

Testing Evidence Planned / Used
The project can be evaluated using the following evidence:


dashboard login screenshot


dashboard live status screenshot


temperature reading screenshot


water-level reading screenshot


alert state screenshot


manual feeding test screenshot


scheduled feeding log screenshot


threshold settings screenshot


activity log screenshot


physical prototype photo


buzzer/LED alert test evidence


servo feeding test evidence



Evaluation Criteria
The system can be evaluated using:
Evaluation AreaMeasurementFeeding reliabilitySuccessful scheduled feeds vs expected feedsManual feedingDashboard/button feed responseSensor accuracyDS18B20 compared with reference thermometerWater-level detectionPercentage change and alert responseAlert responseTime taken to activate buzzer/LEDDashboard usabilityDashboard loads and updates from phone/laptopStorage reliabilitySettings/logs remain after restartSystem stabilityESP32 runs without unexpected failure during testing

Current Final Prototype Status
The final prototype includes:


ESP32 Access Point mode


local dashboard login


live temperature display


live water-level percentage display


alert banner and OK/ALERT status


buzzer and LED alerts


dashboard Feed Now button


physical manual feed button


two RTC-based feeding schedules


temperature and water-level threshold settings


local activity/feed logs


Preferences/NVS storage


complete embedded frontend and firmware in one .ino file



Limitations
The current prototype has some limitations:


The dashboard works locally through ESP32 hotspot and is not cloud-hosted.


Telegram alerts were not activated in the final prototype because the ESP32 hotspot mode does not provide internet access to reach Telegram servers.


The system does not use a traditional database.


Water-level readings depend on correct ultrasonic sensor placement and tank height calibration.


The prototype is built for a small aquarium demonstration setup.


Long-term enclosure and waterproofing can be improved further.



Future Enhancements
Possible future improvements include:


cloud dashboard or remote access


Telegram or mobile push alerts with internet-connected Wi-Fi mode


better physical enclosure


improved cable management


export logs as CSV or text


pH sensor integration


turbidity sensor integration


mobile app version


more advanced feeding quantity control


improved user management/security



How to Run the Project
Requirements


ESP32 development board


Arduino IDE


ESP32 board package installed


Required libraries installed


USB data cable


connected sensors and output devices


Steps


Open Arduino IDE.


Open the file:


Smart_Aquarium_Guardian.ino


Select the correct ESP32 board.


Select the correct port.


Install required libraries if missing.


Upload the code to ESP32.


Power the ESP32.


Connect phone/laptop Wi-Fi to:


SmartAquarium


Open browser and go to:


192.168.4.1


Login using the dashboard credentials.


Monitor temperature, water level, alerts, schedules, and feeding logs.



Repository Structure

smart-aquarium-guardian│├── firmware│   └── Smart_Aquarium_Guardian.ino│├── docs│   ├── diagrams│   ├── requirements│   └── reports│├── evidence│   ├── screenshots│   └── testing│├── submission│   └── component-2│└── README.md

Important Academic Note
This project is an embedded IoT prototype. It should not be treated like a traditional web application with a PHP/Python backend and external database.
The correct interpretation is:
Frontend:HTML, CSS and JavaScript dashboard served by ESP32Backend:ESP32 Arduino C/C++ firmware and WebServer logicDatabase:No traditional databaseLocal storage is handled using ESP32 Preferences/NVS

Summary
Smart Aquarium Guardian is a local-first ESP32 IoT prototype that supports aquarium feeding and safety monitoring. It combines embedded programming, sensor monitoring, actuator control, local dashboard design, alert handling, and local data storage.
The project demonstrates how an ESP32 can be used as both a hardware controller and a local web server to provide a practical smart aquarium support system without depending on cloud services.
Use this as your **main GitHub README**.For your **Implementation folder README**, keep it shorter because that one only explains the implementation folder. The GitHub README can be detailed like this.


Install a simple server


npm install -g http-server




cd smart-aquarium-guardian



http-server -p 8000


http://localhost:8000
