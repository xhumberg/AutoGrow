#define LED YELLOW_LED

//Constant definitions
#define LEFT 0
#define RIGHT 1


void setup() {
  pinMode(LED, OUTPUT);
  digitalWrite(LED, HIGH);
}


void loop() {
  delay(100);
  digitalWrite(LED, LOW);
  delay(100);
  digitalWrite(LED, HIGH);
}

