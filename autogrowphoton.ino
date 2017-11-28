//For UART communication
const size_t READ_BUF_SIZE = 64;
char readBuf[READ_BUF_SIZE];
size_t readBufOffset = 0;

int water = 0;

//Declaration of function
void sendTime();
void process(int);

void setup() {

  Particle.variable("water", &water, INT);
  
  //Begin UART communication.  Serial1 for MSP432
  Serial1.begin(9600);
  //Serial.begin(9600);

}

void loop() {

  //Set up correct time zone 
  Time.zone(-7);

  //Send hour to msp every second
  delay(10000);
  
  //Send time to the MSP432
  sendTime();
  
  //Receive from MSP432 serial messages on RX line
  while(Serial1.available())
  {
      if(readBufOffset < READ_BUF_SIZE)
      {
          //Read in message
          char c = Serial1.read();
          if(c != '\n')
          {
              readBuf[readBufOffset++] = c;
          }
          
          //Reach end of line
          else
          {
              readBuf[readBufOffset] = 0;
              String buf2string(readBuf);
              int val = atoi(buf2string);
            //   Serial.print("Input val from buffer: ");
            //   Serial.print(val);
            //   Serial.println();
              //int val = atoi(string2buf);
              //Serial.println(val);
              process(val);
              
              //Reset buffer position
              readBufOffset = 0;
          }
      }
      //Reset buffer position
      else
      {
          readBufOffset = 0;
      }
  }
}

/*
 * Read in input from buffer and process the line
 */
void process(int line)
{
    // Serial.print("Entering process with given line: ");
    // Serial.print(line);
    // Serial.println();
    //Message received was plant watered
    if(line == 11) //Plant 1
    {
        //Serial.println(line);
        water++;
        //Serial.println("Plant watererd");
        Particle.publish("plantWatered1");
    }
    if(line == 12) //Plant 2
    {
        //Serial.println(line);
        water++;
        //Serial.println("Plant watererd");
        Particle.publish("plantWatered2");
    }
    if(line == 13) //Plant 3
    {
        //Serial.println(line);
        water++;
        //Serial.println("Plant watererd");
        Particle.publish("plantWatered3");
    }
    
    //Empty watering bucket cases
    if(line == 20) //Empty bucket
    {
        Particle.publish("emptyBucket");
    }
    if(line == 21)  //Following statements for empty while watering a plant
    {
        Particle.publish("emptyBucket1");
    }
    if(line == 22)
    {
        Particle.publish("emptyBucket1");
    }
    if(line == 23)
    {
        Particle.publish("emptyBucket1");
    }
    
}

/*
 * This function gets the time from thew Wifi that is connected to and sends it in 24 hour format
 * with the hours followed by the minutes.  If the hour or minute < 10, then a 0 is inserted in front
 * for consistency
 */
void sendTime()
{
  //Insert a "0" in front of hour if hour is less than 10 else send hour (24 hour format)
  if(Time.hour() < 10)
  {
    Serial1.print("0");
    Serial1.print(Time.hour());
  }
  else
  {
    Serial1.print(Time.hour());
  }
  if(Time.minute() < 10)
  {
    Serial1.print("0");
    Serial1.print(Time.minute());
  }
  else
  {
    Serial1.print(Time.minute());
  }
  Serial1.println();
}
