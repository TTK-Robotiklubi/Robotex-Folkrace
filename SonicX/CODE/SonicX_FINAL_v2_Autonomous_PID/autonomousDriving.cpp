#include "autonomousDriving.h" //main headder file for this
#include "globals.h" //for seeing left speed, rightspeed, move motors
#include "distance_sensors.h" //for seeing distance values "extern int distances[3];""
#include <Arduino.h> //for serial printing and more? Arduino functionality
unsigned long reverseStartTime = 0; // For tracking the reverse time
float turning_amplifier = 1.0; // Default turning amplifier

// PID variables
float previous_error = 0.0; // Stores the previous error for derivative calculation
float integral = 0.0; // Stores the cumulative error for integral calculation

void autonomous_mode() {
  // Get the current distances from the left, middle, and right sensors
  float left_distance = distances[0];  // Left sensor (sensor 0)
  float middle_distance = distances[1]; // Middle sensor (sensor 1)
  float right_distance = distances[2]; // Right sensor (sensor 2)

  // Check if the middle sensor detects an obstacle closer than 9 cm
  if (middle_distance < 9 && reverseStartTime == 0) {
    // If obstacle detected in front, reverse for 1 second
    leftSpeed = -speed_limit * 0.6;
    rightSpeed = -speed_limit * 0.6;
    reverseStartTime = millis(); // Start the reverse timer
    turning_amplifier = 2.0; // Increase turning amplifier when an obstacle is close
  }

  // If the robot has been reversing for 1 second, stop and move forward
  if (reverseStartTime != 0 && millis() - reverseStartTime >= 1000) {
    // Stop reversing and start moving forward again
    leftSpeed = speed_limit * 0.6;
    rightSpeed = speed_limit * 0.6;
    reverseStartTime = 0; // Reset the reverse timer
    turning_amplifier = 1.0; // Reset turning amplifier to default value
  }

  // Calculate the error based on the sensor readings
  float error = right_distance - left_distance;

  // PID control calculations with adjusted turning amplifier
  float pid_output = kp * error + ki * integral + kd * (error - previous_error);
  integral += error; // Accumulate the error for integral term
  previous_error = error; // Store current error for next cycle

  // Multiply the PID output by the turning amplifier for more aggressive turning
  pid_output *= turning_amplifier;

  // Adjust motor speeds based on PID output and turning amplifier
  leftSpeed = constrain(speed_limit * 0.6 + pid_output, -speed_limit, speed_limit);
  rightSpeed = constrain(speed_limit * 0.6 - pid_output, -speed_limit, speed_limit);

  moveMotors();  // Move the robot with the calculated speeds

  // Debugging: Indicate autonomous mode and print error and PID output on the same line
  Serial.print("Automatic mode\t");
  Serial.print("Error: ");
  Serial.print(error);
  Serial.print("\tPID Output: ");
  Serial.println(pid_output);
}