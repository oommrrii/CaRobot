//#include <Wire.h>
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

// defines Sonar pins numbers
const int frontTrigPin = 6;
const int frontEchoPin = 7;
const int rearTrigPin = 8;
const int rearEchoPin = 9;

// defines  Sonar variables
long frontDuration;
float Distance[2]; //1 for front distance; 2 for rear distance
long rearDuration;

// Control variables
float Front_Ref, Front_Error, Front_Error_Prev=0, Front_Error_Diferential_Prev=0, Front_Error_Diferential, Front_Error_Integral, u, Kp=50, Kd=4, Ki=5;

// Encoder variables
const int encoderIn0 = 2; // input pin for the interrupter 
const int encoderIn1 = 3; // input pin for the interrupter 
const int encoderIn2 = 4; // input pin for the interrupter 
const int encoderIn3 = 5; // input pin for the interrupter 
int detectState0=0; // Variable for reading the encoder status
int detectState1=0; // Variable for reading the encoder status
int detectState2=0; // Variable for reading the encoder status
int detectState3=0; // Variable for reading the encoder status

// Flags:
bool start = 0, Calibrated_Initial_Speed = 0;

// Calibration parameters
int *Initial_Speed;



void setup() {
  Serial.begin(9600);           // set up Serial library at 9600 bps
  Serial.println("Setuping the vehicle...");

  AFMS.begin();  // create with the default frequency 1.6KHz
  //AFMS.begin(1000);  // OR with a different frequency, say 1KHz
  
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

  pinMode(frontTrigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(frontEchoPin, INPUT); // Sets the echoPin as an Input
  pinMode(rearTrigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(rearEchoPin, INPUT); // Sets the echoPin as an Input


  //Set input pins
  pinMode(encoderIn0, INPUT);
  pinMode(encoderIn1, INPUT);
  pinMode(encoderIn2, INPUT);
  pinMode(encoderIn3, INPUT);
  //Set pin 13 as output LED
  pinMode(LED_BUILTIN, OUTPUT);
  
}


void loop() {

// ############################ Flag to Start algorithm ##############

if (start == 0) {
  GetSonar();
  if (Distance[1] <= 0.04) {
    start =1;
    Serial.println("Starting algorithm");
  }
  return;
}

// ########################### Calibration ###########################

// Calibrating the initiating wheels speed
if (Calibrated_Initial_Speed == 0){
  delay(2000);
  Serial.println("Starting initial speed calibration");
  Initial_Speed = Cal_initial_speed();
  Serial.print("Calibration is done.\nInitial forward/backward speed is: ");
  Serial.print(Initial_Speed[1]);
  Serial.print(" / ");
  Serial.println(Initial_Speed[2]);
}

// ########################### Algorithm #############################

Smooth_Sonar(); // obtain distance values
Front_Ref= 0.31; 
Front_Error= Front_Ref-Distance[1];
Front_Error_Diferential= (Front_Error + Front_Error_Prev)/100;
//Front_Error_Diferential= Front_Error_Diferential_Prev + 2* (Front_Error - Front_Error_Prev);
Front_Error_Integral= Front_Error_Integral + Front_Error/5; // Estimated sampling rate is 500 Hz
u= -1 * (Kp*Front_Error + Kd*Front_Error_Diferential);// + Ki*Front_Error_Integral);

Serial.println(Front_Error_Diferential);

// Save to previous step
Front_Error_Prev = Front_Error;
Front_Error_Diferential_Prev = Front_Error_Diferential;

// Perform output
if (u > 0){
  drive(u + Initial_Speed[1],u + Initial_Speed[1]);
}
else if (u < 0){
  drive(u + Initial_Speed[2],u + Initial_Speed[2]);
}
else{
  drive(0,0);
}
delay(100);

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

}



// ########################## functions #######################

int drive(int l, int r){

  if (l > 255 || r>255){
  Serial.println("Error: Drive input value is greater than 255");
  return 1;
  }
    
  char lDirection, rDirection;
  if (l >= 0){
    lDirection = FORWARD;
  }
  else{
    lDirection = BACKWARD;
  }
  if (r >= 0){
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


void Smooth_Sonar(){
// ############# Smoothing the sonar measurements #########

float F=0, R=0;
int i=0;
while (i<10)
{
  GetSonar();
  F= F+Distance[1];
  R= R+Distance[2];
  i++;
}
Distance[1] = F/10;
Distance[2] = R/10;
Serial.print("Front Distance: ");
Serial.println(Distance[1]);
}


void GetSonar(){

// ############# Checking front distance #########

// Clears the trigPin
digitalWrite(frontTrigPin, LOW);
delayMicroseconds(2);

// Sets the trigPin on HIGH state for 10 micro seconds
digitalWrite(frontTrigPin, HIGH);
delayMicroseconds(10);
digitalWrite(frontTrigPin, LOW);

// Reads the echoPin, returns the sound wave travel time in microseconds
frontDuration= pulseIn(frontEchoPin, HIGH);

// Sound velocity is 0.034 cm/usec
// Calculating the distance
Distance[1]= 0.01*frontDuration*0.034/2; //Distance in meters

// Prints the distance on the Serial Monitor
//Serial.print("Front Distance: ");
//Serial.println(Distance[1]);

// ############# Checking rear distance #########

// Clears the trigPin
digitalWrite(rearTrigPin, LOW);
delayMicroseconds(2);

// Sets the trigPin on HIGH state for 10 micro seconds
digitalWrite(rearTrigPin, HIGH);
delayMicroseconds(10);
digitalWrite(rearTrigPin, LOW);

// Reads the echoPin, returns the sound wave travel time in microseconds
rearDuration = pulseIn(rearEchoPin, HIGH);

// Sound velocity is 0.034 cm/usec
// Calculating the distance
Distance[2]= 0.01*rearDuration*0.034/2; //Distance in meters

// Prints the distance on the Serial Monitor
//Serial.print("Rear Distance: ");
//Serial.println(Distance[2]);

}


int * Cal_initial_speed(){
  
  // ####################### Calibrating forward speed
  Smooth_Sonar();
  float front = Distance[1];
  int i=0;
  // While the car isn't moving => raise the velocity
  while (abs(front - Distance[1]) <= 0.01){
    drive(++i,++i);
    delay(500);
    Smooth_Sonar();
  }
  drive(0,0);
  delay(2000);
  Initial_Speed[1] = i; // Substitute forward initial speed
  Serial.println("Initial forward speed is calibrated");


  // ######################## Calibrating backward speed
  Smooth_Sonar();
  front = Distance[1];
  i=0;
  // While the car isn't moving => raise the velocity
  while (abs(front - Distance[1]) <= 0.01){
    drive(--i,--i);
    delay(500);
    Smooth_Sonar();
  }
  drive(0,0);
  delay(2000);
  Initial_Speed[2] = i; // Substitute backward initial speed
Serial.println("Initial backward speed is calibrated");
    
  Calibrated_Initial_Speed = 1; // UnFlag the need for calibration
  return Initial_Speed;
}

