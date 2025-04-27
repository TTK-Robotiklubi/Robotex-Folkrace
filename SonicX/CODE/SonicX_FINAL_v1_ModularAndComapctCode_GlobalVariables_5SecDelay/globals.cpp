#include "globals.h"
#include <Arduino.h>

// Define the global variables //this made these remoteXY variables usable globally(in every file)
bool green_first_switch = false;
bool manual_mode_switch = false;
bool Start_switch = false;
float kp = 0.0;
float ki = 0.0;
float kd = 0.0;
float speed_limit = 0.0;
float Color_threshold_value = 0.0;
uint8_t color_detecion = 0;
int8_t left_slider = 0;
int8_t right_slider = 0;
int leftSpeed = 0;
int rightSpeed = 0;


void moveMotors() {
  // Apply the mapped speeds to the PWM pins
  analogWrite(PWMA, abs(rightSpeed)); 
  analogWrite(PWMB, abs(leftSpeed));

  // Forward/Reverse control based on leftSpeed (Y-axis)
  if (rightSpeed > 0) {
      digitalWrite(motorA1, HIGH); 
      digitalWrite(motorA2, LOW);
  } else if (rightSpeed < 0) {
      digitalWrite(motorA1, LOW); 
      digitalWrite(motorA2, HIGH);
  } else {
      digitalWrite(motorA1, LOW); 
      digitalWrite(motorA2, LOW);
  }

  // Spin in place control based on rightSpeed (X-axis)
  if (leftSpeed > 0) {
      digitalWrite(motorB1, LOW); // Inverted motor B direction (right motor moves backwards)
      digitalWrite(motorB2, HIGH);
  } else if (leftSpeed < 0) {
      digitalWrite(motorB1, HIGH); 
      digitalWrite(motorB2, LOW);
  } else {
      digitalWrite(motorB1, LOW); 
      digitalWrite(motorB2, LOW);
  }

  // Debugging output
  Serial.print("Left Speed: ");
  Serial.print(leftSpeed);
  Serial.print(", Right Speed: ");
  Serial.println(rightSpeed);
}
