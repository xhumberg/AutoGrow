#include <DS3231.h>

#include "pinDefinitions.h"


//Pinout definition
#define H_ENABLE P5_1
#define H_DIRECTION P3_5
#define H_STEP P3_7
#define V_ENABLE P6_6
#define V_DIRECTION P6_7
#define V_STEP P2_3
#define H_LIMIT_LEFT P5_6  //Red
#define H_LIMIT_RIGHT P2_4 //Brown
#define V_LIMIT_UP P2_6    //
#define V_LIMIT_DOWN P2_7
#define LASER P2_5
#define LASER_SENSOR P3_0
#define GROW_LIGHT P3_6

//For watering system
#define SENSOR1 P6_1
#define SENSOR2 P4_0
#define SENSOR3 P4_2
#define SOLENOID0 P4_4
#define SOLENOID1 P4_5
#define SOLENOID2 P4_7
#define PUMP_EN P5_4
#define PUMP_A P5_5

//ease of use definitions
#define LEFT 0
#define RIGHT 1
#define UP 0
#define DOWN 1

//Constants (so there aren't any magic numbers)
#define H_IDLE_OFFSET 100
#define RESET_DOWN_STEPS 3000

//Setup rtc
DS3231 rtc(SDA, SCL);
Time t, prevt;

void setup() {
  initializePins();
  rtc_setup();
  digitalWrite(H_ENABLE, HIGH);
  initialize();
  digitalWrite(GROW_LIGHT, HIGH);
}

void loop() {
  //delay(3600000);
  rtc_get();
  delay(1000);

}


void initializePins() {
  pinMode(H_ENABLE, OUTPUT);
  pinMode(H_STEP, OUTPUT);
  pinMode(H_DIRECTION, OUTPUT);
  pinMode(V_ENABLE, OUTPUT);
  pinMode(V_STEP, OUTPUT);
  pinMode(V_DIRECTION, OUTPUT);
  pinMode(H_LIMIT_LEFT, INPUT_PULLUP);
  pinMode(H_LIMIT_RIGHT, INPUT_PULLUP);
  pinMode(V_LIMIT_UP, INPUT_PULLUP);
  pinMode(V_LIMIT_DOWN, INPUT_PULLUP);
  pinMode(LASER_SENSOR, INPUT);
  pinMode(GROW_LIGHT, OUTPUT);
  pinMode(LASER, OUTPUT);
  pinMode(SENSOR1, INPUT);
  pinMode(SENSOR2, INPUT);
  pinMode(SENSOR3, INPUT);
  pinMode(SOLENOID0, OUTPUT);
  pinMode(SOLENOID0, OUTPUT);
  pinMode(SOLENOID0, OUTPUT);
  pinMode(SOLENOID1, OUTPUT);
  pinMode(SOLENOID2, OUTPUT);
  pinMode(PUMP_EN, OUTPUT);
  pinMode(PUMP_A, OUTPUT);

  
 // pinMode(BLUE_LED, OUTPUT);
}

/*
 * This function brings the light to the top, and has the laser scan its way downwards until it detects a plant.
 * Once a plant is detected, it's up to the scan function to raise the light to the proper height.
 */
void initialize() {
  digitalWrite(LASER, HIGH);
  while(digitalRead(V_LIMIT_UP))
    vstep(UP);
  toLLimit();

  int scanDirection = RIGHT;
  int scanLimit = H_LIMIT_RIGHT;
  
  while(true) {
    //If we can't step all the way down, return;
    if (!vstep(DOWN, RESET_DOWN_STEPS)) {
      scan();
      return;
    }

    //Scan
    while (digitalRead(scanLimit)) {
        hstep(scanDirection);
        if (!digitalRead(LASER_SENSOR)) {
          scan();
          return;
        }
    }

    //If we reach this point, we didn't break contact, so continue downwards and scan the other direction
    if (scanDirection == RIGHT) {
      scanDirection = LEFT;
      scanLimit = H_LIMIT_LEFT;
    } else {
      scanDirection = RIGHT;
      scanLimit = H_LIMIT_RIGHT;
    }
  }
}

void scan() {
  digitalWrite(LASER, HIGH);
  toLLimit();

  int scanDirection = RIGHT;
  int scanLimit = H_LIMIT_RIGHT;
  boolean interrupted = false;

  while (true) {
    interrupted = false;
    while (digitalRead(scanLimit)) {
      hstep(scanDirection);
      if (!digitalRead(LASER_SENSOR)) {
        interrupted = true;
        if (!vstep(UP, 150)) {
          return;
          digitalWrite(LASER, LOW);
        }
      }
    }
    if (!interrupted) {
      resetH();
      digitalWrite(LASER, LOW);
      return;
    }
    //If we reach this point, we were interrupted, so we should scan the other way
    if (scanDirection == RIGHT) {
      scanDirection = LEFT;
      scanLimit = H_LIMIT_LEFT;
    } else {
      scanDirection = RIGHT;
      scanLimit = H_LIMIT_RIGHT;
    }
  }
  digitalWrite(LASER, LOW);
}

void hstep(int direction) {
    digitalWrite(H_DIRECTION, direction);
    digitalWrite(H_ENABLE, LOW);
    digitalWrite(H_STEP, HIGH);
    delayMicroseconds(500);
    digitalWrite(H_STEP, LOW);
    delayMicroseconds(500);
    digitalWrite(H_ENABLE, HIGH);
}

void vstep(int direction) {
    digitalWrite(V_DIRECTION, direction);
    digitalWrite(V_ENABLE, LOW);
    digitalWrite(V_STEP, HIGH);
    delayMicroseconds(500);
    digitalWrite(V_STEP, LOW);
    delayMicroseconds(500);
    digitalWrite(V_ENABLE, HIGH);
}

boolean vstep(int direction, int amount) {
  for (int i = 0; i < amount; i++) {
    if (direction == UP)
      if (!digitalRead(V_LIMIT_UP))
        return false;
    if (direction == DOWN)
      if (!digitalRead(V_LIMIT_DOWN))
        return false;
    vstep(direction);
  }
  return true;
}

void resetH() {
  toLLimit();
  delay(100);
  for (int i = 0; i < H_IDLE_OFFSET; i++)
    hstep(RIGHT);
}

void toLLimit() {
  while (digitalRead(H_LIMIT_LEFT))
    hstep(LEFT);
}

boolean laserCheck() {
  return digitalRead(LASER_SENSOR);
}

/*
 * Setup the clock and intitialize the time if needed.  If time is setup, comment lines out and upload again 
 */
void rtc_setup() {
  rtc.begin();

  //For first time setup of RTC.  Comment after uploading.
  //rtc.setDOW(FRIDAY);     //Set day of week
  //rtc.setTime(11, 43, 20);     //Set time in hr, min, sec (24 hour)
  //rtc.setDate(11, 10, 2017); //Set date in format m/dd/yyyy
 }

/*
 * This function keeps the time and switches the light on and off depending on the time as well as 
 * watering based on time
 */
void rtc_get() {
  //Get the time from the rtc
  t = rtc.getTime();
  int newHour = 1;

  //Sets previous hour as base for scanning 
  if(newHour = 1);
  {
    prevt = t;
    newHour = 0;
  }

  if(t.hour = prevt.hour + 1)
  {
    digitalWrite(GROW_LIGHT, LOW);
    scan();
    rtc_get();
    digitalWrite(GROW_LIGHT, HIGH);
    newHour = 1;  
  }
  
  //Get day of week, date, and time and print to terminal
  //Serial.print(rtc.getDOWStr());
  //Serial.print(" ");
  //Serial.print(rtc.getDateStr());
  //Serial.print(" ");
  //Serial.print(rtc.getTimeStr());
  //Serial.println();
  //delay(1000);

  //Time to turn on light
  if(t.hour == 8 && !digitalRead(GROW_LIGHT))
  {
    digitalWrite(GROW_LIGHT, HIGH);
    moistureRead();
  }
    
  //Turn off light
  if(t.hour == 16 && digitalRead(GROW_LIGHT))
  {
    digitalWrite(GROW_LIGHT, LOW);
    moistureRead();
  } 
  

}

/*
 * Reads in the moisture level of each plant and waters if needed
 */
void moistureRead() {

  //Holds value read from soil sensors
  int sens1, sens2, sens3;

  sens1 = analogRead(SENSOR1);
  sens2 = analogRead(SENSOR2);
  sens3 = analogRead(SENSOR3);

  //Waters each plant if the read in moisture value was deemed dry
  if(sens1 > 430)
    water1();

  if(sens2 > 430)
    water2();

  if(sens3 > 430)
    water3();
}

/*
 * Handles watering for plant 1
 */
void water1() {
  //Turn on watering for specific plant
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
  
}

/*
 * Handles watering for plant 2
 */
void water2() {
  //Turn on watering for specific plant
  digitalWrite(SOLENOID0, LOW);
  digitalWrite(SOLENOID1, LOW);
  digitalWrite(SOLENOID2, HIGH);
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
  
}

/*
 * Handles watering for plant 3
 */
void water3() {
  //Turn on watering for specific plant
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
  
}
