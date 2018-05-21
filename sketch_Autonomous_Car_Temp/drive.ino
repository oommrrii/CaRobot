/*
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
*/
