#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEClient.h>

// Replace with the address you found from the first scan
#define DEVICE_ADDRESS "11:22:33:44:55:66"

void setup() {
    Serial.begin(115200);
    Serial.println("Starting Arduino BLE Client application...");
    BLEDevice::init("");

    BLEClient* pClient = BLEDevice::createClient();
    BLEAddress bleAddress(DEVICE_ADDRESS);

    // Connect to the remote device
    if(pClient->connect(bleAddress)) {
        Serial.println("Connected to device!");
        
        // Get all services
        std::map<std::string, BLERemoteService*>* services = pClient->getServices();
        for(auto const& service : *services) {
            Serial.print("Service UUID: ");
            Serial.println(service.first.c_str());
            
            // Get characteristics of service
            std::map<std::string, BLERemoteCharacteristic*>* characteristics = service.second->getCharacteristics();
            for(auto const& characteristic : *characteristics) {
                Serial.print("  Characteristic UUID: ");
                Serial.println(characteristic.first.c_str());
                Serial.print("  Properties: ");
                BLERemoteCharacteristic* pChar = characteristic.second;
                if(pChar->canRead()) Serial.print("READ ");
                if(pChar->canWrite()) Serial.print("WRITE ");
                if(pChar->canNotify()) Serial.print("NOTIFY ");
                if(pChar->canIndicate()) Serial.print("INDICATE ");
                Serial.println();
            }
            Serial.println();
        }
        
        pClient->disconnect();
    }
}

void loop() {
    delay(1000);
}