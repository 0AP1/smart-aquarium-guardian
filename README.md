# Smart Aquarium Guardian (IoT based Project)

## Project overview
Smart Aquarium Guardian is an ESP32-based local-first IoT system for a fish bowl aquarium.  
It monitors temperature and water level, provides local alerts, and automates feeding on a schedule.  
The dashboard is hosted directly on the ESP32 (works without internet).

## Core features
- Automated feeding (scheduled + manual feed)
- DS18B20 waterproof temperature monitoring
- Low water level + high water level detection (float switches)
- Local alerts (buzzer + LED)
- ESP32 device-hosted Wi-Fi dashboard
- Event logging for feeding and alerts

## Hardware (planned pin mapping)
- DS18B20 DATA → GPIO 4 (with 4.7k pull-up to 3.3V)
- Low level sensor → GPIO 32
- High level sensor → GPIO 33
- Servo motor signal → GPIO 18 (servo power via 5V)
- Buzzer → GPIO 25
- LED → GPIO 26

## Repository structure
- `firmware/esp32/` → ESP32 code
- `dashboard/ui/` → dashboard frontend (HTML/CSS/JS)
- `docs/` → meeting logs, diagrams, wireframes, sprint plan
- `hardware/` → wiring notes and datasheets
- `evaluation/` → test plan and results evidence
- `evidence/` → weekly screenshots/photos/logs

## How to run (placeholder for now)
- Firmware: Coming soon (Week 4)
- Dashboard: Static UI mock in `dashboard/ui/` (Week 3)

## Author
Aanchal Poudel (77466854)
