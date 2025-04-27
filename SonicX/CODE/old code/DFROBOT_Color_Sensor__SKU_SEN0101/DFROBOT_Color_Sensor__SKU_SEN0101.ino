int s0 = 25, s1 = 26, s2 = 27, s3 = 14;  // Pins for TCS3200 control
int out = 34;                            // OUT pin connected to GPIO34 (interrupt-capable)
volatile int counter = 0;                // Pulse counter
volatile int flag = 0;                   // Color reading stage flag

int countR = 0, countG = 0, countB = 0;  // Store counts for Red, Green, Blue

void setup() {
  Serial.begin(115200);

  // Set up pins
  pinMode(s0, OUTPUT);
  pinMode(s1, OUTPUT);
  pinMode(s2, OUTPUT);
  pinMode(s3, OUTPUT);
  pinMode(out, INPUT);

  // Set frequency scaling to 20% (S0 = HIGH, S1 = LOW)
  digitalWrite(s0, HIGH);
  digitalWrite(s1, LOW);

  // Attach interrupt to count pulses
  attachInterrupt(digitalPinToInterrupt(out), ISR_INTO, RISING);
}

void loop() {
  readColor();  // Function to read color data

  // Print RGB values
  Serial.print("Red: ");
  Serial.print(countR);
  Serial.print(", Green: ");
  Serial.print(countG);
  Serial.print(", Blue: ");
  Serial.println(countB);

  // Determine the color and display it
  determineColor();

  delay(0);  // Wait before next reading
}

void readColor() {
  // Read Red
  digitalWrite(s2, LOW);
  digitalWrite(s3, LOW);
  counter = 0;
  delay(10);  // Measure pulses for 10ms
  countR = counter;

  // Read Green
  digitalWrite(s2, HIGH);
  digitalWrite(s3, HIGH);
  counter = 0;
  delay(10);  // Measure pulses for 10ms
  countG = counter;

  // Read Blue
  digitalWrite(s2, LOW);
  digitalWrite(s3, HIGH);
  counter = 0;
  delay(10);  // Measure pulses for 10ms
  countB = counter;
}

void determineColor() {
  // If all RGB values are below 100, consider it black
  if (countR < 40 && countG < 40 && countB < 40) {
    Serial.println("Detected Color: Black");
  } else if (countR > countG && countR > countB) {
    Serial.println("Detected Color: Red");
  } else if (countG > countR && countG > countB) {
    Serial.println("Detected Color: Green");
  } else if (countB > countR && countB > countG) {
    Serial.println("Detected Color: Blue");
  } else {
    Serial.println("Detected Color: Unclear/Mixed");
  }
}

void ISR_INTO() {
  counter++;
}
