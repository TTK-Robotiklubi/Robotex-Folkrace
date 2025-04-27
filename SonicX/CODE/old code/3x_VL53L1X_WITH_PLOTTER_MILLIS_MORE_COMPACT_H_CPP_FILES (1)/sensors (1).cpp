#include "sensors.h"

// Define sensor objects
VL53L1X sensorLeft, sensorMiddle, sensorRight;

// Calibration factors
const float scalingFactors[] = {1.0, 1.0, 1.4};
const int offsets[] = {0, 0, 0};
int distances[3] = {0, 0, 0}; // Array for storing sensor readings

//added manually to change it in loop afterwards
int maxDistance = 70;  // Initialize to 70


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
    sensor.startContinuous(50); // 50ms interval
}

void readSensors(int distances[]) {
    distances[0] = getCalibratedDistance(sensorLeft, scalingFactors[0], offsets[0]);
    distances[1] = getCalibratedDistance(sensorMiddle, scalingFactors[1], offsets[1]);
    distances[2] = getCalibratedDistance(sensorRight, scalingFactors[2], offsets[2]);
    handleInvalidReadings(distances);
}

int getCalibratedDistance(VL53L1X &sensor, float scalingFactor, int offset) {
    uint16_t rawDistance = sensor.read(false); // Raw distance in mm
    if (rawDistance > 0) {
        int calibratedDistance = (rawDistance + offset) * scalingFactor / 10; // mm to cm
        return (calibratedDistance > maxDistance) ? maxDistance : calibratedDistance;
    }
    return 0; // Invalid reading
}

void handleInvalidReadings(int distances[]) {
    for (int i = 0; i < 3; i++) {
        if (distances[i] == 0) {
            distances[i] = maxDistance; // Assign fallback value
        }
    }
}
