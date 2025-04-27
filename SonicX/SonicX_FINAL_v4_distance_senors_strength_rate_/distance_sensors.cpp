#include "distance_sensors.h"
#include <Arduino.h>
#include <VL53L1X.h>

// Define sensor objects
VL53L1X sensorLeft, sensorMiddle, sensorRight;

// Calibration factors
const float scalingFactors[] = {1.0, 1.0, 1.4};
const int offsets[] = {0, 0, 0};
int distances[3] = {0, 0, 0}; // Array for storing sensor readings

//added manually to change it in loop afterwards
int maxDistance = 100;  // Initialize to 70

// Set a minimum threshold for signal rate (basically signal stength) and maximum for ambient light interference
const float MIN_SIGNAL_RATE = 10.0;  // Example threshold in MCPS //THANKS TO THIS I WONT GET PHANTOM READINGS, IF I FILTER THE READINGS WITH THIS
//const float MAX_AMBIENT_LIGHT = 100.0; // Example threshold for ambient light in MCPS //have to test this? not in use rn



void initSensors() {
    Wire.begin();
    initSensor(sensorLeft, XSHUT_LEFT, ADDRESS_LEFT);
    initSensor(sensorMiddle, XSHUT_MIDDLE, ADDRESS_MIDDLE);
    initSensor(sensorRight, XSHUT_RIGHT, ADDRESS_RIGHT);
}

void initSensor(VL53L1X &sensor, int xshutPin, uint8_t address) {
    pinMode(xshutPin, OUTPUT);
    digitalWrite(xshutPin, LOW);  // Reset
    delay(10);
    digitalWrite(xshutPin, HIGH); // Enable
    delay(10);

    if (!sensor.init()) {
        Serial.println("Failed to initialize VL53L1X sensor!");
        while (1);
    }

    sensor.setAddress(address);
    sensor.setDistanceMode(DISTANCE_MODE);
    sensor.setMeasurementTimingBudget(TIMING_BUDGET);
    
    sensor.startContinuous(20); // 50ms interval if Long, 33ms for Medium and 20ms is minimum 
}

void setROI(VL53L1X &sensor, uint8_t width, uint8_t height) {
    // Address of the ROI configuration registers
    const uint16_t ROI_WIDTH_REGISTER = 0x98;
    const uint16_t ROI_HEIGHT_REGISTER = 0x99;

    // Use the sensor's `writeReg` method to set ROI
    sensor.writeReg(ROI_WIDTH_REGISTER, width);
    sensor.writeReg(ROI_HEIGHT_REGISTER, height);
}

void readSensors(int distances[]) {
    distances[0] = getCalibratedDistance(sensorLeft, scalingFactors[0], offsets[0]);
    distances[1] = getCalibratedDistance(sensorMiddle, scalingFactors[1], offsets[1]);
    distances[2] = getCalibratedDistance(sensorRight, scalingFactors[2], offsets[2]);
}

int getCalibratedDistance(VL53L1X &sensor, float scalingFactor, int offset) {
    uint16_t rawDistance = sensor.read(false); // Raw distance in mm
    float signalRate = sensor.ranging_data.peak_signal_count_rate_MCPS;
    float ambientLight = sensor.ranging_data.ambient_count_rate_MCPS;
   
      Serial.print("Raw distance: ");
    Serial.println(rawDistance);
    Serial.print("Signal rate: ");
    Serial.println(signalRate);
    Serial.print("ambientLight: ");
    Serial.println(ambientLight);
    // Check for weak signals or invalid readings
    if (signalRate < MIN_SIGNAL_RATE || rawDistance == 0) {
        return maxDistance; // Discard weak signals or invalid readings
    }      
    if (rawDistance > 0) {
        int calibratedDistance = (rawDistance + offset) * scalingFactor / 10; // mm to cm
        return (calibratedDistance > maxDistance) ? maxDistance : calibratedDistance;
    }
    return 0; // Invalid reading
}
