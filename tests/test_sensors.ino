/*
 * Unit Test for Sensors (MPU6050, MQ2, GPS, GSM)
 * 
 * Tests the functionality of all integrated sensors and GSM.
 */

 #include <Arduino.h>
 #include <Wire.h>
 #include <MPU6050.h>
 #include <MQ2.h>
 #include <SoftwareSerial.h>
 #include "GPSHandler.h"
 #include "GSMHandler.h"
 
 MPU6050 mpu;
 const int MQ2_PIN = A0;
 HardwareSerial Serial2(2);  // GPS on Serial2
 GPSHandler gps(Serial2);
 SoftwareSerial gsmSerial(18, 19);  // GSM on pins 18/19
 GSMHandler gsm(gsmSerial);
 
 void setup() {
     Serial.begin(115200);
     Serial.println("Sensor Test Starting...");
 
     Wire.begin();
     mpu.initialize();
     gps.begin();
     gsm.begin();
 
     if (!mpu.testConnection()) Serial.println("MPU6050 Failed!");
 }
 
 void loop() {
     // Test MPU6050
     int16_t ax, ay, az, gx, gy, gz;
     mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
     float accel = sqrt((ax / 16384.0) * (ax / 16384.0) + (ay / 16384.0) * (ay / 16384.0) + (az / 16384.0) * (az / 16384.0));
     Serial.print("Accel: "); Serial.println(accel);
     if (accel > 0) Serial.println("MPU6050 Test Passed.");
 
     // Test MQ2
     int mq2Value = analogRead(MQ2_PIN);
     Serial.print("MQ2 Value: "); Serial.println(mq2Value);
     if (mq2Value > 0) Serial.println("MQ2 Test Passed.");
 
     // Test GPS
     float lat, lon;
     if (gps.getLocation(lat, lon)) {
         Serial.print("GPS: "); Serial.print(lat, 6); Serial.print(", "); Serial.println(lon, 6);
         Serial.println("GPS Test Passed.");
     } else {
         Serial.println("GPS Test Failed: No fix.");
     }
 
     // Test GSM
     if (gsm.sendSMS("+1234567890", "Test SMS")) {  // Replace with emergency number
         Serial.println("GSM Test Passed.");
     } else {
         Serial.println("GSM Test Failed.");
     }
 
     delay(5000);
 }