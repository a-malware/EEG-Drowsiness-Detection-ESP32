# Calibration Guide

This guide explains how to calibrate the EEG Drowsiness Detection and Safety System for optimal performance.

## Calibration Process
1. **EEG Calibration**:
   - Press and hold the calibration button (GPIO 0) during startup or use the Serial Monitor command `calibrate` in `main.ino`.
   - Sit comfortably and stay alert for 30 seconds while the system records baseline attention levels.
   - The LED will blink to indicate progress, and a completion message will appear in the Serial Monitor.

2. **MPU6050 Calibration**:
   - Upload `src/utilities/sensor_calibrator.ino` to the ESP32.
   - Open the Serial Monitor and type `calibrate_mpu`.
   - Keep the device stationary for 10 seconds. The script will output baseline acceleration and gyroscope values, suggesting initial thresholds (e.g., ACCEL_THRESHOLD = baseline + 0.5g, GYRO_THRESHOLD = baseline + 50°/s).
   - Update these values in `main.ino`.

3. **MQ2 Calibration**:
   - With `src/utilities/sensor_calibrator.ino` uploaded, type `calibrate_mq2` in the Serial Monitor.
   - Expose the MQ2 sensor to clean air for 10 seconds. The script will output a baseline PPM value, suggesting a threshold (e.g., baseline + 50 ppm).
   - Update the threshold in `main.ino`.

## Post-Calibration
- Verify thresholds by inducing controlled drowsiness, crashes, or smoke exposure and checking alert triggers.
- Save calibrated values using the Preferences library if needed for persistence across reboots.
- Refer to `troubleshooting.md` if alerts are inconsistent.

## Notes
- Calibration should be repeated if hardware is moved or environmental conditions change.
- Ensure a stable power supply during calibration to avoid noise.