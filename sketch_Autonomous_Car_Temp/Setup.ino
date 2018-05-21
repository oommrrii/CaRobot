/*
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


  //Set input pins
  pinMode(encoderIn0, INPUT);
  pinMode(encoderIn1, INPUT);
  pinMode(encoderIn2, INPUT);
  pinMode(encoderIn3, INPUT);

 
  
  pinMode(LED_BUILTIN, OUTPUT); //Set pin 13 as output LED
}
*/
