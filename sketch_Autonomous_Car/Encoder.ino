
//This code is for non Due boards
void timerIsr(){
  Timer1.detachInterrupt();  //stop the timer
  //rotation0 = (counter0 / 20);  // divide by number of holes in Disc [rps]
  //rotation1 = (counter1 / 20);  // divide by number of holes in Disc [rps]
  //display_encoder_reading();
  Serial.println(counter0);
  counter0=0;  //  reset counter to zero
  counter1=0;  //  reset counter to zero
  Timer1.attachInterrupt( timerIsr );  //enable the timer
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// INTERRUPT HANDLERS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
void TC7_Handler()
{
  // We need to get the status to clear it and allow the interrupt to fire again
  TC_GetStatus(TC2, 1);
  detectState0 = !detectState0;
  digitalWrite(led_pin, detectState0);
  //rotation0 = (counter0 / 20);  // divide by number of holes in Disc [rps]
  //rotation1 = (counter1 / 20);  // divide by number of holes in Disc [rps]
  //display_encoder_reading();
  Serial.println(counter0);
  counter0=0;  //  reset counter to zero
  counter1=0;  //  reset counter to zero
  //TC_SetRC(TC2, 1, 656250); // 656000/ 656000 = 1 second
    
}
*/

void docount0(){  // counts from the speed sensor
  counter0++;  // increase +1 the counter value
} 


void docount1(){  // counts from the speed sensor
  counter1++;  // increase +1 the counter value
} 


void display_encoder_reading(){
  Serial.print("Motors Speed (L/R): "); 
  Serial.print(rotation0);  
  Serial.print(",");  
  Serial.println(rotation1);  
  Serial.println(" Rotations per second");   
}


void EncoderCheck(){
  detectState0=digitalRead(encoder1);
  if (detectState0 == HIGH) { //If encoder output is high
    digitalWrite(led_pin, HIGH); //Turn on the status LED
  }
  else {
    digitalWrite(led_pin, LOW); //Turn off the status LED
  }
}
