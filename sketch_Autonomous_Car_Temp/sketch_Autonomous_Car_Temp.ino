#include <Adafruit_MotorShield.h>

// Create the motor shield object with the default I2C address
Adafruit_MotorShield AFMS = Adafruit_MotorShield(); 
// Or, create it with a different I2C address (say for stacking)
// Adafruit_MotorShield AFMS = Adafruit_MotorShield(0x61); 

// Assign 'port' M1, M2, M3 or M4.
Adafruit_DCMotor *Motor1 = AFMS.getMotor(1);
Adafruit_DCMotor *Motor2 = AFMS.getMotor(2);
Adafruit_DCMotor *Motor3 = AFMS.getMotor(3);
Adafruit_DCMotor *Motor4 = AFMS.getMotor(4);

/*
 * Hardware Connections:
 *  Arduino | HC-SR04 
 *  -------------------
 *    5V    |   VCC     
 *    6     |   Trig  (Front)     
 *    7     |   Echo  (Front)     
 *    8     |   Trig  (Rear)
 *    9     |   Echo  (Rear)
 *    GND   |   GND
*/

// Sonar Pins
const int TRIG_PIN = 6;
const int ECHO_PIN = 7;
const int rearTRIG_PIN = 8;
const int rearECHO_PIN = 9;

int drive(int x, int y);

// Anything over 400 cm (23200 us pulse) is "out of range"
const unsigned int MAX_DIST = 23200;

// Control variables
float y =0.1, Front_Ref, Front_Error = 0, Front_Error_Prev=0, Front_Error_Diferential_Prev=0, Front_Error_Diferential = 0, Front_Error_Integral = 0, u = 0; 
float Kp=60, Kd=20, Ki=20;

/* Encoder variables
const int encoderIn0 = 2; // input pin for the interrupter 
const int encoderIn1 = 3; // input pin for the interrupter 
const int encoderIn2 = 4; // input pin for the interrupter 
const int encoderIn3 = 5; // input pin for the interrupter 
int detectState0=0; // Variable for reading the encoder status
int detectState1=0; // Variable for reading the encoder status
int detectState2=0; // Variable for reading the encoder status
int detectState3=0; // Variable for reading the encoder status
*/

// Flags:
bool start = 0;

// Calibration parameters
int *Initial_Speed;

void setup() 
{

  // The Trigger pin will tell the sensor to range find
  
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(rearTRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT); // Sets the echoPin as an Input
  pinMode(rearECHO_PIN, INPUT);
  
  digitalWrite(rearTRIG_PIN, LOW);
  digitalWrite(TRIG_PIN, LOW);
  
  Serial.begin(9600);           // set up Serial library at 9600 bps
  Serial.println("Setuping the vehicle...");

  AFMS.begin();  // create with the default frequency 1.6KHz
  
  // Set the speed to start, from 0 (off) to 255 (max speed)
  Motor1->setSpeed(150);
  Motor1->run(FORWARD);
  Motor1->run(RELEASE);

  Motor2->setSpeed(150);
  Motor2->run(FORWARD);
  Motor2->run(RELEASE);

  Motor3->setSpeed(150);
  Motor3->run(FORWARD);
  Motor3->run(RELEASE);

  Motor4->setSpeed(150);
  Motor4->run(FORWARD);
  Motor4->run(RELEASE);
/*
  //Set input pins
  pinMode(encoderIn0, INPUT);
  pinMode(encoderIn1, INPUT);
  pinMode(encoderIn2, INPUT);
  pinMode(encoderIn3, INPUT); 
*/
  pinMode(LED_BUILTIN, OUTPUT); //Set pin 13 as output LED
}

float GetSonar(float y_prev){

  unsigned long t1;
  unsigned long t2;
  unsigned long pulse_width;
  float cm;
  float y,tau=0.5;

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
  y = y_prev+tau*(cm-y_prev);  // First order filtering of the results
  y_prev = y;
  
// Print out results
//  if ( pulse_width > MAX_DIST ) {
//    Serial.println("Out of range");
//  } else {
//   Serial.println(cm);
//    Serial.println(y);
//  }
  // Wait at least 60ms before next measurement
  delay(100);
  return(y/100);
}

// ########################## functions #######################

int drive(int l, int r){

  if (l > 255 || r>255){
  Serial.println("Error: Drive input value is greater than 255");
  return 1;
  }
    
  char lDirection, rDirection;
  if (l > 0){
    lDirection = FORWARD;
  }
  else{
    lDirection = BACKWARD;
  }
  if (r > 0){
    rDirection = FORWARD;
  }
  else{
    rDirection = BACKWARD;
  }
  
  Motor1->setSpeed(abs(l));
  Motor1->run(lDirection);
  
  Motor2->setSpeed(abs(l));
  Motor2->run(lDirection);
  
  Motor3->setSpeed(abs(r));
  Motor3->run(rDirection);
  
  Motor4->setSpeed(abs(r));
  Motor4->run(rDirection);
  return 0;
}

void loop() {

// ############################ Flag to Start algorithm ##############
switch (start)
{
  case 0:
 {
  y = GetSonar(y);
  if (y <= 0.05) {
    delay(3000);
    start =1;
    Serial.println("Starting algorithm");
  }
  }
  break;
  case 1:
 { 

// ########################### Algorithm #############################
float y = GetSonar(y); // obtain distance to obstacle
Front_Ref= 1.2; 
Front_Error= Front_Ref-y;
Front_Error_Diferential = (Front_Error - Front_Error_Prev)/0.025;
Front_Error_Integral = Front_Error_Integral + (Front_Error*0.025); // Estimated sampling rate is 40 Hz
u=  -1*(Kp*Front_Error+ Ki*Front_Error_Integral+ Kd*Front_Error_Diferential);

Serial.print(y);
Serial.print(",");
Serial.print(Front_Ref);
Serial.print(",");
Serial.print(u/255);
Serial.print(",");
Serial.println(Front_Error);

// Save to previous step
Front_Error_Prev = Front_Error;
drive(u,u);
/*
// ############################ Encoder ########################
detectState0=digitalRead(encoderIn0);
   if (detectState0 == HIGH) { //If encoder output is high
      digitalWrite(LED_BUILTIN, HIGH); //Turn on the status LED
   }
   else {
      digitalWrite(LED_BUILTIN, LOW); //Turn off the status LED
   }
*/
break;
}
}
}
