# Hardware Setup Guide

This guide outlines the hardware setup for the EEG Drowsiness Detection and IoT-Enabled Vehicle Safety System on the ESP32.

## Required Components
- ESP32 Development Board
- MindLink EEG Headset (with Bluetooth)
- NEO-6M GPS Module
- SIM800L GSM Module (with SIM card)
- MPU6050 Accelerometer/Gyroscope Module
- MQ2 Gas/Smoke Sensor
- LED (for visual alerts)
- Buzzer (for audio alerts)
- Push Button (for calibration)
- Jumper wires and breadboard
- 3.3V/5V power supply

## Wiring Diagram
- **MindLink EEG**: Connect via Bluetooth (no physical pins required).
- **GPS (NEO-6M)**:
  - VCC to 3.3V
  - GND to GND
  - TX to ESP32 GPIO 17 (RX2)
  - RX to ESP32 GPIO 16 (TX2)
- **GSM (SIM800L)**:
  - VCC to 3.7-4.2V
  - GND to GND
  - TX to ESP32 GPIO 19
  - RX to ESP32 GPIO 18
- **MPU6050**:
  - VCC to 3.3V
  - GND to GND
  - SDA to ESP32 GPIO 21 (I2C SDA)
  - SCL to ESP32 GPIO 22 (I2C SCL)
- **MQ2 Sensor**:
  - VCC to 5V
  - GND to GND
  - AOUT to ESP32 GPIO A0
- **LED**:
  - Anode to ESP32 GPIO 2
  - Cathode to GND (via resistor)
- **Buzzer**:
  - Positive to ESP32 GPIO 4
  - Negative to GND
- **Push Button**:
  - One pin to ESP32 GPIO 0
  - Other pin to GND (with pull-up resistor internally configured)

## Setup Instructions
1. Assemble the circuit according to the wiring diagram.
2. Ensure the GPS module has a clear view of the sky for a fix.
3. Insert a SIM card into the GSM module and verify network coverage.
4. Power the ESP32 and upload the code via the Arduino IDE.
5. Open the Serial Monitor (115200 baud) to verify connections and calibration.

## Notes
- Adjust pin assignments in `main.ino` if they conflict with your setup.
- Calibrate sensors using `src/utilities/sensor_calibrator.ino` before deployment.
- Refer to `hardware/connections.md` in the hardware directory for detailed diagrams.