#include "autonomousDriving.h" //main headder file for this
#include "globals.h" //for seeing left speed, rightspeed, move motors
#include "distance_sensors.h" //for seeing distance values "extern int distances[3];""
#include <Arduino.h> //for serial printing and more? Arduino functionality

void autonomous_mode(){
  leftSpeed=distances[0]; //testing if it does atleast something
  rightSpeed=distances[2]; //testing if it does atleast something
  moveMotors();
  Serial.println("Automatic mode");
}