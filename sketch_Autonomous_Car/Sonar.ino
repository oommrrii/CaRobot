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
