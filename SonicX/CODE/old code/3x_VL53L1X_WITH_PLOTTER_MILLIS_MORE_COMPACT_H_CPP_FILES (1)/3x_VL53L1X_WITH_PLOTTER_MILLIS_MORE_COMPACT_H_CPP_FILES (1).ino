#include <Wire.h>
#include "sensors.h"  // Include your distance sensor header file
#include "debug.h"    // Include the debug header for serial output
#include "color_sensor.h"  // Include the color sensor header file

void setup() {
  Serial.begin(115200);
  while (!Serial);  // Wait for Serial Monitor to connect

  Wire.begin();  // Initialize I2C

  // Initialize distance sensors (if applicable)
  initSensors();

  // Initialize the color sensor
  initializeColorSensor();
}

void loop() {
  // Read and process data from the distance sensors
  int distances[3];  // Assuming you have 3 sensors
  readSensors(distances);

  // Read color sensor data
  String detectedColor = detectColor();
  uint16_t red = readColor(RED_DATA_LSB);
  uint16_t green = readColor(GREEN_DATA_LSB);

  // Send the distance sensor data to the Serial Monitor for debugging
  sendToSerialPlotter(distances);

  // Send the color sensor data to the Serial Monitor for debugging
  sendColorDataToSerial(detectedColor, red, green);

  delay(50);  // Small delay for stability
}
