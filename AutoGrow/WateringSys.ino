void setupWateringSys() {
  // put your setup code here, to run once:
 
}

void loopWateringSys() {
  // put your main code here, to run repeatedly: 
  Serial.print("\n\n");
  Serial.println("Starting Soil Monitoring System");
  moistureRead();
  Serial.println("Finished Soil Monitoring System");
  Serial.print("\n\n"); 
  delay(1200000);

}


/*
 * Reads in the moisture level of each plant and waters if needed
 */
void moistureRead() {

  //Holds value read from soil sensors
  int sens1, sens2, sens3, bucket;


  bucket = digitalRead(WATERBUCKET);
  sens1 = analogRead(SENSOR1);
  sens2 = analogRead(SENSOR2);
  sens3 = analogRead(SENSOR3);
  
  Serial.println("Reading Soil Sensors");

  Serial.print("Soil Sensor 1: ");
  Serial.print(sens1); //print the value to serial port
  Serial.print('\n'); //print the value to serial port
  Serial.print("Soil Sensor 2: ");
  Serial.print(sens2); //print the value to serial port
  Serial.print('\n'); //print the value to serial 
  Serial.print("Soil Sensor 3: ");
  Serial.print(sens3); //print the value to serial port
  Serial.print('\n'); //print the value to serial 

//  Waters each plant if the read in moisture value was deemed dry
  if(sens1 > 700 && !bucket)
    water1();
  if(sens1 > 700 && bucket)
    Serial.println("Plant 1 needs watered but reservoir is empty ");
  if(sens2 > 700 && !bucket)
    water2();
  if(sens2 > 700 && bucket)
    Serial.println("Plant 2 needs watered but reservoir is empty ");

  if(sens3 > 700 && !bucket)
    water3();
  if(sens3 > 700 && bucket)
     Serial.println("Plant 3 needs watered but reservoir is empty ");
  if(bucket)
     Serial.println("WARNING RESERVOIR IS EMPTY, PLEASE FILL WITH WATER");

}

/*
 * Handles watering for plant 1
 */
void water1() {
  //Turn on watering for specific plant
  Serial.println("Start Watering Plant 1");
  digitalWrite(SOLENOID0, LOW);
  digitalWrite(SOLENOID1, LOW);
  digitalWrite(SOLENOID2, LOW);
  digitalWrite(PUMP_EN, HIGH);
  delay(1000);
  digitalWrite(PUMP_A, HIGH);
  delay(10000);

  //Turn off watering for specific plant
  digitalWrite(PUMP_A, LOW);
  delay(1000);
  digitalWrite(SOLENOID0, LOW);
  digitalWrite(SOLENOID1, LOW);
  digitalWrite(SOLENOID2, LOW);
  digitalWrite(PUMP_EN, LOW);
  Serial.println("Finished Watering Plant 1");
  
}

/*
 * Handles watering for plant 2
 */
void water2() {
  //Turn on watering for specific plant
  Serial.println("Start Watering Plant 2");
  digitalWrite(SOLENOID0, HIGH);
  digitalWrite(SOLENOID1, LOW);
  digitalWrite(SOLENOID2, LOW);
  digitalWrite(PUMP_EN, HIGH);
  delay(1000);
  digitalWrite(PUMP_A, HIGH);
  delay(10000);

  //Turn off watering for specific plant
  digitalWrite(PUMP_A, LOW);
  delay(1000);
  digitalWrite(SOLENOID0, LOW);
  digitalWrite(SOLENOID1, LOW);
  digitalWrite(SOLENOID2, LOW);
  digitalWrite(PUMP_EN, LOW);
  Serial.println("Finished Watering Plant 2");
}

/*
 * Handles watering for plant 3
 */
void water3() {
  //Turn on watering for specific plant
  Serial.println("Start Watering Plant 3");
  digitalWrite(SOLENOID0, LOW);
  digitalWrite(SOLENOID1, HIGH);
  digitalWrite(SOLENOID2, LOW);
  digitalWrite(PUMP_EN, HIGH);
  delay(1000);
  digitalWrite(PUMP_A, HIGH);
  delay(10000);

  //Turn off watering for specific plant
  digitalWrite(PUMP_A, LOW);
  delay(1000);
  digitalWrite(SOLENOID0, LOW);
  digitalWrite(SOLENOID1, LOW);
  digitalWrite(SOLENOID2, LOW);
  digitalWrite(PUMP_EN, LOW);
  Serial.println("Finished Watering Plant 3");
  
}
