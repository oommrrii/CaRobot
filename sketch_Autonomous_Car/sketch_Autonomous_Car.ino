#include <TimerOne.h>
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
float Distance[2] = {0.1,0.1}; //0 for front distance; 1 for rear distance

// Control variables
float Front_Ref, Front_Error, Front_Error_Prev=0, Front_Error_Diferential_Prev=0, Front_Error_Diferential, Front_Error_Integral, u, Kp=100, Kd=200, Ki=100;

// Encoder variables
const int encoder0 = 2; // input pin for the interrupter 
const int encoder1 = 3; // input pin for the interrupter 
const int encoder2 = 4; // input pin for the interrupter 
const int encoder3 = 5; // input pin for the interrupter 
int detectState3=0; // Variable for reading the encoder status
unsigned int counter0=0; // counter for number of encoder triggers
unsigned int counter1=0; // counter for number of encoder triggers
float rotation0=0;
float rotation1=0;

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


  //Set input pins for encoders
  pinMode(encoder3, INPUT);
  
  //Set pin 13 as output LED
  pinMode(13, OUTPUT);

  // Setup timed interrupts for encoders
  Timer1.initialize(1000000); // set timer for 1sec
  attachInterrupt(0, docount0, RISING);  // increase counter when speed sensor pin goes High
  attachInterrupt(1, docount1, RISING);  // increase counter when speed sensor pin goes High
  Timer1.attachInterrupt( timerIsr ); // enable the timer

  Serial.println("Waiting for Flag to start algorithm");

}

// ########################## functions #######################

int drive(int l, int r){
    
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

  if (abs(l) > 255 || abs(r) > 255){
  Serial.println("Error: Drive input value is greater than 255");
  //return 1;
    if (abs(l) > 255)
    l=255;
    if (abs(r) > 255)
    r=255;
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


float * GetSonar(){

// defines  Sonar variables
long frontDuration;
long rearDuration;
static float Distance_meassured[2]; //0 for front meassured distance; 1 for rear meassured distance

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
Distance_meassured[0]= 0.01*frontDuration*0.034/2; //Distance in meters

// Prints the distance on the Serial Monitor
//Serial.print("Front Distance: ");
//Serial.println(Distance_meassured[0]);

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
Distance_meassured[1]= 0.01*rearDuration*0.034/2; //Distance in meters

// Prints the distance on the Serial Monitor
//Serial.print("Rear Distance: ");
//Serial.println(Distance_meassured[1]);

// ############# Wait 60 miliseconds between samplings ##############
delay(60);

return Distance_meassured;

}


void Smooth_Sonar(){
// ############# Filtering the sonar measurements #########

// Declaring array variable for sonar meassurement function
float *Distance_meassured = GetSonar();

// Low pass filter coeficient. (alpha = dt/tau). dt = Sonar sampling rate is 40 Hz. tau = above frequencies will be filtered.
float alpha=0.5;

Distance[0]= Distance[0] + alpha*(*Distance_meassured - Distance[0]);
Distance[1]= Distance[1] + alpha*(*(Distance_meassured+1) - Distance[1]);

/*
 * Ploting 4 signals to the plotter. To check the filter's performance.
Serial.print(*Distance_meassured);
Serial.print(',');
Serial.print(Distance[0]);
Serial.print(',');
Serial.print(*(Distance_meassured+1));
Serial.print(',');
Serial.println(Distance[1]);
*/

/*
Serial.println("Front , Rear Distances: ");
Serial.print(Distance[0]);
Serial.print(" , ");
Serial.println(Distance[1]);
*/
}


void Sonar_reading_check(){
  Smooth_Sonar();
  Serial.println("Front , Rear Distances: ");
  Serial.print(Distance[0]);
  Serial.print(",");
  Serial.println(Distance[1]);
}


int * Cal_initial_speed(){
  
  // ####################### Calibrating forward speed
  
  int i=0, j;
  for (j=0; j<50; j++){
  Smooth_Sonar();
  }
  float front = Distance[0];
  // While the car isn't moving => raise the velocity
  while (abs(front - Distance[0]) <= 0.02){
    drive(++i,++i);
    delay(500);
    Smooth_Sonar();
  }
  drive(0,0);
  delay(2000);
  Initial_Speed[0] = i; // Substitute forward initial speed
  Serial.println("Initial forward speed is calibrated");
  Serial.println("For reference, the last 2 Front Distance readings were: ");
  Serial.print(Distance[0]);
  Serial.print(" / ");
  Serial.println(front);


  // ######################## Calibrating backward speed
  
  for (j=0; j<50; j++){
  Smooth_Sonar();
  }
  front = Distance[0];
  i=0;
  // While the car isn't moving => raise the velocity
  while (abs(front - Distance[0]) <= 0.02){
    drive(--i,--i);
    delay(500);
    Smooth_Sonar();
  }
  drive(0,0);
  delay(2000);
  Initial_Speed[1] = i; // Substitute backward initial speed
  Serial.println("Initial backward speed is calibrated");
  Serial.println("For reference, the last 2 Front Distance readings were: ");
  Serial.print(Distance[0]);
  Serial.print(" / ");
  Serial.println(front);
    
  Calibrated_Initial_Speed = 1; // UnFlag the need for calibration
  return Initial_Speed;
}


void timerIsr(){
  Timer1.detachInterrupt();  //stop the timer
  rotation0 = ((float)counter0 / (float)20);  // divide by number of holes in Disc [rps]
  rotation1 = ((float)counter1 / (float)20);  // divide by number of holes in Disc [rps]
  display_encoder_reading();
  counter0=0;  //  reset counter to zero
  counter1=0;  //  reset counter to zero
  Timer1.attachInterrupt( timerIsr );  //enable the timer
}


void display_encoder_reading(){
  Serial.print("Motors Speed (L/R): "); 
  Serial.print(rotation0);  
  Serial.print("/");  
  Serial.print(rotation1);  
  Serial.println(" Rotations per second");   
}


void docount0(){  // counts from the speed sensor
  counter0++;  // increase +1 the counter value
} 


void docount1(){  // counts from the speed sensor
  counter1++;  // increase +1 the counter value
} 


void loop() {

// ########################### Sonar reading check ###################
/*
Sonar_reading_check();
return;
*/
// ############################ Flag to Start algorithm ##############

while (start == 0) {
  Smooth_Sonar();
  if (Distance[0] <= 0.04) {
    start =1;
    Serial.println("Flag received - Starting algorithm");
  }
}

// ########################### Calibration ###########################
/*
// Calibrating the initiating wheels speed
if (Calibrated_Initial_Speed == 0){
  delay(2000);
  Serial.println("For reference, Front/Rear Distances are: ");
  Serial.print(Distance[0]);
  Serial.print(" / ");
  Serial.println(Distance[1]);
  Serial.println("Starting initial speed calibration");
  Initial_Speed = Cal_initial_speed();
  Serial.print("Calibration is done.\nInitial forward/backward speed is: ");
  Serial.print(Initial_Speed[0]);
  Serial.print(" / ");
  Serial.println(Initial_Speed[1]);
}
*/
// ########################### Algorithm #############################
/*
Smooth_Sonar(); // obtain distance values
Front_Ref= 0.70; 
Front_Error= Front_Ref-Distance[0];
Front_Error_Diferential= (Front_Error + Front_Error_Prev)/2;
//Front_Error_Diferential= Front_Error_Diferential_Prev + 2* (Front_Error - Front_Error_Prev);
Front_Error_Integral= Front_Error_Integral + Front_Error/10; // Estimated sampling rate is 500 Hz
u= -1 * (Kp*Front_Error + Kd*Front_Error_Diferential + Ki*Front_Error_Integral);

//Serial.print(Front_Ref);
//Serial.print(",");
//Serial.print(Front_Error);
//Serial.print(",");
//Serial.print(Front_Error_Diferential);
//Serial.print(",");
//Serial.println(Front_Error_Integral);
//Serial.print(",");
//Serial.println(u);

// Save to previous step
Front_Error_Prev = Front_Error;
Front_Error_Diferential_Prev = Front_Error_Diferential;

// Perform output w/o calibration
drive(u,u);

/*
// Apply calibration and Perform output
if (u > 0){
  drive(u + Initial_Speed[0],u + Initial_Speed[0]);
}
else if (u < 0){
  drive(u + Initial_Speed[1],u + Initial_Speed[1]);
}
else{
  drive(0,0);
}
*/
delay(100);

// ############################ Encoder ########################

detectState3=digitalRead(encoder3);


   if (detectState3 == HIGH) { //If encoder output is high
      digitalWrite(13, HIGH); //Turn on the status LED
   }
   else {
      digitalWrite(13, LOW); //Turn off the status LED
   }

}

