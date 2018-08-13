#include <TimerOne.h>

//#include <Wire.h>
#include <Adafruit_MotorShield.h>

// LED pins declaration
const int led_pin = 7;

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
const int frontTrigPin = 4;
const int frontEchoPin = 5;
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
int detectState0=0; // Variable for reading the encoder status
volatile byte counter0=0; // counter for number of encoder triggers
volatile byte counter1=0; // counter for number of encoder triggers
int rotation0=0;
int rotation1=0;

// Flags:
bool start = 0, Calibrated_Initial_Speed = 0;

// Calibration parameters
int *Initial_Speed;



void setup() {
  Serial.begin(9600);           // set up Serial library at 9600 bps
  Serial.println("Setuping the vehicle...");

    
  //Set output LED pins (13 is built-in)
  pinMode(13, OUTPUT);
  pinMode(led_pin, OUTPUT);

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


/*
  // Clock Interrupts definition for Due board

  // turn on the timer clock in the power management controller 
  pmc_set_writeprotect(false);		 // disable write protection for pmc registers
  pmc_enable_periph_clk(ID_TC7);	 // enable peripheral clock TC7

  /* we want wavesel 01 with RC.
    These are the clock frequencies available to the timers /2,/8,/32,/128:
    TIMER_CLOCK1: 84Mhz/2 = 42.000 MHz
    TIMER_CLOCK2: 84Mhz/8 = 10.500 MHz
    TIMER_CLOCK3: 84Mhz/32 = 2.625 MHz
    TIMER_CLOCK4: 84Mhz/128 = 656.250 KHz
    TIMER_CLOCK5: SLCK ( slow clock )
  */
  //TC_Configure(/* clock */TC2,/* channel */1, TC_CMR_WAVE | TC_CMR_WAVSEL_UP_RC | TC_CMR_TCCLKS_TIMER_CLOCK4); 
  /*TC_SetRC(TC2, 1, 164062);
  TC_Start(TC2, 1);

  // enable timer interrupts on the timer
  TC2->TC_CHANNEL[1].TC_IER=TC_IER_CPCS;   // IER = interrupt enable register
  TC2->TC_CHANNEL[1].TC_IDR=~TC_IER_CPCS;  // IDR = interrupt disable register

  // Enable the interrupt in the nested vector interrupt controller
  // TC4_IRQn where 4 is the timer number * timer channels (3) + the channel number (=(1*3)+1) for timer1 channel1
  NVIC_EnableIRQ(TC7_IRQn);
*/

  //Set input pins for encoders
  pinMode(encoder0, INPUT);
  pinMode(encoder1, INPUT);


  // Set encoders interrupts
  //attachInterrupt(encoder0, docount0, RISING);  // increase counter when speed sensor pin goes High
  //attachInterrupt(encoder1, docount1, RISING);  // increase counter when speed sensor pin goes High



  // This code is for non Due boards
  // Setup timed interrupts for encoders
  Timer1.initialize(10000000); // set timer for 1sec
  attachInterrupt(2, docount0, RISING);  // increase counter when speed sensor pin goes High
  attachInterrupt(3, docount1, RISING);  // increase counter when speed sensor pin goes High
  Timer1.attachInterrupt( timerIsr ); // enable the timer


  Serial.println("Waiting for Flag to start algorithm");

}

void loop() {

// ########################### Sonar reading check ###################

//Sonar_reading_check();
//return;

// ############################ Flag to Start algorithm ##############

while (start == 0) {
  Smooth_Sonar();
  if (Distance[0] <= 0.06) {
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
*/
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

//EncoderCheck(); //Checks if the encoder is received by the arduino (should have opposite blinking)
}

