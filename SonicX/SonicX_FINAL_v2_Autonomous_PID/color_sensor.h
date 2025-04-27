#ifndef COLOR_SENSOR_H
#define COLOR_SENSOR_H

#include <Wire.h>
#include "globals.h" //for "Color_threshold_value" via globals, phone

// BH1745NUC I2C address
#define BH1745_ADDRESS 0x38

// Register addresses
#define MODE_CONTROL1    0x41
#define MODE_CONTROL2    0x42
#define MODE_CONTROL3    0x44
#define RED_DATA_LSB     0x50
#define GREEN_DATA_LSB   0x52

// Function declarations
void initializeColorSensor();
String detectColor();
uint16_t readColor(uint8_t reg);
void write8(uint8_t reg, uint8_t value);
void sendColorDataToSerial(String detectedColor, uint16_t red, uint16_t green);

// Declare global variables
extern bool isRed;
extern bool isGreen;
//extern int Color_threshold_value; //now from globals.h

#endif
