/*
void loop() {

// ############################ Flag to Start algorithm ##############
switch (start)
{
  case 0:
 {
  y = GetSonar();
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
float y = GetSonar(); // obtain distance to obstacle
Front_Ref= 0.6; 
Front_Error= Front_Ref-y;
Front_Error_Diferential = (Front_Error - Front_Error_Prev)/0.025;
Front_Error_Integral = Front_Error_Integral + (Front_Error*0.025); // Estimated sampling rate is 40 Hz
u=  -1*(Kp*Front_Error+ Ki*Front_Error_Integral+ Kd*Front_Error_Diferential);

Serial.print(y);
Serial.print(",");
//Serial.print(Front_Ref);
//Serial.print(",");
Serial.print(u/255);
Serial.print(",");
Serial.println(Front_Error);

// Save to previous step
Front_Error_Prev = Front_Error;
/*
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

drive(u,u);

// ############################ Encoder ########################

detectState0=digitalRead(encoderIn0);
   if (detectState0 == HIGH) { //If encoder output is high
      digitalWrite(LED_BUILTIN, HIGH); //Turn on the status LED
   }
   else {
      digitalWrite(LED_BUILTIN, LOW); //Turn off the status LED
   }

break;
}
}
}
*/
