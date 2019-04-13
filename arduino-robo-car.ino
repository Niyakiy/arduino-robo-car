#include <IRremote.h>

// Driver PINs
#define rightFwdPin 11
#define rightBkwPin 10
#define rightSpeedPin 5
#define leftFwdPin 9
#define leftBkwPin 8
#define leftSpeedPin 6

// IR Controls codes
#define STOP_BUTTON  0x3E108
#define UP_BUTTON    0x1E108
#define DOWN_BUTTON  0x9E108
#define RIGHT_BUTTON 0xDE108
#define LEFT_BUTTON  0x5E108

// movement specific defines
#define DEFAULT_ACTION_DELAY 250
#define DEFAULT_SPEED_STEP 25
#define MAX_SPEED 255
#define MIN_SPEED 75

// Global Flags
#define DEFAULT_DEBUG_FLAG true

// Custom types
typedef enum { LEFT, RIGHT, ALL} driver_type;

// GLOBALS
int forwardSpeed;

IRrecv irrecv(2);

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

void change_speed(int speed_step, driver_type drive=ALL) {
  int new_speed = forwardSpeed + speed_step;

  // checking MAX speed boudaries
  if (abs(new_speed) > MAX_SPEED) {
    new_speed = new_speed > 0 ? MAX_SPEED : -MAX_SPEED;
  }

  // making sure not to have low speed
  if (abs(new_speed) < MIN_SPEED) {
    new_speed = new_speed > 0 ? MIN_SPEED : -MIN_SPEED;
  }

  forwardSpeed = new_speed;

  if (DEFAULT_DEBUG_FLAG) {
    Serial.print("fwdSpeed=");
    Serial.println(forwardSpeed);
  }
  if (forwardSpeed > 0) {
    forward(drive);
  }
  else {
    backward(drive);
  }
  analogWrite(leftSpeedPin, abs(forwardSpeed));
  analogWrite(rightSpeedPin, abs(forwardSpeed));
}

void turnRight(int action_delay=DEFAULT_ACTION_DELAY) {
  if (forwardSpeed == 0) {
    change_speed(DEFAULT_SPEED_STEP * 4, LEFT);
    delay(action_delay);
    stop();
  }
  else {
    if (forwardSpeed > 0)
      stop(RIGHT);
    else
      stop(LEFT);
    delay(action_delay);
    change_speed(0);
  }
}

void turnLeft(int action_delay=DEFAULT_ACTION_DELAY) {
  if (forwardSpeed == 0) {
    change_speed(DEFAULT_SPEED_STEP * 4, RIGHT);
    delay(action_delay);
    stop();
  }
  else {
    if (forwardSpeed > 0)
      stop(LEFT);
    else
      stop(RIGHT);
    delay(action_delay);
    change_speed(0);
  }
}

void processControls(decode_results *results) {
  char action_name[10];
  strcpy(action_name, "");
  switch (results->value) {
    case UP_BUTTON:
      strcpy(action_name, "UP");
      change_speed(DEFAULT_SPEED_STEP);
      break;
    case DOWN_BUTTON:
      strcpy(action_name, "DOWN");
      change_speed(-DEFAULT_SPEED_STEP);
      break;
    case LEFT_BUTTON:
      strcpy(action_name, "LEFT");
      turnLeft();
      break;
    case RIGHT_BUTTON:
      strcpy(action_name, "RIGHT");
      turnRight();
      break;
    case STOP_BUTTON:
      strcpy(action_name, "STOP");
      stop();
      break;
    default:
      break;
  }
  if (DEFAULT_DEBUG_FLAG && strlen(action_name)) {
    Serial.print("In processControls - ");
    Serial.println(action_name);
  }
}

void loop()
{
  decode_results results;

  if (irrecv.decode(&results)) {
    processControls(&results);
    irrecv.resume();
  }
}
