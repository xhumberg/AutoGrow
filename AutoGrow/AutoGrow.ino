
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
#define READ P6_0
#define UP_BUTTON P4_6
#define DOWN_BUTTON P6_4
#define INIT_BUTTON P6_5
#define WATER_BUTTON P4_3
#define WATER_SWITCH_ONE P5_0
#define WATER_SWITCH_ZERO P5_2


//For watering system
#define SENSOR1 P6_1
#define SENSOR2 P4_0
#define SENSOR3 P4_2
#define SOLENOID0 P4_4
#define SOLENOID1 P4_5
#define SOLENOID2 P4_7
#define PUMP_EN P5_4
#define PUMP_A P5_5
#define WATERBUCKET P4_1

//ease of use definitions
#define LEFT 0
#define RIGHT 1
#define UP 0
#define DOWN 1

//Constants (so there aren't any magic numbers)
#define H_IDLE_OFFSET 100
#define RESET_DOWN_STEPS 500

int TIME, HOUR, MIN;

int initialCase = 1;
int scanCount = 0;
int called = 1;
int scanning = 0;

//For UART communcation from Photon
const size_t READ_BUF_SIZE = 64;
char readBuf[READ_BUF_SIZE];

//Keep track of hours
int prevh;
bool newh = true;

size_t readBufOffset = 0;

void setup() {

  Serial.begin(9600);      // open the serial port at 9600 bps:
  Serial1.begin(9600);
  digitalWrite(PUMP_A, LOW);
  initializePins();
  digitalWrite(H_ENABLE, HIGH);
  initialize();

  //Check the hour for whether or not to turn on the light
  if (HOUR >= 8 && HOUR < 20 && digitalRead(V_LIMIT_UP))
  {
    digitalWrite(GROW_LIGHT, HIGH);
  }
}

void loop() {

  //Only turn the light on between 8 AM and 8 PM
  while (scanning == 1) {delay(100);} //Lock while scanning
  if (HOUR >= 8 && HOUR < 20 && digitalRead(V_LIMIT_UP))
  {
    digitalWrite(GROW_LIGHT, HIGH);
  }

  //Sets a new hour for scanning
  int newHOUR = HOUR;
  if (newh)
  {
    prevh = newHOUR;
    newh = false;
  }

  //Scan once every hour and check the moisture levels
  if ( newh == false && newHOUR != prevh)
  {
    digitalWrite(GROW_LIGHT, LOW);

    // Complete 6 normal scans
    if(scanCount < 6){
    scan();
    scanCount++;
    }

    //6 scans have not raised the light, so lower the light to make sure we didn't end up with a fluke incident.
    if(scanCount >=6){
      called = 0;
      initialCase = 1;
      initialize();
    }
    
    moistureRead();

    while (scanning == 1) {delay(100);} //Lock while scanning
    
    //Turn the light back on?
    if (HOUR >= 8 && HOUR < 20 && digitalRead(V_LIMIT_UP))
    {
      digitalWrite(GROW_LIGHT, HIGH);
    }
    prevh = newHOUR;
    newh = true;
  }

  detectButtons();
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
  pinMode(WATERBUCKET, INPUT);
  pinMode(READ, INPUT);
  pinMode(UP_BUTTON, INPUT_PULLUP);
  pinMode(DOWN_BUTTON, INPUT_PULLUP);
  pinMode(INIT_BUTTON, INPUT_PULLUP);
  pinMode(WATER_BUTTON, INPUT_PULLUP);
  pinMode(WATER_SWITCH_ONE, INPUT_PULLUP);
  pinMode(WATER_SWITCH_ZERO, INPUT_PULLUP);

  // pinMode(BLUE_LED, OUTPUT);
}

/*
   This function brings the light to the top, and has the laser scan its way downwards until it detects a plant.
   Once a plant is detected, it's up to the scan function to raise the light to the proper height.
*/
void initialize() {
  digitalWrite(LASER, HIGH);

//Ensure Initialize still does something when we reach the top
  notTop();

  toLLimit();

  int scanDirection = RIGHT;
  int scanLimit = H_LIMIT_RIGHT;

//  //Do this only when FIRST initializing. Scan immediately, go up if needed.
//  if(called){
//    scan();
//  }

  while (true) {
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
  scanning = 1;
  digitalWrite(LASER, HIGH);
  digitalWrite(GROW_LIGHT, LOW);
  toLLimit();
  delay(1000); 

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
          resetH();
          digitalWrite(LASER, LOW);
          scanning = 0;
          return;
        }
      }
    }
    
    if (!interrupted) {
      resetH();
      digitalWrite(LASER, LOW);
      scanning = 0;
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
  digitalWrite(H_ENABLE, LOW);
}

void vstep(int direction) {
  digitalWrite(V_DIRECTION, direction);
  digitalWrite(V_ENABLE, LOW );
  digitalWrite(V_STEP, HIGH);
  delayMicroseconds(500);
  digitalWrite(V_STEP, LOW);
  delayMicroseconds(500);
  digitalWrite(V_ENABLE, HIGH);
}

boolean vstep(int direction, int amount) {
  initialCase = 0;
  
  
  for (int i = 0; i < amount; i++) {
    if (direction == UP)
      if (!digitalRead(V_LIMIT_UP))
        return false;
    if (direction == DOWN)
      if (!digitalRead(V_LIMIT_DOWN))
        return false;
    vstep(direction);
  }

  if(direction == UP){
    scanCount = 0;
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

void test(){
  for(int testI = 0; testI < 7; testI++ ){
    Serial.print("Scan Test: ");
    Serial.print(testI);
    Serial.print("\n");
    scan();
    scanCount++;
    Serial.print("Scan Count: ");
    Serial.print(scanCount);
    Serial.print("\n");
  }
}

void notTop() {
  if (!digitalRead(V_LIMIT_UP))
    vstep(DOWN, 1000);
}

void detectButtons() {
  if (!digitalRead(UP_BUTTON)) {
    digitalWrite(GROW_LIGHT, LOW);
    while (!digitalRead(UP_BUTTON))
      vstep(UP, 100);
    scan();
  }
  else if (!digitalRead(DOWN_BUTTON)) {
    digitalWrite(GROW_LIGHT, LOW);
    while (!digitalRead(DOWN_BUTTON))
      vstep(DOWN, 100);
    scan();
  }
  if (!digitalRead(INIT_BUTTON)) {
    digitalWrite(GROW_LIGHT, LOW);
    initialize();
  }
  if (!digitalRead(WATER_BUTTON)) {
    int thisOne = (!digitalRead(WATER_SWITCH_ZERO)) + 2*(!digitalRead(WATER_SWITCH_ONE));
    switch (thisOne) {
      case 0:
        water1();
        break;
      case 1:
        water2();
        break;
      case 2:
        water3();
        break;
      case 3:
        water4();
        break;
    }
  }
  delay(100);
}

