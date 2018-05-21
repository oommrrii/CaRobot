/*
float GetSonar(){

  unsigned long t1, t2, reart1, reart2, pulse_width, rearpulse_width;
  float rearcm, cm, y_prev=0, y,tau=5;

  // Hold the trigger pin high for at least 10 us
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  digitalWrite(rearTRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(rearTRIG_PIN, LOW);
  
  // Wait for pulse on echo pin
  while ( digitalRead(ECHO_PIN) == 0 );

  // Measure how long the echo pin was held high (pulse width)
  // Note: the micros() counter will overflow after ~70 min
  t1 = micros();
  while ( digitalRead(ECHO_PIN) == 1);
  t2 = micros();
  pulse_width = t2 - t1;

   // Wait for pulse on echo pin
  while ( digitalRead(rearECHO_PIN) == 0 );

  // Measure how long the echo pin was held high (pulse width)
  // Note: the micros() counter will overflow after ~70 min
  reart1 = micros();
  while ( digitalRead(rearECHO_PIN) == 1);
  reart2 = micros();
  rearpulse_width = reart2 - reart1;

  // Calculate distance in centimeters. The constants
  // are found in the datasheet, and calculated from the assumed speed 
  //of sound in air at sea level (~340 m/s).
  cm = pulse_width / 58.0;
  rearcm = rearpulse_width / 58.0;
  y = cm-(tau*y_prev);  // First order filtering of the results
  
// Print out results
//  if ( pulse_width > MAX_DIST ) {
//    Serial.println("Out of range");
//  } else {
//   Serial.println(cm);
//    Serial.println(y);
//  }

  y_prev = y;
  // Wait at least 60ms before next measurement
  delay(100);
  if (rearcm<5){
    return(rearcm);
  }
  else{
  return(y/100);
  }
}
*/
