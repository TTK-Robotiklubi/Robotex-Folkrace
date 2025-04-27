#include "autonomousDriving.h" //main header file for this
#include "globals.h" //for seeing left speed, right speed, move motors
#include "distance_sensors.h" //for seeing distance values "extern int distances[3];"
#include <Arduino.h> //for serial printing and more? Arduino functionality

unsigned long reverseStartTime = 0; // For tracking the reverse time
float turning_amplifier = 1.0; // Default turning amplifier

unsigned long obstacleStartTime = 0; // To track when obstacle is detected
bool obstacleDetected = false;      // To track if obstacle detection has started


// PID variables
float previous_error = 0.0; // Stores the previous error for derivative calculation
float integral = 0.0; // Stores the cumulative error for integral calculation
const float integral_limit = 10.0; // Adjust this limit as needed
float move_speed =0;

void autonomous_mode() {
  // Get the current distances from the left, middle, and right sensors
  float left_distance = distances[0];  // Left sensor (sensor 0)
  float middle_distance = distances[1]; // Middle sensor (sensor 1)
  float right_distance = distances[2]; // Right sensor (sensor 2)

  float base_speed = speed_limit * 0.7;  // Define the base speed as 70% of max speed 165*0.7= 115,5

   // Handle reversing logic
if (reverseStartTime != 0) {
  // Check if reversing time is complete
  if (millis() - reverseStartTime >= 1000) {
    move_speed = base_speed; // Resume normal speed
    reverseStartTime = 0;    // Reset the reverse timer
    turning_amplifier = 1.0; // Reset turning amplifier
    Serial.println("5. Reversing complete. Resuming normal operation.");
  } else {
    move_speed = -base_speed; // Continue reversing
    turning_amplifier = 5.0;  // Safety setting while reversing

    // Calculate reverse turn bias based on sensor differences
    float reverse_turn_bias = (distances[2] - distances[0]) * turning_amplifier; // Scale factor for sensitivity
    leftSpeed = constrain(move_speed + reverse_turn_bias, -speed_limit, speed_limit);
    rightSpeed = constrain(move_speed - reverse_turn_bias, -speed_limit, speed_limit);

    moveMotors(); // Apply motor commands
    Serial.print("4. Reversing with turning! Left Speed: ");
    Serial.print(leftSpeed);
    Serial.print("\tRight Speed: ");
    Serial.println(rightSpeed);

    return; // Exit the function
  }
}

  // Check for obstacles and update obstacle detection timer
  if (middle_distance < 12 || left_distance < 12 || right_distance < 12) {
    if (!obstacleDetected) {
      obstacleDetected = true;               // Start obstacle detection
      obstacleStartTime = millis();          // Record the time
    } else if (millis() - obstacleStartTime >= 1000) {
      // If obstacle detected for 500ms, reverse
      move_speed = -base_speed;             // Start reversing
      reverseStartTime = millis();          // Set reverse timer
      leftSpeed = move_speed;               // Set left and right speeds for reversing
      rightSpeed = move_speed;
      moveMotors();                         // Apply motor commands
      Serial.println("4. Obstacle too close! Reversing...");
      return;                               // Exit the function
    }
  } else {
    // Reset detection if no obstacle
    obstacleDetected = false;
    obstacleStartTime = 0;
  }



  // Normal driving logic
  move_speed = base_speed;

  // Adjust turning amplifier based on proximity
  if (left_distance < 30 || middle_distance < 35 || right_distance < 30) {
    turning_amplifier = 2.0; // Increase turning amplifier for proximity
    Serial.println("2. Turning amplifier increased due to proximity.");
  } else {
    turning_amplifier = 1.0; // Default turning amplifier
  }

  // Debug: Print reverseStartTime and millis for tracking
  Serial.print("3. reverse_start_time: ");
  Serial.print(reverseStartTime);
  Serial.print("\tmillis: ");
  Serial.println(millis());


  // Calculate the error based on the sensor readings
  float error = right_distance - left_distance;

  // PID control calculations with adjusted turning amplifier
  float pid_output = kp * error + ki * integral + kd * (error - previous_error);
  integral += error; // Accumulate the error for integral term
  integral = constrain(integral, -integral_limit, integral_limit);// Integral calculation with a limit
  previous_error = error; // Store current error for next cycle

  // Multiply the PID output by the turning amplifier for more aggressive turning
  pid_output *= turning_amplifier;

  // Debugging: Indicate autonomous mode and print error and PID output on the same line
  Serial.print("6. Automatic mode\t");
  Serial.print("Error: ");
  Serial.print(error);
  Serial.print("\tPID Output: ");
  Serial.println(pid_output);

  Serial.print("7. Speed before PID adjustment:\t");
  Serial.print("leftspeed: ");
  Serial.print(leftSpeed);
  Serial.print("\trightSpeed: ");
  Serial.print(rightSpeed);
  Serial.print("\tBase speed: ");
  Serial.print(base_speed);
  Serial.print("\tMove Speed: ");
  Serial.println(move_speed);

  // Adjust motor speeds based on PID output and turning amplifier
  leftSpeed = constrain(move_speed + pid_output, -speed_limit, speed_limit);
  rightSpeed = constrain(move_speed - pid_output, -speed_limit, speed_limit);

  moveMotors();  // Move the robot with the calculated speeds


}
