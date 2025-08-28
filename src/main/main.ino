/*
 * EEG-based Drowsiness Detection and IoT-Enabled Vehicle Safety System for ESP32
 * 
 * This system connects to a MindLink EEG device via Bluetooth, processes EEG signals
 * using Kalman filtering and FFT analysis for drowsiness detection, and integrates
 * MPU6050 for crash detection, MQ2 for smoke/fire detection, and GSM with GPS for
 * emergency alerts with real-time location.
 * 
 * Author: a-malware
 * License: MIT
 */

 #include <BLEDevice.h>
 #include <BLEUtils.h>
 #include <BLEScan.h>
 #include <BLEAdvertisedDevice.h>
 #include <arduinoFFT.h>
 #include <Preferences.h>
 #include <Wire.h>
 #include <MPU6050.h>
 #include <MQ2.h>
 #include <SoftwareSerial.h>
 #include <TinyGPS++.h>
 
 #include "KalmanEEG.h"
 #include "AttentionCalculator.h"
 
 // Configuration
 #define MINDLINK_DEVICE_NAME "MindLink"  // Replace with your device name
 #define MINDLINK_SERVICE_UUID        "XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX"
 #define MINDLINK_CHARACTERISTIC_UUID "XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX"
 #define SCAN_TIME 5
 #define LED_PIN 2
 #define BUZZER_PIN 4
 #define BUTTON_PIN 0  // Boot button for calibration
 #define MQ2_PIN A0   // Analog pin for MQ2 sensor
 #define GPS_TX 16    // GPS module TX pin (Serial2)
 #define GPS_RX 17    // GPS module RX pin (Serial2)
 #define GSM_TX 18    // GSM module TX pin
 #define GSM_RX 19    // GSM module RX pin
 
 // MPU6050 and thresholds
 MPU6050 mpu;
 const float ACCEL_THRESHOLD = 1.5;  // g-force for crash detection
 const float GYRO_THRESHOLD = 200;   // degrees/s for crash detection
 
 // GPS and GSM configuration
 TinyGPSPlus gps;
 SoftwareSerial gpsSerial(GPS_RX, GPS_TX);
 SoftwareSerial gsmSerial(GSM_RX, GSM_TX);
 const char* EMERGENCY_NUMBER = "+1234567890";  // Replace with emergency number
 
 // Global objects
 BLEClient* pClient = NULL;
 BLERemoteCharacteristic* pRemoteCharacteristic = NULL;
 bool deviceConnected = false;
 bool calibrationMode = false;
 
 KalmanEEG kalmanFilter;
 AttentionCalculator attentionCalc;
 
 // Data buffer for EEG processing
 const int SAMPLES = 128;
 float eegBuffer[SAMPLES];
 int bufferIndex = 0;
 
 // Accident detection variables
 bool crashDetected = false;
 bool fireDetected = false;
 unsigned long lastAlertTime = 0;
 const unsigned long ALERT_COOLDOWN = 60000;  // 1-minute cooldown
 
 class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
     void onResult(BLEAdvertisedDevice advertisedDevice) {
         if (advertisedDevice.getName() == MINDLINK_DEVICE_NAME) {
             BLEDevice::getScan()->stop();
             connectToServer(advertisedDevice);
         }
     }
 };
 
 void notifyCallback(BLERemoteCharacteristic* pBLERemoteCharacteristic, 
                    uint8_t* pData, size_t length, bool isNotify) {
     // Parse EEG data from MindLink (adjust based on your device's format)
     float rawEEG[3];  // 3 electrodes
     
     // Example parsing - replace with actual MindLink data format
     for(int i = 0; i < 3 && i * 4 < length; i++) {
         memcpy(&rawEEG[i], &pData[i * 4], 4);
     }
     
     // Apply Kalman filtering
     float filteredEEG[3];
     kalmanFilter.filterEEG(rawEEG, filteredEEG);
     
     // Add to buffer for FFT processing
     eegBuffer[bufferIndex] = (filteredEEG[0] + filteredEEG[1] + filteredEEG[2]) / 3.0;
     bufferIndex = (bufferIndex + 1) % SAMPLES;
     
     // Process full buffer
     if(bufferIndex == 0) {
         float attention = attentionCalc.calculateAttention(eegBuffer, SAMPLES);
         
         Serial.printf("Attention Level: %.2f%%\n", attention);
         
         // Check for drowsiness
         if(!calibrationMode && attention < attentionCalc.getThreshold()) {
             triggerAlert("Drowsiness");
         }
     }
 }
 
 bool connectToServer(BLEAdvertisedDevice& advertisedDevice) {
     Serial.println("Connecting to MindLink device...");
     
     pClient = BLEDevice::createClient();
     
     if(!pClient->connect(&advertisedDevice)) {
         Serial.println("Failed to connect to device");
         return false;
     }
     
     BLERemoteService* pRemoteService = pClient->getService(MINDLINK_SERVICE_UUID);
     if (pRemoteService == nullptr) {
         Serial.println("Failed to find service UUID");
         pClient->disconnect();
         return false;
     }
 
     pRemoteCharacteristic = pRemoteService->getCharacteristic(MINDLINK_CHARACTERISTIC_UUID);
     if (pRemoteCharacteristic == nullptr) {
         Serial.println("Failed to find characteristic UUID");
         pClient->disconnect();
         return false;
     }
 
     if(pRemoteCharacteristic->canNotify()) {
         pRemoteCharacteristic->registerForNotify(notifyCallback);
         Serial.println("Successfully connected and registered for notifications");
         deviceConnected = true;
         return true;
     }
     
     Serial.println("Characteristic does not support notifications");
     pClient->disconnect();
     return false;
 }
 
 void triggerAlert(const char* alertType) {
     Serial.print(alertType);
     Serial.println(" DETECTED! ALERTING!");
     
     // Visual and audio alert
     for(int i = 0; i < 5; i++) {
         digitalWrite(LED_PIN, HIGH);
         tone(BUZZER_PIN, 1000, 200);
         delay(200);
         digitalWrite(LED_PIN, LOW);
         noTone(BUZZER_PIN);
         delay(200);
     }
     
     // Send GSM alert if not in cooldown
     unsigned long currentTime = millis();
     if (currentTime - lastAlertTime > ALERT_COOLDOWN) {
         sendGSMEmergencyAlert(alertType);
         lastAlertTime = currentTime;
     }
 }
 
 void sendGSMEmergencyAlert(const char* alertType) {
     // Update GPS location
     while (gpsSerial.available() > 0) {
         if (gps.encode(gpsSerial.read())) {
             if (gps.location.isValid()) {
                 String location = String(gps.location.lat(), 6) + ", " + String(gps.location.lng(), 6);
                 gsmSerial.begin(9600);
                 delay(1000);
                 
                 gsmSerial.println("AT");  // Check GSM module
                 delay(1000);
                 gsmSerial.println("AT+CMGF=1");  // Set text mode
                 delay(1000);
                 
                 String message = "EMERGENCY: " + String(alertType) + " detected at " + location;
                 gsmSerial.print("AT+CMGS=\"");
                 gsmSerial.print(EMERGENCY_NUMBER);
                 gsmSerial.println("\"");
                 delay(1000);
                 gsmSerial.println(message);
                 delay(1000);
                 gsmSerial.write(0x1A);  // Ctrl+Z to send
                 delay(1000);
                 
                 Serial.println("GSM Emergency Alert Sent with Location: " + location);
                 gsmSerial.end();
                 return;
             }
         }
     }
     // Fallback if no GPS fix
     Serial.println("No GPS fix available, alert not sent.");
 }
 
 void checkAccident() {
     // Read MPU6050 data
     int16_t ax, ay, az, gx, gy, gz;
     mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
     
     // Convert to g-force and degrees/s
     float accelX = ax / 16384.0;
     float accelY = ay / 16384.0;
     float accelZ = az / 16384.0;
     float gyroX = gx / 131.0;
     float gyroY = gy / 131.0;
     float gyroZ = gz / 131.0;
     
     // Calculate total acceleration magnitude
     float accelMagnitude = sqrt(accelX * accelX + accelY * accelY + accelZ * accelZ) - 1.0;  // Subtract 1g (gravity)
     float gyroMagnitude = sqrt(gyroX * gyroX + gyroY * gyroY + gyroZ * gyroZ);
     
     // Detect crash
     if (accelMagnitude > ACCEL_THRESHOLD || gyroMagnitude > GYRO_THRESHOLD) {
         if (!crashDetected) {
             crashDetected = true;
             triggerAlert("Crash");
         }
     } else {
         crashDetected = false;
     }
 }
 
 void checkFire() {
     int mq2Value = analogRead(MQ2_PIN);
     float ppm = mq2Value * (5000.0 / 1023.0);  // Convert to ppm (adjust based on sensor calibration)
     
     if (ppm > 200) {  // Threshold for smoke/fire detection (adjust as needed)
         if (!fireDetected) {
             fireDetected = true;
             triggerAlert("Fire/Smoke");
         }
     } else {
         fireDetected = false;
     }
 }
 
 void onCalibrationProgress(int progress) {
     Serial.printf("Calibration Progress: %d%%\n", progress);
     
     // Blink LED to show progress
     if(progress % 10 == 0) {
         digitalWrite(LED_PIN, HIGH);
         delay(100);
         digitalWrite(LED_PIN, LOW);
     }
 }
 
 void startCalibration() {
     Serial.println("Starting calibration process...");
     Serial.println("Please sit comfortably and stay alert during calibration");
     
     calibrationMode = true;
     attentionCalc.calibrate(onCalibrationProgress);
     calibrationMode = false;
     
     Serial.println("Calibration complete!");
 }
 
 void scanForDevice() {
     Serial.println("Scanning for MindLink device...");
     BLEScan* pBLEScan = BLEDevice::getScan();
     pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
     pBLEScan->setActiveScan(true);
     pBLEScan->start(SCAN_TIME);
 }
 
 void setup() {
     Serial.begin(115200);
     Serial.println("EEG Drowsiness and Safety System Starting...");
     
     // Initialize pins
     pinMode(LED_PIN, OUTPUT);
     pinMode(BUZZER_PIN, OUTPUT);
     pinMode(BUTTON_PIN, INPUT_PULLUP);
     
     // Initialize I2C for MPU6050
     Wire.begin();
     mpu.initialize();
     if (!mpu.testConnection()) {
         Serial.println("MPU6050 connection failed!");
     }
     
     // Initialize GPS
     gpsSerial.begin(9600);
     
     // Initialize BLE
     BLEDevice::init("ESP32_Safety_Monitor");
     
     // Initialize filters and calculators
     kalmanFilter.setParameters(0.1, 0.1);
     
     // Check if calibration button is pressed during startup
     if(digitalRead(BUTTON_PIN) == LOW) {
         delay(100); // Debounce
         if(digitalRead(BUTTON_PIN) == LOW) {
             startCalibration();
         }
     }
     
     // Start scanning for MindLink device
     scanForDevice();
     
     Serial.println("System ready!");
 }
 
 void loop() {
     // Check for calibration button press
     static unsigned long lastButtonCheck = 0;
     if(millis() - lastButtonCheck > 100) {
         if(digitalRead(BUTTON_PIN) == LOW) {
             delay(50); // Debounce
             if(digitalRead(BUTTON_PIN) == LOW) {
                 startCalibration();
                 delay(1000); // Prevent multiple triggers
             }
         }
         lastButtonCheck = millis();
     }
     
     // Check connection status
     if (!deviceConnected) {
         Serial.println("Device disconnected. Attempting to reconnect...");
         scanForDevice();
         delay(5000);
     }
     
     // Check for accidents and fire
     checkAccident();
     checkFire();
     
     // Main loop delay
     delay(100);
 }