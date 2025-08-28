#include <arduinoFFT.h>
#include <Preferences.h>  // ESP32's built-in storage library

// Constants for FFT and attention calculation
#define SAMPLES 128
#define SAMPLING_FREQ 512
#define CALIBRATION_TIME 30
#define NUM_BANDS 4
#define DEBUG_OUTPUT 1  // Set to 0 to disable debug printing

// Frequency bands
struct FrequencyBand {
    const char* name;
    float minFreq;
    float maxFreq;
};

const FrequencyBand BANDS[NUM_BANDS] = {
    {"Delta", 0.5, 4},
    {"Theta", 4, 8},
    {"Alpha", 8, 13},
    {"Beta", 13, 30}
};

class AttentionCalculator {
private:
    arduinoFFT FFT;
    double vReal[SAMPLES];
    double vImag[SAMPLES];
    float attentionHistory[10];
    int historyIndex = 0;
    float calibratedThreshold;
    Preferences preferences;  // For storing calibration data
    float bandPowers[NUM_BANDS];  // Store power of each frequency band
    
    // Enhanced band power calculation with normalization
    float getBandPower(float minFreq, float maxFreq) {
        int minBin = (int)(minFreq * SAMPLES / SAMPLING_FREQ);
        int maxBin = (int)(maxFreq * SAMPLES / SAMPLING_FREQ);
        float power = 0;
        
        for(int i = minBin; i <= maxBin && i < SAMPLES/2; i++) {
            power += sqrt(vReal[i] * vReal[i] + vImag[i] * vImag[i]);
        }
        
        // Normalize by number of bins
        return power / (maxBin - minBin + 1);
    }
    
    float movingAverage(float newValue) {
        attentionHistory[historyIndex] = newValue;
        historyIndex = (historyIndex + 1) % 10;
        
        float sum = 0;
        for(int i = 0; i < 10; i++) {
            sum += attentionHistory[i];
        }
        return sum / 10;
    }
    
    // Enhanced attention calculation using multiple band ratios
    float calculateAttentionFromBands() {
        // Beta/(Theta + Alpha) ratio - higher means more attentive
        float attentionRatio1 = bandPowers[3] / (bandPowers[1] + bandPowers[2] + 1e-6);
        
        // Delta suppression indicates wakefulness
        float attentionRatio2 = 1.0 - (bandPowers[0] / (bandPowers[1] + bandPowers[2] + bandPowers[3] + 1e-6));
        
        // Combine ratios with weights
        float attention = (0.6 * attentionRatio1 + 0.4 * attentionRatio2) * 100.0;
        return constrain(attention, 0, 100);
    }

public:
    AttentionCalculator() : FFT(vReal, vImag, SAMPLES, SAMPLING_FREQ) {
        for(int i = 0; i < SAMPLES; i++) {
            vReal[i] = vImag[i] = 0;
        }
        for(int i = 0; i < 10; i++) {
            attentionHistory[i] = 0;
        }
        loadCalibration();
    }
    
    void loadCalibration() {
        preferences.begin("attention", false);  // false = read/write mode
        calibratedThreshold = preferences.getFloat("threshold", 40.0);
        preferences.end();
    }
    
    void saveCalibration() {
        preferences.begin("attention", false);
        preferences.putFloat("threshold", calibratedThreshold);
        preferences.end();
    }
    
    float calculateAttention(float* eegData, int length) {
        // Copy data to FFT arrays
        for(int i = 0; i < length && i < SAMPLES; i++) {
            vReal[i] = eegData[i];
            vImag[i] = 0;
        }
        
        // Apply FFT
        FFT.Windowing(FFT_WIN_TYP_HAMMING, FFT_FORWARD);
        FFT.Compute(FFT_FORWARD);
        FFT.ComplexToMagnitude();
        
        // Calculate power in each band
        for(int i = 0; i < NUM_BANDS; i++) {
            bandPowers[i] = getBandPower(BANDS[i].minFreq, BANDS[i].maxFreq);
        }
        
        if(DEBUG_OUTPUT) {
            printBandPowers();
        }
        
        float attention = calculateAttentionFromBands();
        return movingAverage(attention);
    }
    
    void calibrate(void (*progressCallback)(int) = nullptr) {
        float sumAttention = 0;
        int samples = 0;
        long startTime = millis();
        
        Serial.println("Starting calibration...");
        Serial.println("Please maintain normal attention for 30 seconds");
        
        // Collection phase
        while((millis() - startTime) < (CALIBRATION_TIME * 1000)) {
            float eegData[SAMPLES];  // Replace with actual data acquisition
            float attention = calculateAttention(eegData, SAMPLES);
            
            sumAttention += attention;
            samples++;
            
            int progress = ((millis() - startTime) * 100) / (CALIBRATION_TIME * 1000);
            if(progressCallback) {
                progressCallback(progress);
            }
            
            if(DEBUG_OUTPUT && (samples % 10 == 0)) {
                Serial.printf("Current attention: %.2f\n", attention);
            }
            
            delay(100);
        }
        
        float avgAttention = sumAttention / samples;
        calibratedThreshold = avgAttention * 0.7;
        
        // Save calibration to NVS
        saveCalibration();
        
        Serial.printf("Calibration complete! Threshold: %.2f\n", calibratedThreshold);
    }
    
    void printBandPowers() {
        Serial.println("Band Powers:");
        for(int i = 0; i < NUM_BANDS; i++) {
            Serial.printf("%s: %.2f  ", BANDS[i].name, bandPowers[i]);
        }
        Serial.println();
    }
    
    float getThreshold() { return calibratedThreshold; }
    void setThreshold(float threshold) {
        calibratedThreshold = threshold;
        saveCalibration();
    }
    
    // Get relative band powers for visualization
    void getBandPowers(float* powers) {
        float total = 0;
        for(int i = 0; i < NUM_BANDS; i++) {
            total += bandPowers[i];
        }
        for(int i = 0; i < NUM_BANDS; i++) {
            powers[i] = (total > 0) ? (bandPowers[i] / total * 100) : 0;
        }
    }
};