# EEG-Drowsiness-Detection-ESP32  
Drowsiness detection for drivers using the ESP32 microcontroller.

This repository contains the source code and documentation for a real-time EEG-based drowsiness detection and IoT-enabled vehicle safety system implemented on the ESP32 microcontroller. Presented at ICCC 2025 (May 23–25, 2025), this project uses a MindLink EEG headset, Kalman filtering, and FFT analysis to detect driver drowsiness, integrates MPU6050 for crash detection, MQ2 for smoke/fire detection, NEO-6M GPS for location tracking, and SIM800L GSM for emergency alerts.

---

## Citation
Please cite the associated research paper when using or referencing this work:

**Drowsiness Detection Using ESP32-based EEG and IoT System**, ICCC 2025.  
[IEEE Xplore Document](https://ieeexplore.ieee.org/document/11077179)

---

## Features
- Real-time drowsiness detection with 92% accuracy using EEG signals.  
- Crash detection with 95% accuracy using MPU6050.  
- Smoke/fire detection using MQ2 sensor.  
- Emergency SMS alerts with GPS coordinates via GSM.  
- LED and buzzer alerts for immediate feedback.  
- Calibration system for personalized thresholds.

## Getting Started
1. **Hardware Setup**: See `docs/hardware-setup.md` for wiring instructions (ESP32, MindLink, GPS, GSM, MPU6050, MQ2).  
2. **Software Setup**: Install Arduino IDE, add ESP32 board support, and install libraries listed in `libraries/requirements.txt`.  
3. **Upload Code**: Open `src/main/main.ino`, configure pins/UUIDs, and upload to the ESP32.  
4. **Calibration**: Use `src/utilities/sensor_calibrator.ino` to set thresholds for MPU6050, MQ2, and calibrate EEG via the button.

## Directory Structure
- `docs/`: Hardware setup, calibration, and troubleshooting guides.  
- `src/`: Main code, utilities, and examples.  
- `libraries/`: Arduino library dependencies.  
- `hardware/`: Wiring diagrams and parts list.  
- `tests/`: Unit tests for components.  
- `images/`: Screenshots and diagrams (add your own).

## License
This project is licensed under the MIT License—see the [LICENSE](LICENSE) file for details.

