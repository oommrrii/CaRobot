/**
  * Hardware Connections:
 *  Arduino | HC-SR04 
 *  -------------------
 *    5V    |   VCC     
 *    6     |   Trig     
 *    7     |   Echo     
 *    GND   |   GND
 */

// Pins
const int TRIG_PIN = 6;
const int ECHO_PIN = 7;

// Anything over 400 cm (23200 us pulse) is "out of range"
const unsigned int MAX_DIST = 23200;

  unsigned long t1;
  unsigned long t2;
  unsigned long pulse_width;
  float cm = 0, y_prev[4] = {0,0,0,0}, y[4] = {0,0,0,0} ,tau[4] = {1,0.5,0.25,0.125};
  int i = 0;

void setup() {

  // The Trigger pin will tell the sensor to range find
  pinMode(TRIG_PIN, OUTPUT);
  digitalWrite(TRIG_PIN, LOW);

  // We'll use the serial monitor to view the sensor output
  Serial.begin(9600);
}

void loop() {
  
  // Hold the trigger pin high for at least 10 us
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // Wait for pulse on echo pin
  while ( digitalRead(ECHO_PIN) == 0 );

  // Measure how long the echo pin was held high (pulse width)
  // Note: the micros() counter will overflow after ~70 min
  t1 = micros();
  while ( digitalRead(ECHO_PIN) == 1);
  t2 = micros();
  pulse_width = t2 - t1;

  // Calculate distance in centimeters. The constants
  // are found in the datasheet, and calculated from the assumed speed 
  //of sound in air at sea level (~340 m/s).
  cm = pulse_width / 58.0;
//  y[0] = cm - tau[0]*y_prev[0];  // First order filtering of the results
//  y_prev[0] = y[0];
  
//Choose between different time constants

  while (i<4){
  y[i] = y_prev[i]+tau[i]*(cm - y_prev[i]);  // First order filtering of the results
  y_prev[i] = y[i];
  i++;
  }
  i = 0;

  
// Print out results for verification
  if ( pulse_width > MAX_DIST ) {
    Serial.println("Out of range");
  } else {
    Serial.print(cm);
    Serial.print(",");
    Serial.print(y[0]);
    Serial.print(",");
    Serial.print(y[1]);
    Serial.print(",");
    Serial.print(y[2]);
    Serial.print(",");
    Serial.println(y[3]);
    }

// Wait at least 60ms before next measurement
  delay(100);
}
