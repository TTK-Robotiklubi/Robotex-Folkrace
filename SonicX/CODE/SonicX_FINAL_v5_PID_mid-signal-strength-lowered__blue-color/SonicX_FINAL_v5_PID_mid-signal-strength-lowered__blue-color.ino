#include <Wire.h>
#include "RemoteXY_Config.h" //ONLY FOR INITIAL SETUP
#include "globals.h"
#include "distance_sensors.h"  // Include your distance sensor header file
#include "color_sensor.h"  // Include the color sensor header file
#include "debug.h"    // Include the debug header for serial output
#include "autonomousDriving.h"
#include "manualDriving.h"

int motorA1 = 32, motorA2 = 33, PWMA = 25, motorB1 = 26, motorB2 = 27, PWMB = 14;
unsigned long startMillis = 0;  // Variable to store the time when the Start_switch is turned on
bool waitingForStart = false;   // Flag to check if we are waiting for the delay


void setup() {
  Serial.begin(115200);
  RemoteXY_Init (); 
  motor_pinMode_setup();// Set motor control pins
  Wire.begin();  // Initialize I2C
  initSensors();// Initialize distance sensors (if applicable)
  initializeColorSensor();// Initialize the color sensor

//Initial RemoteXY GUI values
  RemoteXY.kp=1.5;
  RemoteXY.ki=0.02;
  RemoteXY.kd = 3.5;
  RemoteXY.speed_limit =255;
  RemoteXY.Color_threshold_value =650;

}

void loop() { 
  RemoteXY_Handler (); //didn't understand what this is here for, gui worked well if i commented it out

// Read and process data from the distance sensors
  readSensors(distances);

// Read color sensor data
  String detectedColor = detectColor();
  uint16_t red = readColor(RED_DATA_LSB);
  uint16_t green = readColor(GREEN_DATA_LSB);
  uint16_t blue = readColor(BLUE_DATA_LSB);
//debug serial + plotter
  // Send the distance sensor data to the Serial Monitor for debugging
  sendToSerialPlotter(distances);
  // Send the color sensor data to the Serial Monitor for debugging
  sendColorDataToSerial(detectedColor, red, green, blue); //while dealing with basic driving autonomous part
//

//if WRONG speed_limit is set:
  if (RemoteXY.speed_limit >255) {
    RemoteXY.speed_limit =255;
  }

//RemoteXY varibles -> global variables
  green_first_switch= RemoteXY.green_first_switch; // =1 if switch ON and =0 if OFF
  manual_mode_switch= RemoteXY.manual_mode_switch; // =1 if switch ON and =0 if OFF
  Start_switch= RemoteXY.Start_switch; // =1 if switch ON and =0 if OFF
  kp = RemoteXY.kp;
  ki = RemoteXY.ki;
  kd= RemoteXY.kd;
  speed_limit= RemoteXY.speed_limit;
  color_detecion = RemoteXY.color_detecion; // =1 if switch ON and =0 if OFF
  left_slider= RemoteXY.left_slider; // from -100 to 100
  Color_threshold_value= RemoteXY.Color_threshold_value;
  right_slider = RemoteXY.right_slider; // from -100 to 100

//if MAIN_SWITCH ON
  if (RemoteXY.Start_switch==1) {
    // If not already waiting for the 5-second delay
    if (waitingForStart == false) {
      startMillis = millis();  // Record the time when the main switch was turned on
      waitingForStart = true;  // Set the flag to true, indicating we're waiting
      Serial.println("1.5 Waiting for delay to start...");
    }

  // If 5 seconds have passed since the switch was turned on
    if (millis() - startMillis >= 5000) { //should be 5000 for 5 sec
      //Serial.println("1.7 Delay is over now..."); // After the 5-second delay, check for manual or automatic mode
    //if MANUAL_MODE
      if (RemoteXY.manual_mode_switch==1) {
        manual_mode();
    //if AUTOMATIC_MODE
      } else{ 
        autonomous_mode();
      }
    }
//if MAIN_SWITCH OFF
  } else{
    waitingForStart = false; // Reset the waiting flag
    leftSpeed=0;
    rightSpeed=0;
    moveMotors();
    Serial.println("Main switch off!");
  }
//

//RemoteXY GUI indicators
  RemoteXY.led_red = map (isRed,0,1,0,255);
  RemoteXY.led_green = map (isGreen,0,1,0,255);
  RemoteXY.red_color_graph = red;
  RemoteXY.green_color_graph = green;
  RemoteXY.blue_color_graph = blue;
  RemoteXY.left_sensor_graph = distances[0];
  RemoteXY.middle_sensor_graph = distances[1];
  RemoteXY.right_sensor_graph = distances[2];
  RemoteXY.left_speed_graph = leftSpeed;
  RemoteXY.right_speed_graph = rightSpeed;


  RemoteXY_delay(20); //can be even smaller number?? ok for sensors on 70cm? 10m worked also //DO I EVEN NEED THIS DELAY? worked with it commented out? 20 worked ok, but id I is bigger then 0, then it gets bad
} 

void motor_pinMode_setup(){// Set motor control pins
  pinMode(motorA1, OUTPUT); pinMode(motorA2, OUTPUT);
  pinMode(motorB1, OUTPUT); pinMode(motorB2, OUTPUT);
  pinMode(PWMA, OUTPUT); pinMode(PWMB, OUTPUT);
}