// Define ultrasonic sensor pins
const int trigPin3 = 21;  // Trigger pin for sensor 1 (LEFT)
const int echoPin3 = 19;  // Echo pin for sensor 1 (LEFT)
const int trigPin2 = 22;  // Trigger pin for sensor 2 (MIDDLE)
const int echoPin2 = 18;  // Echo pin for sensor 2 (MIDDLE)
const int trigPin1 = 23;  // Trigger pin for sensor 3 (RIGHT)
const int echoPin1 = 17;  // Echo pin for sensor 3 (RIGHT)

// Motor control pins
const int motorA1 = 25; // Motor A direction pin 1 (Forward)
const int motorA2 = 26; // Motor A direction pin 2 (Backward)
const int motorB1 = 14; // Motor B direction pin 1 (Forward, reversed)
const int motorB2 = 27; // Motor B direction pin 2 (Backward, reversed)
const int PWMA = 12;    // Motor A PWM pin
const int PWMB = 13;    // Motor B PWM pin

// Control parameters
const int baseSpeed = 150*0.4;  // Base speed for forward movement 150
const int maxSpeed = 255*0.4;   // Maximum speed 255
//const int obstacleDistance = 25; // Threshold distance for obstacles (in cm) //not in use

// PID control constants
float Kp = 1;  // Proportional constant
float Ki = 0;  // Integral constant (smaller for smoother control)
float Kd = 0;  // Derivative constant
float previousError = 0;
float integral = 0;
float integralLimit = 50; // Limit for integral term to avoid windup
float derivativeLimit = 50; // Limit for derivative term
float distance1_and_distance3_limit=550;

// Variables to store sensor data
long distance1, distance2, distance3;

// Setup function to initialize pins
void setup() {
  Serial.begin(9600);  // Start serial communication for debugging

  // Set ultrasonic sensor pins
  pinMode(trigPin1, OUTPUT); pinMode(echoPin1, INPUT);
  pinMode(trigPin2, OUTPUT); pinMode(echoPin2, INPUT);
  pinMode(trigPin3, OUTPUT); pinMode(echoPin3, INPUT);

  // Set motor control pins
  pinMode(motorA1, OUTPUT); pinMode(motorA2, OUTPUT);
  pinMode(motorB1, OUTPUT); pinMode(motorB2, OUTPUT);
  pinMode(PWMA, OUTPUT); pinMode(PWMB, OUTPUT);

  // Initialize motors to stop
  stopMotors();
}

// Main loop for controlling robot movement
void loop() {
  // Measure distances from all sensors
  distance1 = measureDistance(trigPin3, echoPin3); // LEFT sensor
  distance2 = measureDistance(trigPin2, echoPin2); // MIDDLE sensor
  distance3 = measureDistance(trigPin1, echoPin1); // RIGHT sensor

 // Sensor output distance limit
  if (distance1 > distance1_and_distance3_limit) {
    distance1 = distance1_and_distance3_limit;
  }
  if (distance3 > distance1_and_distance3_limit){
    distance3 = distance1_and_distance3_limit;
  }

  // Print sensor data for debugging
  Serial.print("Left: "); Serial.print(distance1); 
  Serial.print(" cm, Middle: "); Serial.print(distance2); 
  Serial.print(" cm, Right: "); Serial.print(distance3);

  // Calculate the general error based on the difference between left and right sensors
  float generalError = distance1 - distance3;  // Error between left and right sensors

  // Print the general error to debug
  Serial.print("\tGeneral Error: "); Serial.print(generalError);

  // Amplify turning based on middle sensor distance
  float turningAmplifier = 1.0;  // Default multiplier for turning speed
  if (distance1 < 10|| distance2 < 15 || distance3 < 10) {
    reverse();  // Reverse if obstacle is very close
    delay(500);  // Reverse for 2 seconds
    return;  // Skip the rest of the logic during reverse
  } else if (distance2 < 25) {
    turningAmplifier = 1.5;  // Amplify turning when middle sensor detects closer to 45cm
      if (distance1 < 25 & distance3 < 25) {
        turningAmplifier = 5.0;
      }
  } else if (distance2 >= 25) {
    turningAmplifier = 1.0;  // Normal turning speed if no obstacle
  }

  // PID control for motor speeds based on the general error
  int leftSpeed  = baseSpeed - pidControl(generalError) * turningAmplifier; // Left motor speed with turning amplifier to amplify speed based error
  int rightSpeed = baseSpeed + pidControl(generalError) * turningAmplifier; // Right motor speed with turning amplifier to amplify speed based error

  // Amplify speed based on the turning amplifier
  // leftSpeed *= turningAmplifier;
  // rightSpeed *= turningAmplifier;

  // Apply motor speed limits
  leftSpeed = constrain(leftSpeed, 0, maxSpeed);
  rightSpeed = constrain(rightSpeed, 0, maxSpeed);

  // Print the motor speeds to debug
  Serial.print(" Turning amplifier: "); Serial.print(turningAmplifier);
  Serial.print("\tLeft Speed: "); Serial.print(leftSpeed);
  Serial.print(" Right Speed: "); Serial.println(rightSpeed);

  // Move forward with adjusted speeds
  moveForward(leftSpeed, rightSpeed);

  delay(100); // Wait before the next iteration
}

// Function to measure distance using ultrasonic sensor
long measureDistance(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH);
  return (duration / 2) / 29.1; // Convert to centimeters
}

// Function to calculate PID control output
float pidControl(float error) {
  integral += error;
  
  // Limit integral term to avoid windup
  integral = constrain(integral, -integralLimit, integralLimit);

  float derivative = error - previousError;
  
  // Limit derivative term to smooth response
  derivative = constrain(derivative, -derivativeLimit, derivativeLimit);
  
  previousError = error;
  return Kp * error + Ki * integral + Kd * derivative;
}

// Function to move the robot forward with PID-controlled speeds
void moveForward(int leftSpeed, int rightSpeed) {
  digitalWrite(motorA1, HIGH); // Motor A goes forward
  digitalWrite(motorA2, LOW);   // Motor A forward direction
  digitalWrite(motorB1, HIGH);  // Motor B goes forward (reversed)
  digitalWrite(motorB2, LOW);   // Motor B forward direction (reversed)
  analogWrite(PWMA, leftSpeed); // Set speed for Motor A
  analogWrite(PWMB, rightSpeed); // Set speed for Motor B
}

// Function to reverse the robot
void reverse() {
  digitalWrite(motorA1, LOW);   // Motor A goes backward
  digitalWrite(motorA2, HIGH);  // Motor A backward direction
  digitalWrite(motorB1, LOW);   // Motor B goes backward (reversed)
  digitalWrite(motorB2, HIGH);  // Motor B backward direction (reversed)
  analogWrite(PWMA, baseSpeed); // Set speed for Motor A
  analogWrite(PWMB, baseSpeed); // Set speed for Motor B
}

// Function to stop all motors
void stopMotors() {
  digitalWrite(motorA1, LOW);
  digitalWrite(motorA2, LOW);
  digitalWrite(motorB1, LOW);
  digitalWrite(motorB2, LOW);
  analogWrite(PWMA, 0); // Stop Motor A
  analogWrite(PWMB, 0); // Stop Motor B
}
