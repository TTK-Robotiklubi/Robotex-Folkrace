#ifndef DEBUG_H
#define DEBUG_H

// Send the distance sensor data to the Serial Monitor for debugging
void sendToSerialPlotter(const int distances[]) {
  Serial.print("1. Left: ");
  Serial.print(distances[0]);
  Serial.print(", Middle: ");
  Serial.print(distances[1]);
  Serial.print(", Right: ");
  Serial.println(distances[2]);
}

// Send the color data (detected color and raw red/green values) to the Serial Monitor
void sendColorDataToSerial(String detectedColor, uint16_t red, uint16_t green) {
  Serial.print("Red: ");
  Serial.print(red);
  Serial.print(", Green: ");
  Serial.print(green);
  Serial.print(", Detected Color: ");
  Serial.println(detectedColor);
}

#endif
