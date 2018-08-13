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
