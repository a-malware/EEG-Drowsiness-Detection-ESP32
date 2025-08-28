/*
 * Unit Test for KalmanEEG Filter
 * 
 * Tests the Kalman filter implementation for noise reduction.
 */

 #include <Arduino.h>
 #include "KalmanEEG.h"
 
 KalmanEEG kalmanFilter;
 
 void setup() {
     Serial.begin(115200);
     Serial.println("Kalman Filter Test Starting...");
 
     kalmanFilter.setParameters(0.1, 0.1);  // Example parameters
 }
 
 void loop() {
     float rawEEG[3] = {10.0, 12.0 + random(-100, 100) / 10.0, 11.0};  // Noisy data
     float filteredEEG[3];
 
     kalmanFilter.filterEEG(rawEEG, filteredEEG);
 
     Serial.print("Raw: ");
     Serial.print(rawEEG[1]);
     Serial.print(" Filtered: ");
     Serial.println(filteredEEG[1]);
 
     if (abs(filteredEEG[1] - 12.0) < 1.0) {  // Expect filtered value close to 12.0
         Serial.println("Test Passed: Noise reduced effectively.");
     } else {
         Serial.println("Test Failed: Excessive noise remains.");
     }
 
     delay(1000);
 }