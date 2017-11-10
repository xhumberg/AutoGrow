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

//ease of use definitions
#define LEFT 0
#define RIGHT 1
#define UP 0
#define DOWN 1

//Constants (so there aren't any magic numbers)
#define H_IDLE_OFFSET 100
#define RESET_DOWN_STEPS 3000


void setup() {
  initializePins();
  
  digitalWrite(H_ENABLE, HIGH);
  initialize();
  digitalWrite(GROW_LIGHT, HIGH);
}

void loop() {
  delay(3600000);
  digitalWrite(GROW_LIGHT, LOW);
  scan();
  digitalWrite(GROW_LIGHT, HIGH);  
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
  pinMode(BLUE_LED, OUTPUT);
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

