//A .h file, or header file, contains declarations—like a blueprint. These declarations inform the program what functions, variables, or classes exist, without actually defining how they work.
//Think of a .h file as a menu in a restaurant. It lists all the dishes (functions, variables) available but doesn’t tell you how to cook them (that's in the .cpp file).

#ifndef SENSORS_H
#define SENSORS_H

#include <Wire.h>
#include <VL53L1X.h>

// Define XSHUT pins for each sensor
#define XSHUT_LEFT 2
#define XSHUT_MIDDLE 0
#define XSHUT_RIGHT 4

// Unique I2C addresses for each sensor
#define ADDRESS_LEFT 0x30
#define ADDRESS_MIDDLE 0x31
#define ADDRESS_RIGHT 0x32

// Distance measurement settings
//#define MAX_DISTANCE 70          // Maximum allowable distance in cm
extern int maxDistance;  // Declare as extern for global use, panin selle, mitte define, et saaks pärast loopis ka muuta seda.


#define DISTANCE_MODE VL53L1X::Short // Short distance mode
#define TIMING_BUDGET 50000      // Measurement timing budget (microseconds)

// Sensor objects
extern VL53L1X sensorLeft, sensorMiddle, sensorRight; // Define these in sensors.cpp

// Calibration factors
extern const float scalingFactors[]; // Declare as extern
extern const int offsets[];

// Function declarations
void initSensors();
void initSensor(VL53L1X &sensor, int xshutPin, uint8_t address);
void readSensors(int distances[]);
int getCalibratedDistance(VL53L1X &sensor, float scalingFactor, int offset);
void handleInvalidReadings(int distances[]);

#endif
