#include "pinDefinitions.h"


void setupPhotonRead() {
}

void loopPhotonRead(){
  delay(60000);
   // 1 = light on 0 = light off
  if(digitalRead(READ))
  {
    digitalWrite(GROW_LIGHT, HIGH);
    Serial.println("LIGHT ON"); //print the value to serial port
  }
  else if (!digitalRead(READ))
  {
    digitalWrite(GROW_LIGHT, LOW);
    Serial.println("LIGHT OFF"); //print the value to serial port
  }
}

