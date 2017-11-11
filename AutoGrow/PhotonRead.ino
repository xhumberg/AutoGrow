#include "pinDefinitions.h"
#define READ P6_0

void setup() {
 pinMode(READ, INPUT); 
}

void loop(){
  delay(60000);
  digitalRead(READ); // 1 = light on 0 = light off
  if(READ)
  {
    digitalWrite(GROW_LIGHT, HIGH);
  }
  else
  {
    digitalWrite(GROW_LIGHT,LOW);
  }
}

