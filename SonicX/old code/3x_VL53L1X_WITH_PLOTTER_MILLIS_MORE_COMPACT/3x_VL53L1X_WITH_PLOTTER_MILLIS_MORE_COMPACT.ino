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
#define MAX_DISTANCE 70          // Maximum allowable distance in centimeters
#define DISTANCE_MODE VL53L1X::Short // Short distance mode (up to ~1.3m)
#define TIMING_BUDGET 50000      // Measurement timing budget (microseconds) (50ms)
#define LOOP_INTERVAL 50        // Interval between loop iterations (milliseconds) (200ms- 5measurements in 1 sec) (same value as in "sensor.startContinuous(50); // 50ms interval)

// Sensor objects
VL53L1X sensorLeft, sensorMiddle, sensorRight;

// Calibration factors (adjust based on testing)
const float scalingFactors[] = {1.0, 1.0, 1.4};
const int offsets[] = {0, 0, 0}; // Calibration offsets in mm

// Timing variables
unsigned long previousMillis = 0;  // Store the last time the loop ran
unsigned long previousMillisSensor = 0; // Store the last time a sensor reading was taken
unsigned long sensorInterval = 50;  // Interval for sensor readings (ms)

void setup() {
  Serial.begin(115200);
  while (!Serial); // Wait for Serial Monitor to connect

  Wire.begin();

  // Initialize sensors
  initSensor(sensorLeft, XSHUT_LEFT, ADDRESS_LEFT);
  initSensor(sensorMiddle, XSHUT_MIDDLE, ADDRESS_MIDDLE);
  initSensor(sensorRight, XSHUT_RIGHT, ADDRESS_RIGHT);

  Serial.println("All sensors initialized!");
}

void loop() {
  unsigned long currentMillis = millis();

  // Check if it's time to update the sensor readings
  if (currentMillis - previousMillisSensor >= sensorInterval) {
    previousMillisSensor = currentMillis; // Update the last time sensor readings were taken

    // Read and calibrate distances
    int distances[] = {
      getCalibratedDistance(sensorLeft, scalingFactors[0], offsets[0]),
      getCalibratedDistance(sensorMiddle, scalingFactors[1], offsets[1]),
      getCalibratedDistance(sensorRight, scalingFactors[2], offsets[2])
    };

    // Check invalid readings and handle fallback
    handleInvalidReadings(distances);

    // Output data for Serial Plotter
    sendToSerialPlotter(distances);
  }

  // Perform any other tasks that should run continuously
  if (currentMillis - previousMillis >= LOOP_INTERVAL) {
    previousMillis = currentMillis; // Update the last time the loop ran
    // Additional tasks can be added here if needed
  }
}

// Initializes a VL53L1X sensor
void initSensor(VL53L1X &sensor, int xshutPin, uint8_t address) {
  pinMode(xshutPin, OUTPUT);
  digitalWrite(xshutPin, LOW);  // Reset
  delay(10);                     // Small delay for reset to take effect
  digitalWrite(xshutPin, HIGH); // Enable
  delay(10);                     // Small delay for sensor to power up

  if (!sensor.init()) {
    Serial.println("Failed to initialize VL53L1X sensor!");
    while (1);
  }

  sensor.setAddress(address);
  sensor.setDistanceMode(DISTANCE_MODE);
  sensor.setMeasurementTimingBudget(TIMING_BUDGET);
  sensor.startContinuous(50); // 50ms interval
}

// Reads and calibrates distance from a sensor
int getCalibratedDistance(VL53L1X &sensor, float scalingFactor, int offset) {
  uint16_t rawDistance = sensor.read(false); // Raw distance in mm
  if (rawDistance > 0) {
    int calibratedDistance = (rawDistance + offset) * scalingFactor / 10; // mm to cm
    return (calibratedDistance > MAX_DISTANCE) ? MAX_DISTANCE : calibratedDistance;
  }
  return 0; // Return 0 if the reading is invalid
}

// Handles invalid readings and applies fallback
void handleInvalidReadings(int distances[]) {
  const char *sensorNames[] = {"Left", "Middle", "Right"};
  for (int i = 0; i < 3; i++) {
    if (distances[i] == 0) {
      //Serial.print(sensorNames[i]); //commented out, so no noise on plotter
      //Serial.println(" sensor reading is invalid (out of range). Using fallback value.");
      distances[i] = MAX_DISTANCE; // Assign fallback value
    }
  }
}

// Sends data to Serial Plotter
void sendToSerialPlotter(const int distances[]) {
  Serial.print("Left:");
  Serial.print(distances[0]);
  Serial.print(",    Middle:");
  Serial.print(distances[1]);
  Serial.print(",    Right:");
  Serial.println(distances[2]);
}
