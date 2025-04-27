#include "color_sensor.h"

// Initialize the BH1745NUC color sensor
void initializeColorSensor() {
  write8(MODE_CONTROL1, 0x02);  // Set measurement time (e.g., 160ms)
  write8(MODE_CONTROL2, 0x10);  // Set gain settings (e.g., 1x gain)
  write8(MODE_CONTROL3, 0x02);  // Enable measurement
}

// Define the global variables
bool isRed = false;
bool isGreen = false;
//int Color_threshold_value =0; //declared in main ino file via phone. 

// Detect the color based on the sensor readings
String detectColor() {
  uint16_t red = readColor(RED_DATA_LSB);
  uint16_t green = readColor(GREEN_DATA_LSB);
  uint16_t blue = readColor(BLUE_DATA_LSB);

  // Thresholds for detecting colors (customize for your needs)
  isRed = red > Color_threshold_value && green < Color_threshold_value;
  isGreen = green > Color_threshold_value && red < Color_threshold_value;

  if (isRed) {
    return "RED";
  } else if (isGreen) {
    return "GREEN";
  } else {
    return "NONE";
  }
}

// Read a 16-bit color value from the sensor
uint16_t readColor(uint8_t reg) {
  Wire.beginTransmission(BH1745_ADDRESS);
  Wire.write(reg);
  Wire.endTransmission(false);
  Wire.requestFrom(BH1745_ADDRESS, 2);
  uint8_t lsb = Wire.read();
  uint8_t msb = Wire.read();
  return (msb << 8) | lsb;
}

// Write a value to a register on the color sensor
void write8(uint8_t reg, uint8_t value) {
  Wire.beginTransmission(BH1745_ADDRESS);
  Wire.write(reg);
  Wire.write(value);
  Wire.endTransmission();
}

