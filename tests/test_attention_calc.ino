/*
 * Unit Test for AttentionCalculator
 * 
 * Tests the attention score calculation based on EEG data.
 */

 #include <Arduino.h>
 #include "AttentionCalculator.h"
 
 AttentionCalculator attentionCalc;
 const int SAMPLES = 128;
 float eegBuffer[SAMPLES];
 
 void setup() {
     Serial.begin(115200);
     Serial.println("Attention Calculator Test Starting...");
 
     // Initialize buffer with sample data
     for (int i = 0; i < SAMPLES; i++) {
         eegBuffer[i] = (i % 2 == 0) ? 10.0 : 5.0;  // Simulate alternating Theta/Beta patterns
     }
     attentionCalc.calibrate(NULL);  // Set baseline
 }
 
 void loop() {
     float attention = attentionCalc.calculateAttention(eegBuffer, SAMPLES);
     Serial.printf("Attention Score: %.2f%%\n", attention);
 
     if (attention > 50.0) {  // Expect reasonable attention score
         Serial.println("Test Passed: Valid attention score.");
     } else {
         Serial.println("Test Failed: Unexpectedly low attention.");
     }
 
     delay(1000);
 }