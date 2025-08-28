// KalmanEEG.h
class KalmanEEG {
    private:
        // Single state Kalman filter for each EEG channel
        float x[3];        // State estimates for 3 channels
        float p[3];        // Error covariances
        float q;           // Process noise
        float r;           // Measurement noise
        float k[3];        // Kalman gains
    
    public:
        KalmanEEG() {
            // Initialize with reasonable defaults
            q = 0.1;  // Process noise
            r = 0.1;  // Measurement noise
            
            // Initialize states and covariances
            for(int i = 0; i < 3; i++) {
                x[i] = 0.0f;
                p[i] = 1.0f;
                k[i] = 0.0f;
            }
        }
        
        // Update single channel
        float update(float measurement, int channel) {
            // Predict
            // State stays same (x = x)
            p[channel] = p[channel] + q;
            
            // Update
            k[channel] = p[channel] / (p[channel] + r);
            x[channel] = x[channel] + k[channel] * (measurement - x[channel]);
            p[channel] = (1 - k[channel]) * p[channel];
            
            return x[channel];
        }
        
        // Process all channels
        void filterEEG(float* input, float* output) {
            for(int i = 0; i < 3; i++) {
                output[i] = update(input[i], i);
            }
        }
        
        // Adjust filter parameters
        void setParameters(float process_noise, float measurement_noise) {
            q = process_noise;
            r = measurement_noise;
        }
    };