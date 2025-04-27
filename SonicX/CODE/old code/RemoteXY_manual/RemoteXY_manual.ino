#include <Wire.h>
#include "RemoteXY_Config.h" //ONLY FOR INITIAL SETUP
#include "motor_config.h" //ONLY FOR INITIAL SETUP
#include "sensors.h"  // Include your distance sensor header file
#include "color_sensor.h"  // Include the color sensor header file
#include "debug.h"    // Include the debug header for serial output

int baseSpeed = 150;

void setup() {
  Serial.begin(115200);
  RemoteXY_Init (); 
  motor_pinMode_setup();// Set motor control pins
  Wire.begin();  // Initialize I2C
  initSensors();// Initialize distance sensors (if applicable)
  initializeColorSensor();// Initialize the color sensor

//Initial GUI values
  RemoteXY.kp=10;
  RemoteXY.ki=0.02;
  RemoteXY.kd = 15;;
  RemoteXY.speed_limit =150;
  RemoteXY.Color_threshold_value =460;
}

void loop() { 
  RemoteXY_Handler ();

// Read and process data from the distance sensors
  int distances[3];  // Assuming you have 3 sensors
  readSensors(distances);

// Read color sensor data
  String detectedColor = detectColor();
  uint16_t red = readColor(RED_DATA_LSB);
  uint16_t green = readColor(GREEN_DATA_LSB);

//if WRONG MAX_SPEED is set:
  if (RemoteXY.speed_limit >255) {
    RemoteXY.speed_limit =255;
  }

//if MAIN_SWITCH ON
  if (RemoteXY.Start_switch==1) {
  //if MANUAL_MODE
    if (RemoteXY.manual_mode_switch==1) {
      leftSpeed = map(RemoteXY.left_slider, -100, 100, -RemoteXY.speed_limit, RemoteXY.speed_limit); // Decrease speed of left wheel when turning
      rightSpeed = map(RemoteXY.right_slider, -100, 100, -RemoteXY.speed_limit, RemoteXY.speed_limit); // Increase speed of right wheel when turning
      moveMotors();
      Serial.println("Manual mode");
    
  //if AUTOMATIC_MODE
    } else{ 
      Serial.println("Automatic mode");
    }



  //if MAIN_SWITCH OFF
  } else{
    stopMotors();
    Serial.println("Main switch off!");
  }


//GUI indicators
  //color
  RemoteXY.led_red = map (isRed,0,1,0,255);
  RemoteXY.led_green = map (isGreen,0,1,0,255);
  RemoteXY.red_color_graph = red;
  RemoteXY.green_color_graph = green;
  Color_threshold_value= RemoteXY.Color_threshold_value;
  //distance
  RemoteXY.left_sensor_graph = distances[0];
  RemoteXY.middle_sensor_graph = distances[1];
  RemoteXY.right_sensor_graph = distances[2];
  //speed
  RemoteXY.left_speed_graph = leftSpeed;
  RemoteXY.right_speed_graph = rightSpeed;

//debug serial + plotter
  // Send the distance sensor data to the Serial Monitor for debugging
  sendToSerialPlotter(distances);
  // Send the color sensor data to the Serial Monitor for debugging
  sendColorDataToSerial(detectedColor, red, green);
//
  RemoteXY_delay(10); //can be even smaller number?? ok for sensors on 70cm?
}

void motor_pinMode_setup(){// Set motor control pins
  pinMode(motorA1, OUTPUT); pinMode(motorA2, OUTPUT);
  pinMode(motorB1, OUTPUT); pinMode(motorB2, OUTPUT);
  pinMode(PWMA, OUTPUT); pinMode(PWMB, OUTPUT);
}

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

void reverse() {
  digitalWrite(motorA1, LOW); digitalWrite(motorA2, HIGH);
  digitalWrite(motorB1, LOW); digitalWrite(motorB2, HIGH);
  analogWrite(PWMA, baseSpeed);
  analogWrite(PWMB, baseSpeed);
  Serial.println("Reversing.");
}

void stopMotors() {
  digitalWrite(motorA1, LOW); digitalWrite(motorA2, LOW);
  digitalWrite(motorB1, LOW); digitalWrite(motorB2, LOW);
  analogWrite(PWMA, 0);
  analogWrite(PWMB, 0);
  //Serial.println("Motors stopped.");
}