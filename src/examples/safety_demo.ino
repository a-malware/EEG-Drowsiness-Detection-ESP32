/*
 * Safety Demo for EEG Drowsiness and Vehicle Safety System
 * 
 * This example demonstrates integrated drowsiness, crash, and fire/smoke detection
 * with alerts using the ESP32-based system.
 */

 #include <Arduino.h>
 #include "GPSHandler.h"
 #include "GSMHandler.h"
 #include "KalmanEEG.h"
 #include "AttentionCalculator.h"
 
 #define LED_PIN 2
 #define BUZZER_PIN 4
 #define BUTTON_PIN 0
 #define MQ2_PIN A0
 
 HardwareSerial Serial2(2);  // GPS on Serial2
 GPSHandler gps(Serial2);
 SoftwareSerial gsmSerial(18, 19);  // GSM on pins 18/19
 GSMHandler gsm(gsmSerial);
 KalmanEEG kalmanFilter;
 AttentionCalculator attentionCalc;
 
 const int SAMPLES = 128;
 float eegBuffer[SAMPLES];
 int bufferIndex = 0;
 
 MPU6050 mpu;
 const float ACCEL_THRESHOLD = 1.5;
 const float GYRO_THRESHOLD = 200;
 
 void setup() {
     Serial.begin(115200);
     Serial.println("Safety Demo Starting...");
 
     pinMode(LED_PIN, OUTPUT);
     pinMode(BUZZER_PIN, OUTPUT);
     pinMode(BUTTON_PIN, INPUT_PULLUP);
 
     Wire.begin();
     mpu.initialize();
     if (!mpu.testConnection()) Serial.println("MPU6050 connection failed!");
 
     gps.begin();
     gsm.begin();
 
     kalmanFilter.setParameters(0.1, 0.1);
     attentionCalc.calibrate(NULL);  // Simple calibration for demo
 
     Serial.println("System ready for demo!");
 }
 
 void loop() {
     // Simulate EEG data (replace with actual BLE data in full system)
     float rawEEG[3] = {10.0, 12.0, 11.0};  // Example values
     float filteredEEG[3];
     kalmanFilter.filterEEG(rawEEG, filteredEEG);
     eegBuffer[bufferIndex] = (filteredEEG[0] + filteredEEG[1] + filteredEEG[2]) / 3.0;
     bufferIndex = (bufferIndex + 1) % SAMPLES;
 
     if (bufferIndex == 0) {
         float attention = attentionCalc.calculateAttention(eegBuffer, SAMPLES);
         Serial.printf("Attention: %.2f%%\n", attention);
         if (attention < attentionCalc.getThreshold()) triggerAlert("Drowsiness");
     }
 
     // Simulate crash detection
     int16_t ax, ay, az, gx, gy, gz;
     mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
     float accelMagnitude = sqrt((ax / 16384.0) * (ax / 16384.0) + (ay / 16384.0) * (ay / 16384.0) + ((az / 16384.0) - 1.0) * ((az / 16384.0) - 1.0));
     float gyroMagnitude = sqrt((gx / 131.0) * (gx / 131.0) + (gy / 131.0) * (gy / 131.0) + (gz / 131.0) * (gz / 131.0));
     if (accelMagnitude > ACCEL_THRESHOLD || gyroMagnitude > GYRO_THRESHOLD) triggerAlert("Crash");
 
     // Simulate fire detection
     int mq2Value = analogRead(MQ2_PIN);
     if (mq2Value * (5000.0 / 1023.0) > 200) triggerAlert("Fire/Smoke");
 
     delay(100);
 }
 
 void triggerAlert(const char* alertType) {
     Serial.print(alertType);
     Serial.println(" DETECTED!");
 
     for (int i = 0; i < 5; i++) {
         digitalWrite(LED_PIN, HIGH);
         tone(BUZZER_PIN, 1000, 200);
         delay(200);
         digitalWrite(LED_PIN, LOW);
         noTone(BUZZER_PIN);
         delay(200);
     }
 
     float lat, lon;
     if (gps.getLocation(lat, lon)) {
         String message = "EMERGENCY: " + String(alertType) + " at " + String(lat, 6) + ", " + String(lon, 6);
         gsm.sendSMS("+1234567890", message.c_str());  // Replace with emergency number
     }
 }