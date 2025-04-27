#include "manualDriving.h" //main headder file for this
#include "globals.h" //for seeing left speed, rightspeed, move motors

#include <Arduino.h> //for serial printing

void manual_mode(){
  leftSpeed = map(left_slider, -100, 100, -speed_limit, speed_limit); // Decrease speed of left wheel when turning
  rightSpeed = map(right_slider, -100, 100, -speed_limit, speed_limit); // Increase speed of right wheel when turning
  moveMotors();
  Serial.println("Manual mode");
}
