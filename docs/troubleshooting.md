# Troubleshooting Guide

This guide helps resolve common issues with the EEG Drowsiness Detection and Safety System.

## Common Issues and Solutions
- **MindLink EEG Not Connecting**:
  - Ensure the MindLink device is powered and in pairing mode.
  - Verify `MINDLINK_DEVICE_NAME`, `MINDLINK_SERVICE_UUID`, and `MINDLINK_CHARACTERISTIC_UUID` in `main.ino` match your device.
  - Check Bluetooth interference and restart the ESP32.

- **No GPS Fix**:
  - Ensure the NEO-6M has a clear view of the sky and is powered (3.3V).
  - Wait 30-60 seconds for an initial fix; add a blinking LED indicator in `setup()` if needed.
  - Check wiring (TX to GPIO 17, RX to GPIO 16) and baud rate (9600).

- **GSM Alerts Not Sending**:
  - Verify the SIM800L has a valid SIM card with an active plan.
  - Check network coverage and antenna connection.
  - Ensure correct pin wiring (TX to GPIO 19, RX to GPIO 18) and add debug prints in `GSMHandler.cpp` to trace AT commands.
  - Retry sending if "OK" is not received after 1000ms.

- **MPU6050 or MQ2 Not Responding**:
  - Confirm I2C connections (SDA to GPIO 21, SCL to GPIO 22 for MPU6050; A0 for MQ2).
  - Recalibrate using `src/utilities/sensor_calibrator.ino` and adjust thresholds if baseline values are off.
  - Test with a multimeter to ensure power and ground are stable.

- **Alerts Not Triggering**:
  - Check calibration thresholds in `main.ino` (e.g., `ACCEL_THRESHOLD`, attention threshold).
  - Ensure LED and buzzer are correctly wired and not damaged.
  - Review Serial Monitor output for attention levels or sensor readings.

## Advanced Debugging
- Enable verbose output in the Arduino IDE to see compilation details.
- Use an oscilloscope to check signal integrity on I2C or analog pins.
- Consult `hardware/connections.md` for wiring verification.

## Contact
For persistent issues, open an issue on the GitHub repository or contact me at [amalanand@duck.com].