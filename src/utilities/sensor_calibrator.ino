/*
 * Sensor Calibrator for EEG Drowsiness and Safety System
 * 
 * This utility calibrates the MPU6050 (acceleration/gyro thresholds) and MQ2 (ppm threshold)
 * for the ESP32-based safety system.
 */

 #include <Wire.h>
 #include <MPU6050.h>
 #include <MQ2.h>
 
 MPU6050 mpu;
 const int MQ2_PIN = A0;
 
 void setup() {
     Serial.begin(115200);
     Serial.println("Sensor Calibration Starting...");
 
     // Initialize I2C for MPU6050
     Wire.begin();
     mpu.initialize();
     if (!mpu.testConnection()) {
         Serial.println("MPU6050 connection failed!");
         while (1);
     }
 
     Serial.println("Calibration ready. Follow instructions:");
 }
 
 void calibrateMPU6050() {
     Serial.println("Calibrating MPU6050... Keep the device stationary for 10 seconds.");
     delay(10000);  // Allow stabilization
 
     long axTotal = 0, ayTotal = 0, azTotal = 0;
     long gxTotal = 0, gyTotal = 0, gzTotal = 0;
     const int samples = 100;
 
     for (int i = 0; i < samples; i++) {
         int16_t ax, ay, az, gx, gy, gz;
         mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
         axTotal += ax; ayTotal += ay; azTotal += az;
         gxTotal += gx; gyTotal += gy; gzTotal += gz;
         delay(50);
     }
 
     float accelX = axTotal / samples / 16384.0;
     float accelY = ayTotal / samples / 16384.0;
     float accelZ = (azTotal / samples / 16384.0) - 1.0;  // Subtract 1g
     float gyroX = gxTotal / samples / 131.0;
     float gyroY = gyTotal / samples / 131.0;
     float gyroZ = gzTotal / samples / 131.0;
 
     float baselineAccel = sqrt(accelX * accelX + accelY * accelY + accelZ * accelZ);
     float baselineGyro = sqrt(gyroX * gyroX + gyroY * gyroY + gyroZ * gyroZ);
 
     Serial.print("Baseline Acceleration (g): "); Serial.println(baselineAccel);
     Serial.print("Baseline Gyro (deg/s): "); Serial.println(baselineGyro);
     Serial.println("Set ACCEL_THRESHOLD to " + String(baselineAccel + 0.5) + "g and GYRO_THRESHOLD to " + String(baselineGyro + 50) + "deg/s as a starting point.");
 }
 
 void calibrateMQ2() {
     Serial.println("Calibrating MQ2... Expose to clean air for 10 seconds.");
     delay(10000);  // Allow sensor stabilization
 
     long mq2Total = 0;
     const int samples = 100;
 
     for (int i = 0; i < samples; i++) {
         mq2Total += analogRead(MQ2_PIN);
         delay(50);
     }
 
     float baselinePPM = (mq2Total / samples) * (5000.0 / 1023.0);
     Serial.print("Baseline PPM (clean air): "); Serial.println(baselinePPM);
     Serial.println("Set MQ2 threshold to " + String(baselinePPM + 50) + "ppm as a starting point for smoke/fire detection.");
 }
 
 void loop() {
     if (Serial.available() > 0) {
         String command = Serial.readStringUntil('\n');
         command.trim();
 
         if (command == "calibrate_mpu") {
             calibrateMPU6050();
         } else if (command == "calibrate_mq2") {
             calibrateMQ2();
         } else if (command == "help") {
             Serial.println("Commands: calibrate_mpu, calibrate_mq2, help");
         }
     }
     delay(100);
 }