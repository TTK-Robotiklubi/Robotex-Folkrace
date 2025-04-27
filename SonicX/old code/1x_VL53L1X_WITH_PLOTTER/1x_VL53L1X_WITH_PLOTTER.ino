#include <Wire.h>
#include <VL53L1X.h>

// Define XSHUT pin for the sensor
#define XSHUT_PIN 4

// Unique I2C address for the sensor
#define SENSOR_ADDRESS 0x30

// Create sensor object
VL53L1X sensor;

// Calibration factors (adjust based on testing)
float scalingFactor = 1;    // Scaling factor for the sensor
int offset = 0;             // Offset for the sensor (in mm)

#define MAX_DISTANCE 70  // Maximum allowable distance in centimeters

void setup() {
  // Serial Begin
  Serial.begin(115200);
  while (!Serial);

  // Initialize I2C
  Wire.begin();

  // Initialize sensor
  initSensor(sensor, XSHUT_PIN, SENSOR_ADDRESS);

  Serial.println("Sensor initialized!");
}

void loop() {
  // Read distance from the sensor
  int distance = calibrateDistance(sensor, scalingFactor, offset);

  // Apply the 70 cm limit
  distance = (distance > MAX_DISTANCE) ? MAX_DISTANCE : distance;

  // Output the data for plotting
  Serial.print("Distance:");
  Serial.println(distance);

  // Delay for stability
  delay(200); // Adjust delay as needed
}

// Initialize the VL53L1X sensor
void initSensor(VL53L1X &sensor, int xshutPin, uint8_t address) {
  // Reset the sensor using its XSHUT pin
  pinMode(xshutPin, OUTPUT);
  digitalWrite(xshutPin, LOW);  // Hold XSHUT low to reset
  delay(10);                    // Wait for reset
  digitalWrite(xshutPin, HIGH); // Pull XSHUT high to enable
  delay(10);                    // Wait for sensor to power up

  // Initialize the sensor
  if (!sensor.init()) {
    Serial.println("Failed to initialize VL53L1X sensor!");
    while (1);
  }

  // Assign a new I2C address
  sensor.setAddress(address);

  // Set distance mode and timing budget
  sensor.setDistanceMode(VL53L1X::Short);        // Short Mode: For short distances with higher accuracy (up to ~1.3m).
  sensor.setMeasurementTimingBudget(50000);     // 50ms timing budget

  // Start continuous ranging
  sensor.startContinuous(50); // 50ms interval
}

// Calibrate the distance from the sensor
int calibrateDistance(VL53L1X &sensor, float scalingFactor, int offset) {
  uint16_t rawDistance = sensor.read(false); // false = don't clear interrupt flag
  if (rawDistance > 0) {
    // Convert to cm (scaling + offset + conversion to cm)
    return (rawDistance + offset) * scalingFactor / 10; // Divided by 10 to convert mm to cm
  }
  return 0; // Return 0 if the reading is invalid
}
