#include "pinDefinitions.h"



void setupPhotonRead() {
}

void loopPhotonRead() {
  delay(1000);
 
  
  // 1 = light on 0 = light off
  while (scanning == 1) {delay(100);} //Lock while scanning
  if (HOUR >= 8 && HOUR < 20 && digitalRead(V_LIMIT_UP))
  {
    digitalWrite(GROW_LIGHT, HIGH);
    //  Serial.println("LIGHT ON"); //print the value to serial port
  }
  else
  {
    digitalWrite(GROW_LIGHT, LOW);
    //Serial.println("LIGHT OFF"); //print the value to serial port
  }

  //For reading in the time coming from Photon
  while (Serial1.available())
  {
    if (readBufOffset < READ_BUF_SIZE)
    {
      char c = Serial1.read();
      if (c != '\n')
      {
        readBuf[readBufOffset++] = c;
      }

      //Reach the end of line
      else
      {
        readBuf[readBufOffset] = 0;
        String buf2string(readBuf);
        //Serial.println(buf2string); // **** UNCOMMENT THIS LINE TO PRINT CURRENT TIME IN SERIAL. ****
        String HOURT(readBuf[0]);
        String HOURO(readBuf[1]);
        String SHOUR = HOURT + HOURO;
        HOUR = SHOUR.toInt();

        String MINT(readBuf[2]);
        String MINO(readBuf[3]);
        String SMIN = MINT + MINO;
        MIN = SMIN.toInt();
        //Serial.print("HOUR: ");
        //Serial.print(HOUR);
        //Serial.print("\nMIN: ");
        //Serial.print(MIN);
        //Serial.print("\n");

        readBufOffset = 0;
      }
    }
    else
    {
      readBufOffset = 0;
    }
  }
}

