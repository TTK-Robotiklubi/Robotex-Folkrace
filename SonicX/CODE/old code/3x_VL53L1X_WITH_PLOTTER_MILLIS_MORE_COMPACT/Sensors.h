// Sends data to Serial Plotter
void sendToSerialPlotter(const int distances[]) {
  Serial.print("Left:");
  Serial.print(distances[0]);
  Serial.print(",    Middle:");
  Serial.print(distances[1]);
  Serial.print(",    Right:");
  Serial.println(distances[2]);
}