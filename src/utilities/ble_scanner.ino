#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

int scanTime = 5; //In seconds
BLEScan* pBLEScan;

// This class handles the scanning callback
class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice advertisedDevice) {
        Serial.println("Device found:");
        Serial.printf("Name: %s\n", advertisedDevice.getName().c_str());
        Serial.printf("Address: %s\n", advertisedDevice.getAddress().toString().c_str());
        Serial.printf("RSSI: %d\n", advertisedDevice.getRSSI());
        
        if(advertisedDevice.haveServiceUUID()) {
            Serial.print("Service UUID: ");
            Serial.println(advertisedDevice.getServiceUUID().toString().c_str());
        }
        
        Serial.println("------------------------");
    }
};

void setup() {
    Serial.begin(115200);
    Serial.println("BLE Scanner");

    BLEDevice::init("");
    pBLEScan = BLEDevice::getScan(); //create new scan
    pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
    pBLEScan->setActiveScan(true); //active scan uses more power, but gets results faster
    pBLEScan->setInterval(100);
    pBLEScan->setWindow(99);  // less or equal setInterval value
}

void loop() {
    // Put your main code here, to run repeatedly:
    Serial.println("Starting BLE scan...");
    BLEScan* pBLEScan = BLEDevice::getScan();
    BLEScanResults foundDevices = pBLEScan->start(scanTime, false);
    Serial.print("Devices found: ");
    Serial.println(foundDevices.getCount());
    Serial.println("Scan done!");
    delay(2000);
}