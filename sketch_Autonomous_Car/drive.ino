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
