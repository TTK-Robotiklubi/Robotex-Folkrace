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

// Create sensor objects
VL53L1X sensorLeft;
VL53L1X sensorMiddle;
VL53L1X sensorRight;

// Calibration factors (adjust based on testing)
float scalingFactorLeft = 1;    // Scaling factor for the left sensor
float scalingFactorMiddle = 1;  // Scaling factor for the middle sensor
float scalingFactorRight = 1.4;   // Scaling factor for the right sensor

int offsetLeft = 0;    // Offset for the left sensor (in mm)
int offsetMiddle = 0;  // Offset for the middle sensor (in mm)
int offsetRight = 0;  // Offset for the right sensor (in mm)

//Limit is set 70 because it is what the senors see, when robot is in the middle of the track (actually limit is view cm less), so it doesnt wobble around too much and when it sees too much, then it just gives output of 70cm
#define MAX_DISTANCE 70  // Maximum allowable distance in centimeters  

void setup() { // Serial Begin
  Serial.begin(115200);
  while (!Serial);

  // Initialize I2C
  Wire.begin();

  // Initialize sensors
  initSensor(sensorLeft, XSHUT_LEFT, ADDRESS_LEFT);
  initSensor(sensorMiddle, XSHUT_MIDDLE, ADDRESS_MIDDLE);
  initSensor(sensorRight, XSHUT_RIGHT, ADDRESS_RIGHT);

  Serial.println("All sensors initialized!");
}

void loop() {
  // Read distances from each sensor
  int distance1 = calibrateDistance(sensorLeft, scalingFactorLeft, offsetLeft);
  int distance2 = calibrateDistance(sensorMiddle, scalingFactorMiddle, offsetMiddle);
  int distance3 = calibrateDistance(sensorRight, scalingFactorRight, offsetRight);

  // Apply the 70 cm limit (updated to ensure data does not exceed 70 cm)
  distance1 = (distance1 > MAX_DISTANCE) ? MAX_DISTANCE : distance1;
  distance2 = (distance2 > MAX_DISTANCE) ? MAX_DISTANCE : distance2;
  distance3 = (distance3 > MAX_DISTANCE) ? MAX_DISTANCE : distance3;

  // Check and handle invalid readings
  //This problem occurs only when the measured distance is set to "Short" and the real distance is over 1.3m
  if (distance1 == 0) {
    Serial.println("Left sensor reading is 0 (out of range or invalid).");
    distance1 = MAX_DISTANCE;   
  }

  if (distance2 == 0) {
    Serial.println("Middle sensor reading is 0 (out of range or invalid).");
    distance2 = MAX_DISTANCE;  
  }

  if (distance3 == 0) {
    Serial.println("Right sensor reading is 0 (out of range or invalid).");
    distance3 = MAX_DISTANCE;
  }

  // **Serial Plotter format** - send data for each sensor in one line
  Serial.print("Left:");
  Serial.print(distance1);  // Left sensor data
  Serial.print(",    Middle:"); 
  Serial.print(distance2);  // Middle sensor data
  Serial.print(",    Right:"); 
  Serial.println(distance3); // Right sensor data
  // This data is now ready to be plotted in the Serial Plotter.

  delay(200); // Adjust delay as needed for data stability
}

// Initialize a VL53L1X sensor
void initSensor(VL53L1X &sensor, int xshutPin, uint8_t address) {
  // Reset the sensor using its XSHUT pin
  pinMode(xshutPin, OUTPUT);
  digitalWrite(xshutPin, LOW);  // Hold XSHUT low to reset
  delay(10);                   // Wait for reset
  digitalWrite(xshutPin, HIGH); // Pull XSHUT high to enable
  delay(10);                   // Wait for sensor to power up

  // Initialize the sensor
  if (!sensor.init()) {
    Serial.println("Failed to initialize a VL53L1X sensor!");
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

// Calibrate the distance from a sensor
int calibrateDistance(VL53L1X &sensor, float scalingFactor, int offset) {
  uint16_t rawDistance = sensor.read(false); // false = don't clear interrupt flag
  if (rawDistance > 0) {
    // Convert to cm (scaling + offset + conversion to cm)
    return (rawDistance + offset) * scalingFactor / 10; // Divided by 10 to convert mm to cm
  }
  return 0; // Return 0 if the reading is invalid
}
