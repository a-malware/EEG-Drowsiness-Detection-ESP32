# Hardware Connections

This document details the wiring for the EEG Drowsiness Detection and Safety System.

## Connection Diagram
- **ESP32**:
  - 3.3V to VCC of MPU6050, GPS, and LED (via resistor)
  - GND to GND of all modules
  - GPIO 21 (SDA) to SDA of MPU6050
  - GPIO 22 (SCL) to SCL of MPU6050
  - GPIO 2 to LED anode (cathode to GND via 220Ω resistor)
  - GPIO 4 to Buzzer positive (negative to GND)
  - GPIO 0 to one pin of Push Button (other pin to GND)
  - GPIO A0 to AOUT of MQ2
  - GPIO 17 (TX2) to RX of NEO-6M
  - GPIO 16 (RX2) to TX of NEO-6M
  - GPIO 19 to RX of SIM800L
  - GPIO 18 to TX of SIM800L

- **MindLink EEG**: Connects via Bluetooth (no physical wiring).

## Power Considerations
- NEO-6M: 3.3V supply with a 3.3V-5V level shifter if needed.
- SIM800L: 3.7-4.2V supply (e.g., LiPo battery) with a 2A capable source.
- MQ2: 5V supply for optimal sensitivity.

## Notes
- Ensure I2C pull-up resistors (4.7kΩ) are present on SDA/SCL lines if not built into MPU6050.
- Use a breadboard or PCB for stable connections during testing.
- Refer to `parts-list.md` for component specifications.