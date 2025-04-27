#ifndef GLOBALS_H
#define GLOBALS_H
#include <cstdint> // Ensure uint8_t and int8_t are defined

extern int motorA1, motorA2, PWMA, motorB1, motorB2, PWMB; //for using these variables in moveMotors().

//from RemoteXY, making them global to use in all files, that use "#include globals.h"
extern bool green_first_switch;    // =1 if switch ON and =0 if OFF
extern bool manual_mode_switch;    // =1 if switch ON and =0 if OFF
extern bool Start_switch;          // =1 if switch ON and =0 if OFF
extern float kp;
extern float ki;
extern float kd;
extern float speed_limit;
extern float Color_threshold_value;
extern uint8_t color_detecion;     // =1 if switch ON and =0 if OFF
extern int8_t left_slider;         // From -100 to 100
extern int8_t right_slider;        // From -100 to 100
extern int leftSpeed;
extern int rightSpeed;

extern void moveMotors();


#endif