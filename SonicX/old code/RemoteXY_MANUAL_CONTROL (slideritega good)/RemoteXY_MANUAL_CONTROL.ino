/*
   -- SonicX_Project --
   
   This source code of graphical user interface 
   has been generated automatically by RemoteXY editor.
   To compile this code using RemoteXY library 3.1.13 or later version 
   download by link http://remotexy.com/en/library/
   To connect using RemoteXY mobile app by link http://remotexy.com/en/download/                   
     - for ANDROID 4.15.01 or later version;
     - for iOS 1.12.1 or later version;
    
   This source code is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.    
*/

//////////////////////////////////////////////
//        RemoteXY include library          //
//////////////////////////////////////////////

// you can enable debug logging to Serial at 115200
//#define REMOTEXY__DEBUGLOG    

// RemoteXY select connection mode and include library 
#define REMOTEXY_MODE__ESP32CORE_BLE

#include <BLEDevice.h>

// RemoteXY connection settings 
#define REMOTEXY_BLUETOOTH_NAME "SonicX_Bluetooth"


#include <RemoteXY.h>

// RemoteXY GUI configuration  
#pragma pack(push, 1)  
uint8_t RemoteXY_CONF[] =   // 448 bytes
  { 255,22,0,30,0,185,1,19,0,0,0,83,111,110,105,99,88,0,24,1,
  106,200,1,1,28,0,129,10,130,14,8,0,36,76,101,102,116,0,65,83,
  45,18,18,96,129,42,131,25,8,0,135,77,105,100,100,108,101,0,129,81,
  131,17,7,0,204,82,105,103,104,116,0,129,32,126,42,5,64,16,60,45,
  32,77,111,116,111,114,32,115,112,101,101,100,115,32,45,62,0,129,83,36,
  16,4,0,16,68,101,116,101,99,116,101,100,0,129,38,72,32,6,0,36,
  83,112,101,101,100,32,108,105,109,105,116,58,0,129,4,30,7,10,64,164,
  80,0,2,55,25,44,9,0,2,26,31,31,71,114,101,101,110,32,102,105,
  114,115,116,0,82,101,100,32,102,105,114,115,116,0,2,34,101,42,16,0,
  2,26,31,31,77,97,110,117,97,108,95,109,111,100,101,0,65,117,116,111,
  109,97,116,105,99,95,109,111,100,101,0,2,10,8,34,17,0,2,26,31,
  31,79,78,0,79,70,70,0,129,86,41,12,4,0,16,32,67,111,108,111,
  114,58,0,7,12,29,40,10,110,64,2,26,2,2,7,12,42,40,10,110,
  64,2,26,2,2,7,12,55,40,10,110,64,2,26,2,2,129,4,42,4,
  12,64,164,73,0,129,3,56,8,12,64,164,68,0,7,35,81,40,10,110,
  64,2,26,2,2,2,55,12,43,7,0,2,26,31,31,79,78,0,79,70,
  70,0,129,10,2,33,6,64,1,77,97,105,110,32,115,119,105,116,99,104,
  0,129,55,5,43,6,0,15,99,111,108,111,114,95,100,101,116,101,99,116,
  105,111,110,58,0,68,12,147,40,40,18,8,1,6,68,58,147,40,40,19,
  8,36,135,6,68,54,38,26,26,18,8,36,135,129,12,139,39,6,64,94,
  77,111,116,111,114,32,115,112,101,101,100,115,58,0,129,63,140,28,6,64,
  13,68,105,115,116,97,110,99,101,115,58,0,4,84,80,14,42,48,2,26,
  4,10,79,14,42,48,2,26 };
  
// this structure defines all the variables and events of your control interface 
struct {

    // input variables
  uint8_t green_first_switch; // =1 if switch ON and =0 if OFF
  uint8_t manual_mode_switch; // =1 if switch ON and =0 if OFF
  uint8_t Start_switch; // =1 if switch ON and =0 if OFF
  float kp;
  float ki;
  float kd;
  float speed_limit;
  uint8_t color_detecion; // =1 if switch ON and =0 if OFF
  int8_t right_slider; // from -100 to 100
  int8_t left_slider; // from -100 to 100

    // output variables
  uint8_t led_red; // =0..255 LED Red brightness
  uint8_t led_green; // =0..255 LED Green brightness
  float left_speed_graph;
  float right_speed_graph;
  float left_sensor_graph;
  float middle_sensor_graph;
  float right_sensor_graph;
  float red_color_graph;
  float green_color_graph;

    // other variable
  uint8_t connect_flag;  // =1 if wire connected, else =0

} RemoteXY;   
#pragma pack(pop)
 
/////////////////////////////////////////////
//           END RemoteXY include          //
/////////////////////////////////////////////



const int motorA1 = 32, motorA2 = 33, PWMA = 25;
const int motorB1 = 26, motorB2 = 27, PWMB = 14;

 //defining it but defining in code:
int leftSpeed=0;
int rightSpeed=0;
int maxSpeed=0;

//defined here:
int baseSpeed = 150;

void setup() 
{
  Serial.begin(115200);
  RemoteXY_Init (); 
  
  // Set motor control pins
  pinMode(motorA1, OUTPUT); pinMode(motorA2, OUTPUT);
  pinMode(motorB1, OUTPUT); pinMode(motorB2, OUTPUT);
  pinMode(PWMA, OUTPUT); pinMode(PWMB, OUTPUT);
  
}

void loop() 
{ 
  RemoteXY_Handler ();

  maxSpeed=RemoteXY.speed_limit;

  //if MAIN_SWITCH ON
  if (RemoteXY.Start_switch==1) {
  //if MANUAL_MODE
    if (RemoteXY.manual_mode_switch==1) {
      leftSpeed = map(RemoteXY.left_slider, -100, 100, -maxSpeed, maxSpeed); // Decrease speed of left wheel when turning
      rightSpeed = map(RemoteXY.right_slider, -100, 100, -maxSpeed, maxSpeed); // Increase speed of right wheel when turning
      moveMotors();
      Serial.print('Manual mode');
    
  //if AUTOMATIC_MODE
    } else{ 
      Serial.print('Automatic mode');
    }



//if MAIN_SWITCH OFF
  } else{
    stopMotors();
    Serial.print('Main switch off!');
  }


  //debug GUI
  RemoteXY.left_speed_graph = leftSpeed;
  RemoteXY.right_speed_graph = rightSpeed;

  RemoteXY_delay(50); 
}






void moveMotors() {
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

  // Apply the mapped speeds to the PWM pins
  analogWrite(PWMA, abs(rightSpeed)); 
  analogWrite(PWMB, abs(leftSpeed));

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
  Serial.println("Motors stopped.");
}