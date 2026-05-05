# Smart Aquarium Guardian

## IoT-Based Automated Feeding with Temperature and Water-Level Alerts

Smart Aquarium Guardian is an ESP32-based IoT aquarium support system developed as a Level 6 Production Project. The system automates fish feeding, monitors aquarium water temperature, measures water level, triggers local buzzer and LED alerts for unsafe conditions, and provides a local web dashboard for user interaction.

The project is designed as a local-first IoT prototype. Instead of depending on cloud services, the ESP32 creates its own Wi-Fi hotspot and hosts the dashboard directly on the device. A user can connect a phone or laptop to the ESP32 hotspot, open the local dashboard in a browser, view live aquarium status, update settings, and manually trigger feeding.

## Project Title

Smart Aquarium Guardian: IoT-Based Automated Feeding with Temperature and Water-Level Alerts

## Author

Aanchal Poudel  
BSc (Hons) Computing  
Level 6 Production Project  
The British College / Leeds Beckett University

## Project Overview

Aquarium care requires regular feeding and continuous monitoring of safe water conditions. In real life, fish feeding can become difficult when the owner is away from home. Unsafe water temperature or water-level conditions may also go unnoticed without regular checking.

Smart Aquarium Guardian addresses this problem by combining:

- automatic fish feeding
- manual feed control
- water temperature monitoring
- water-level monitoring
- buzzer and LED alerts
- ESP32-hosted local dashboard
- local settings and activity log storage

The ESP32 works as the main controller. It reads sensor values, controls the servo feeder, activates alerts, stores settings and logs locally, and hosts the dashboard through its own Wi-Fi hotspot.

## Aim of the Project

The aim of this project is to design and build an IoT-based aquarium support system that automates fish feeding using a servo-driven dispensing mechanism and monitors basic aquarium safety conditions. The system measures water temperature and water level, triggers local alerts using a buzzer and LED when unsafe conditions are detected, and displays live system information through a device-hosted local web dashboard.

## Main Objectives

The main objectives of this project are:

- To build an ESP32-based smart aquarium support prototype.
- To automate fish feeding using a servo motor feeder.
- To support scheduled feeding using RTC-based time.
- To provide manual feeding through both the dashboard and a physical button.
- To monitor water temperature using a DS18B20 waterproof temperature sensor.
- To monitor water level using an ultrasonic water-level sensor.
- To trigger buzzer and LED alerts when water temperature or water level goes outside safe thresholds.
- To provide a local dashboard hosted by the ESP32.
- To allow users to configure feeding schedules and alert thresholds from the dashboard.
- To store settings, feed count, and activity logs locally using ESP32 Preferences/NVS.
- To support testing and evaluation using screenshots, logs, and prototype evidence.

## Key Features

### 1. ESP32 Local Hotspot

The ESP32 creates its own Wi-Fi hotspot.

```text
SSID: SmartAquarium
Local IP Address: 192.168.4.1
```

The user connects a phone or laptop to this hotspot and opens the dashboard using a web browser.

### 2. Login System

The system includes a simple login page before the dashboard can be accessed.

Prototype login credentials used in the final implementation:

```text
Username: aanchal
Password: tbc@2026
```

For a real-world version, these credentials should be changed and improved with stronger authentication.

### 3. Local Web Dashboard

The dashboard is hosted directly by the ESP32 WebServer.

The dashboard displays:

- RTC date and time
- water temperature
- water-level percentage
- OK or ALERT status
- active alert reason
- total feed count
- schedule settings
- temperature threshold settings
- water-level threshold settings
- feed/activity log
- manual Feed Now button

The dashboard is written using embedded HTML, CSS, and JavaScript inside the Arduino `.ino` file.

### 4. Automatic Feeding

The system supports two scheduled feeding times.

The DS3231 RTC module provides real-time clock data. The ESP32 checks whether the current time matches the saved feeding schedule.

When a scheduled feeding time is reached:

- the servo motor rotates
- fish food is dispensed
- feed count is updated
- the event is saved in the activity log

A cooldown mechanism is used to prevent repeated feeding within a short time.

### 5. Manual Feeding

Manual feeding is supported in two ways:

- from the dashboard using the Feed Now button
- from the physical manual feed button connected to the ESP32

Manual feed actions also use a cooldown delay to reduce the risk of accidental repeated feeding.

### 6. Temperature Monitoring

The DS18B20 waterproof temperature sensor is used to monitor aquarium water temperature.

The temperature value is shown on the dashboard in degrees Celsius.

The user can set:

- minimum safe temperature
- maximum safe temperature

If the temperature goes outside the configured safe range, the system triggers an alert.

### 7. Water-Level Monitoring

The final implementation uses an ultrasonic water-level sensor to measure water level as a percentage.

The ultrasonic sensor uses:

```text
TRIG: GPIO 12
ECHO: GPIO 13
```

The measured distance is converted into water-level percentage based on the prototype tank height.

The user can set:

- minimum safe water-level percentage
- maximum safe water-level percentage

If the water level goes below or above the configured safe range, the system triggers an alert.

### 8. Buzzer and LED Alerts

The system uses local alert outputs:

```text
Buzzer: GPIO 19
LED: GPIO 25
```

Alerts are triggered when:

- water temperature is too low
- water temperature is too high
- water level is too low
- water level is too high

Example alert messages:

```text
TEMP LOW
TEMP HIGH
LEVEL LOW
LEVEL HIGH
```

The alert reason is also displayed on the dashboard.

### 9. Local Settings and Logs

The project does not use a traditional database such as MySQL, MongoDB, Firebase, or SQLite.

Instead, settings and logs are stored locally on the ESP32 using:

```text
Preferences / NVS
```

Stored data includes:

- feeding schedule 1
- feeding schedule 2
- minimum and maximum temperature thresholds
- minimum and maximum water-level thresholds
- feed count
- activity logs

This supports the local-first design and allows the prototype to work without internet access.

## Final Implementation Note

The original project specification planned to use two float switches for low and high water-level detection.

However, in the final prototype, an ultrasonic water-level sensor was used instead. This change was made because the available float switch was too large for the small 5 to 6 litre prototype aquarium.

This was a practical engineering adaptation, not a mistake.

The ultrasonic sensor was more suitable because:

- it is non-contact
- it fits the small prototype aquarium better
- it measures water level as a percentage
- it still supports unsafe water-level detection
- it meets the core requirement of water-level monitoring and alerting

## Hardware Components

| Component | Purpose |
|---|---|
| ESP32 Development Board | Main controller, Wi-Fi hotspot, local web server, sensor reading, and actuator control |
| DS18B20 Waterproof Temperature Sensor | Measures aquarium water temperature |
| Ultrasonic Water-Level Sensor | Measures water level percentage |
| DS3231 RTC Module | Provides real-time clock data for scheduled feeding |
| Servo Motor | Rotates feeder mechanism to dispense fish food |
| Manual Feed Button | Allows physical manual feeding |
| Buzzer | Provides audible alert for unsafe conditions |
| LED | Provides visual alert for unsafe conditions |
| Jumper Wires / Breadboard | Used for prototype wiring and circuit connections |
| Power Supply / USB Cable | Powers ESP32 and supports firmware upload |
| Aquarium Prototype | Small 5 to 6 litre aquarium setup used for demonstration |

## GPIO Pin Configuration

| Hardware | ESP32 Pin |
|---|---|
| DS18B20 Temperature Sensor | GPIO 4 |
| Ultrasonic Sensor TRIG | GPIO 12 |
| Ultrasonic Sensor ECHO | GPIO 13 |
| Servo Motor Feeder | GPIO 18 |
| Buzzer | GPIO 19 |
| LED | GPIO 25 |
| Manual Feed Button | GPIO 27 |
| DS3231 RTC | I2C |

## Software and Libraries

The project is written in Arduino-style Embedded C/C++ for ESP32.

Main libraries used:

```cpp
#include <WiFi.h>
#include <WebServer.h>
#include <Preferences.h>
#include <Wire.h>
#include "RTClib.h"
#include <ESP32Servo.h>
#include <OneWire.h>
#include <DallasTemperature.h>
```

### Library Purposes

| Library | Purpose |
|---|---|
| WiFi.h | Creates ESP32 Access Point hotspot |
| WebServer.h | Hosts local dashboard and API routes |
| Preferences.h | Stores settings and logs in ESP32 NVS |
| Wire.h | Supports I2C communication |
| RTClib.h | Reads DS3231 RTC date and time |
| ESP32Servo.h | Controls servo feeder motor |
| OneWire.h | Supports DS18B20 sensor communication |
| DallasTemperature.h | Reads DS18B20 temperature data |

## Project Architecture

The system follows a local embedded IoT architecture.

```text
User Phone / Laptop
        |
        | connects to ESP32 hotspot
        |
ESP32 Access Point
        |
ESP32 Web Server and Dashboard
        |
ESP32 Firmware Logic
        |
-----------------------------------------
| Sensor Monitoring | Feeding | Alerts |
-----------------------------------------
        |
Preferences / NVS Local Storage
```

The ESP32 acts as the central controller.

It performs the following roles:

- creates a Wi-Fi hotspot
- hosts the web dashboard
- reads temperature data
- reads water-level data
- reads RTC time
- receives manual feed button input
- controls servo feeding
- controls buzzer and LED alerts
- stores settings and logs locally

## Frontend and Backend Explanation

This project has both frontend and backend, but in an embedded IoT form.

### Frontend

The frontend is the dashboard that the user sees in the browser.

It is written using:

- HTML
- CSS
- JavaScript

It includes:

- login page
- dashboard interface
- sensor cards
- feeding controls
- schedule inputs
- threshold settings
- alert banner
- activity log display

### Backend

The backend is the ESP32 firmware and WebServer logic.

It is written in Arduino-style Embedded C/C++.

It handles:

- route handling
- login checking
- sensor reading
- feeding logic
- alert logic
- schedule checking
- Preferences/NVS storage
- JSON API responses for dashboard updates

The backend is not a cloud server. It runs inside the ESP32.

## Main Source Code File

The main implementation file is:

```text
Smart_Aquarium_Guardian.ino
```

This file includes:

- all ESP32 firmware logic
- embedded dashboard frontend
- sensor reading functions
- alert checking functions
- feeding functions
- server routes
- storage handling
- setup and loop logic

## Important Functions in the Code

| Function | Purpose |
|---|---|
| `timeStr()` | Converts RTC time into readable HH:MM:SS format |
| `dateStr()` | Converts RTC date into readable YYYY-MM-DD format |
| `dateTimeStr()` | Creates timestamp for logs |
| `saveLog()` | Stores feeding and alert-related events in Preferences/NVS |
| `feedFish()` | Rotates servo feeder and records feeding event |
| `readSensors()` | Reads DS18B20 temperature and ultrasonic water-level data |
| `checkAlerts()` | Checks temperature and water level against thresholds |
| `handleLogin()` | Serves login page |
| `handleDoLogin()` | Checks dashboard username and password |
| `handleRoot()` | Serves main dashboard page |
| `handleData()` | Sends feed count, logs, and settings to dashboard |
| `handleTime()` | Sends RTC date and time to dashboard |
| `handleSensor()` | Sends temperature, water level, and alert state |
| `handleSave()` | Saves updated schedules and thresholds |
| `setup()` | Initialises Wi-Fi, storage, sensors, RTC, servo, pins, and routes |
| `loop()` | Runs server, sensor monitoring, alerts, scheduled feeding, and button feeding |

## Web Server Routes

| Route | Purpose |
|---|---|
| `/login` | Displays login page |
| `/doLogin` | Validates login credentials |
| `/` | Displays dashboard after login |
| `/data` | Sends feed count, logs, and saved settings as JSON |
| `/time` | Sends RTC date and time as JSON |
| `/sensor` | Sends temperature, water level, alert state, and reason as JSON |
| `/save` | Saves schedule and threshold settings |
| `/feed` | Triggers manual feeding from dashboard |

## Dashboard Data Flow

The dashboard uses JavaScript `fetch()` requests to get live data from the ESP32.

```text
/data     -> feed count, logs, thresholds
/time     -> RTC date and time
/sensor   -> temperature, water level, alert state
/feed     -> manual feeding action
/save     -> save schedules and thresholds
```

This allows the dashboard to update without refreshing the full page.

## System Workflow

1. The ESP32 is powered on.
2. ESP32 starts the `SmartAquarium` Wi-Fi hotspot.
3. The ESP32 starts the local web server.
4. The user connects a phone or laptop to the hotspot.
5. The user opens the dashboard at `192.168.4.1`.
6. The user logs in using the dashboard credentials.
7. ESP32 reads temperature and water-level data.
8. The dashboard displays live sensor readings.
9. ESP32 checks whether values are inside safe threshold ranges.
10. If unsafe values are detected, buzzer and LED alerts are triggered.
11. ESP32 checks RTC time for scheduled feeding.
12. If scheduled time is reached, the servo feeder dispenses food.
13. The user can also trigger manual feeding from the dashboard or button.
14. Feeding events and settings are stored locally using Preferences/NVS.
15. The system continues running while powered on.

## Alert Logic

The system checks the following conditions:

```text
if temperature < minimum temperature:
    TEMP LOW alert

if temperature > maximum temperature:
    TEMP HIGH alert

if water level < minimum level:
    LEVEL LOW alert

if water level > maximum level:
    LEVEL HIGH alert
```

When an alert is active:

- LED turns on
- buzzer beeps repeatedly
- dashboard shows alert message
- alert reason is displayed

When conditions return to normal:

- buzzer turns off
- LED turns off
- dashboard returns to OK status

## Feeding Logic

The feeding system supports both scheduled and manual feeding.

### Scheduled Feeding

The user can set two feeding schedules from the dashboard.

The ESP32 compares the current RTC time with saved schedule values.

If the time matches:

- servo rotates
- food is dispensed
- event is logged as `RTC1` or `RTC2`

### Manual Feeding

Manual feeding can be triggered by:

- dashboard Feed Now button
- physical button connected to GPIO 27

Manual events are logged as:

```text
Manual
BUTTON
```

### Cooldown Protection

A cooldown period is used to prevent repeated feeding.

```text
Cooldown: 20 seconds
```

This helps reduce accidental overfeeding.

## Local Storage

The project uses ESP32 Preferences/NVS.

Saved values include:

| Saved Value | Purpose |
|---|---|
| `h1`, `m1`, `s1` | Feeding schedule 1 |
| `h2`, `m2`, `s2` | Feeding schedule 2 |
| `minTemp` | Minimum safe temperature |
| `maxTemp` | Maximum safe temperature |
| `minLevel` | Minimum safe water level |
| `maxLevel` | Maximum safe water level |
| `count` | Feed count |
| `logs` | Activity/feed log |

This makes the system remember settings after restart.

## Testing Evidence

The project testing evidence includes:

- dashboard login screenshot
- dashboard live status screenshot
- temperature reading screenshot
- water-level reading screenshot
- alert state screenshot
- manual feeding test screenshot
- scheduled feeding log screenshot
- threshold settings screenshot
- activity log screenshot
- physical prototype photo
- buzzer/LED alert test evidence
- servo feeding test evidence

## Evaluation Criteria

| Evaluation Area | Measurement |
|---|---|
| Feeding reliability | Successful scheduled feeds compared with expected feeds |
| Manual feeding | Dashboard and physical button feed response |
| Sensor accuracy | DS18B20 compared with reference thermometer |
| Water-level detection | Percentage change and alert response |
| Alert response | Time taken to activate buzzer and LED |
| Dashboard usability | Dashboard loads and updates from phone/laptop |
| Storage reliability | Settings and logs remain after restart |
| System stability | ESP32 runs without unexpected failure during testing |

## Current Final Prototype Status

The final prototype includes:

- ESP32 Access Point mode
- local dashboard login
- live temperature display
- live water-level percentage display
- alert banner and OK/ALERT status
- buzzer and LED alerts
- dashboard Feed Now button
- physical manual feed button
- two RTC-based feeding schedules
- temperature and water-level threshold settings
- local activity/feed logs
- Preferences/NVS storage
- complete embedded frontend and firmware in one `.ino` file

## Limitations

The current prototype has some limitations:

- The dashboard works locally through ESP32 hotspot and is not cloud-hosted.
- Telegram alerts were not activated in the final prototype because the ESP32 hotspot mode does not provide internet access to reach Telegram servers.
- The system does not use a traditional database.
- Water-level readings depend on correct ultrasonic sensor placement and tank height calibration.
- The prototype is built for a small aquarium demonstration setup.
- Long-term enclosure and waterproofing can be improved further.

## Future Enhancements

Possible future improvements include:

- cloud dashboard or remote access
- Telegram or mobile push alerts with internet-connected Wi-Fi mode
- better physical enclosure
- improved cable management
- export logs as CSV or text
- pH sensor integration
- turbidity sensor integration
- mobile app version
- advanced feeding quantity control
- improved user management and security

## How to Run the Project

### Requirements

- ESP32 development board
- Arduino IDE
- ESP32 board package installed
- required libraries installed
- USB data cable
- connected sensors and output devices

### Steps

1. Open Arduino IDE.
2. Open the file `Smart_Aquarium_Guardian.ino`.
3. Select the correct ESP32 board.
4. Select the correct port.
5. Install required libraries if missing.
6. Upload the code to ESP32.
7. Power the ESP32.
8. Connect phone/laptop Wi-Fi to `SmartAquarium`.
9. Open browser and go to `192.168.4.1`.
10. Login using the dashboard credentials.
11. Monitor temperature, water level, alerts, schedules, and feeding logs.

## Repository Structure

```text
smart-aquarium-guardian
|
├── firmware
|   └── Smart_Aquarium_Guardian.ino
|
├── docs
|   ├── diagrams
|   ├── requirements
|   └── reports
|
├── evidence
|   ├── screenshots
|   └── testing
|
├── submission
|   └── component-2
|
└── README.md
```

## Important Academic Note

This project is an embedded IoT prototype. It should not be treated like a traditional web application with a PHP/Python backend and an external database.

The correct interpretation is:

```text
Frontend:
HTML, CSS and JavaScript dashboard served by ESP32

Backend:
ESP32 Arduino C/C++ firmware and WebServer logic

Database:
No traditional database
Local storage is handled using ESP32 Preferences/NVS
```

## Summary

Smart Aquarium Guardian is a local-first ESP32 IoT prototype that supports aquarium feeding and safety monitoring. It combines embedded programming, sensor monitoring, actuator control, local dashboard design, alert handling, and local data storage.

The project demonstrates how an ESP32 can be used as both a hardware controller and a local web server to provide a practical smart aquarium support system without depending on cloud services.

