#include <IRremote.h>

int rightFwdPin = 11;
int rightBkwPin = 10;
int rightSpeedPin = 5;
int leftFwdPin = 9;
int leftBkwPin = 8;
int leftSpeedPin = 6;

int forwardSpeed = 0;

#define STOP_BUTTON  0x3E108
#define UP_BUTTON    0x1E108
#define DOWN_BUTTON  0x9E108
#define RIGHT_BUTTON 0xDE108
#define LEFT_BUTTON  0x5E108

IRrecv irrecv(2);

typedef enum { LEFT, RIGHT, ALL} driver_type;

void setup()
{
  pinMode(leftFwdPin, OUTPUT);
  pinMode(rightFwdPin, OUTPUT);
  pinMode(leftFwdPin, OUTPUT);
  pinMode(rightFwdPin, OUTPUT);
  pinMode(leftSpeedPin, OUTPUT);
  pinMode(rightSpeedPin, OUTPUT);

  forwardSpeed = 0;

  Serial.begin(9600);
  // In case the interrupt driver crashes on setup, give a clue
  // to the user what's going on.
  Serial.println("Enabling IRin");
  irrecv.enableIRIn(); // Start the receiver
  Serial.println("Enabled IRin");
}

void forward(driver_type drive=ALL) {
  switch(drive) {
    case LEFT:
      digitalWrite(leftBkwPin, LOW);
      digitalWrite(leftFwdPin, HIGH);
      break;
    case RIGHT:
      digitalWrite(rightBkwPin, LOW);
      digitalWrite(rightFwdPin, HIGH);
      break;
    case ALL:
      digitalWrite(leftBkwPin, LOW);
      digitalWrite(rightBkwPin, LOW);
      digitalWrite(leftFwdPin, HIGH);
      digitalWrite(rightFwdPin, HIGH);
  }
}

void backward(driver_type drive=ALL) {
  switch(drive) {
    case LEFT:
      digitalWrite(leftBkwPin, HIGH);
      digitalWrite(leftFwdPin, LOW);
      break;
    case RIGHT:
      digitalWrite(rightBkwPin, HIGH);
      digitalWrite(rightFwdPin, LOW);
      break;
    case ALL:
      digitalWrite(leftBkwPin, HIGH);
      digitalWrite(rightBkwPin, HIGH);
      digitalWrite(leftFwdPin, LOW);
      digitalWrite(rightFwdPin, LOW);
  }
}

void stop(driver_type driver=ALL) {
  switch(driver) {
    case LEFT:
      analogWrite(leftFwdPin, 0);
      analogWrite(leftBkwPin, 0);
      analogWrite(leftSpeedPin, 0);
      break;
    case RIGHT:
      analogWrite(rightFwdPin, 0);
      analogWrite(rightBkwPin, 0);
      analogWrite(rightSpeedPin, 0);
      break;
    case ALL:
      analogWrite(leftFwdPin, 0);
      analogWrite(rightFwdPin, 0);
      analogWrite(leftBkwPin, 0);
      analogWrite(rightBkwPin, 0);
      analogWrite(leftSpeedPin, 0);
      analogWrite(rightSpeedPin, 0);

      forwardSpeed = 0;
  }
}

void change_speed(int speed_step) {
  forwardSpeed = forwardSpeed + speed_step;
  if (forwardSpeed > 255) {
    forwardSpeed = 255;
  }
  if (forwardSpeed < -255) {
    forwardSpeed = -255;
  }
  Serial.print("fwdSpeed=");
  Serial.println(forwardSpeed);
  if (forwardSpeed > 0) {
    forward();  
  }
  else {
    backward();
  }
  analogWrite(leftSpeedPin, abs(forwardSpeed));
  analogWrite(rightSpeedPin, abs(forwardSpeed));
}

void turnRight() {
  stop(RIGHT);
  delay(500);
  change_speed(0);
}

void turnLeft() {
  stop(LEFT);
  delay(500);
  change_speed(0);
}

void processControls(decode_results *results) {
  Serial.println("In processControls");
  switch (results->value) {
    case UP_BUTTON:
      Serial.println("UP!");
      change_speed(25);
      break;
    case DOWN_BUTTON:
      Serial.println("DOWN!");
      change_speed(-25);
      break;
    case LEFT_BUTTON:
      Serial.println("LEFT!");
      turnLeft();
      break;
    case RIGHT_BUTTON:
      Serial.println("RIGHT!");
      turnRight();
      break;
    case STOP_BUTTON:
      Serial.println("STOP!");
      stop();
      break;
    default:
      Serial.print("==> 0x");
      Serial.println(results->value, HEX);
  }
}

void loop()
{

  decode_results results;
  
  if (irrecv.decode(&results)) {
    processControls(&results);
    irrecv.resume();              // Prepare for the next value

  }
  //delay(50);
}
