#include <SoftwareSerial.h>

SoftwareSerial hm10(2, 3);
// Arduino D2 = RX ← HM-10 TXD
// Arduino D3 = TX → HM-10 RXD

// ======================================================
// SPEED CONTROL SECTION
// ======================================================

// ======================================================
// RIGHT WHEEL SPEED SETTINGS
// ======================================================

const int RIGHT_GEAR1_MIN_SPEED = 80;
const int RIGHT_GEAR1_MAX_SPEED = 80;

const int RIGHT_GEAR2_MIN_SPEED = 90;
const int RIGHT_GEAR2_MAX_SPEED = 110;

// Time for the right wheel to reach maximum speed
const unsigned long RIGHT_RAMP_TIME = 2000; // ms

// ======================================================
// LEFT WHEEL SPEED SETTINGS
// ======================================================

const int LEFT_GEAR1_MIN_SPEED = 80;
const int LEFT_GEAR1_MAX_SPEED = 80;

const int LEFT_GEAR2_MIN_SPEED = 90;
const int LEFT_GEAR2_MAX_SPEED = 110;

// Time for the left wheel to reach maximum speed
const unsigned long LEFT_RAMP_TIME = 2000; // ms

int currentRightMinWheelSpeed = RIGHT_GEAR1_MIN_SPEED;
int currentRightMaxWheelSpeed = RIGHT_GEAR1_MAX_SPEED;

int currentLeftMinWheelSpeed = LEFT_GEAR1_MIN_SPEED;
int currentLeftMaxWheelSpeed = LEFT_GEAR1_MAX_SPEED;

// Two lifting motors speed using the second L298N module
// Lift up and lift down use separate speed values for better control.
const int LIFT_UP_SPEED = 50;
const int LIFT_DOWN_SPEED = 130;


// Wheel command timeout settings.
// Gear 1 and Gear 2 use separate wheel timeouts so each gear can be tuned independently.
// If one wheel button press keeps the robot moving too long, reduce the relevant value.
// If the app sends repeated commands while held, keep this slightly longer than the app repeat interval.
const unsigned long GEAR1_COMMAND_TIMEOUT = 150; // ms
const unsigned long GEAR2_COMMAND_TIMEOUT = 330; // ms

unsigned long currentWheelCommandTimeout = GEAR1_COMMAND_TIMEOUT;

// Lift command timeout.
// This is separate from wheel timeout so lifting can be tuned independently.
const unsigned long LIFT_COMMAND_TIMEOUT = 80; // ms

// ======================================================
// MOTOR PINS
// ======================================================

// ======================================================
// First L298N module: right and left wheel motors
// ======================================================

// Right wheel motor
const int RIGHT_EN = 5;   // L298N #1 ENA, PWM speed control
const int RIGHT_IN1 = 6;  // L298N #1 IN1
const int RIGHT_IN2 = 7;  // L298N #1 IN2

// Left wheel motor
const int LEFT_EN = 9;    // L298N #1 ENB, PWM speed control
const int LEFT_IN1 = 8;   // L298N #1 IN3
const int LEFT_IN2 = 12;  // L298N #1 IN4

// ======================================================
// Second L298N module: two lifting motors
// ======================================================

// Lift motor 1
const int LIFT1_EN = 10;   // L298N #2 ENA, PWM speed control
const int LIFT1_IN1 = 4;   // L298N #2 IN1
const int LIFT1_IN2 = A2;  // L298N #2 IN2 (moved from D13)

// Lift motor 2
const int LIFT2_EN = 11;   // L298N #2 ENB, PWM speed control
const int LIFT2_IN1 = A0;  // L298N #2 IN3
const int LIFT2_IN2 = A1;  // L298N #2 IN4

// ======================================================
// STATE VARIABLES
// ======================================================

unsigned long lastWheelCommandTime = 0;
unsigned long lastLiftCommandTime = 0;

bool wheelMoving = false;
bool liftMoving = false;

char currentWheelCommand = '\0';
unsigned long wheelCommandStartTime = 0;

// ======================================================
// SETUP
// ======================================================

void setup() {
  Serial.begin(9600);
  hm10.begin(9600);

  // Wheel motors
  pinMode(RIGHT_EN, OUTPUT);
  pinMode(RIGHT_IN1, OUTPUT);
  pinMode(RIGHT_IN2, OUTPUT);

  pinMode(LEFT_EN, OUTPUT);
  pinMode(LEFT_IN1, OUTPUT);
  pinMode(LEFT_IN2, OUTPUT);

  // Lift motors
  pinMode(LIFT1_EN, OUTPUT);
  pinMode(LIFT1_IN1, OUTPUT);
  pinMode(LIFT1_IN2, OUTPUT);

  pinMode(LIFT2_EN, OUTPUT);
  pinMode(LIFT2_IN1, OUTPUT);
  pinMode(LIFT2_IN2, OUTPUT);

  stopAll();

  Serial.println("BLE robot control ready.");
  Serial.println("A = forward");
  Serial.println("B = turn right");
  Serial.println("C = backward");
  Serial.println("D = turn left");
  Serial.println("R = test right wheel only");
  Serial.println("L = test left wheel only");
  Serial.println("E = lift up");
  Serial.println("F = lift down");
  Serial.println("H = gear 1");
  Serial.println("G = gear 2");
  Serial.println("S = stop all");
}

// ======================================================
// LOOP
// ======================================================

void loop() {
  if (hm10.available()) {
    char data = hm10.read();
    data = toupper(data);

    Serial.print("Received: ");
    Serial.println(data);

    if (data == 'A' || data == 'B' || data == 'C' || data == 'D' ||
        data == 'R' || data == 'L') {
      handleWheelCommand(data);

      wheelMoving = true;
      lastWheelCommandTime = millis();
    }

    else if (data == 'E') {
      liftUp();
      liftMoving = true;
      lastLiftCommandTime = millis();

      hm10.println("Lift up");
      Serial.println("Lift up");
    }

    else if (data == 'F') {
      liftDown();
      liftMoving = true;
      lastLiftCommandTime = millis();

      hm10.println("Lift down");
      Serial.println("Lift down");
    }

    else if (data == 'H') {
      setGear1();

      hm10.println("Gear 1 selected");
      Serial.println("Gear 1 selected");
    }

    else if (data == 'G') {
      setGear2();

      hm10.println("Gear 2 selected");
      Serial.println("Gear 2 selected");
    }

    else if (data == 'S') {
      stopAll();

      hm10.println("Stop all");
      Serial.println("Stop all");
    }
  }

  if (wheelMoving && millis() - lastWheelCommandTime > currentWheelCommandTimeout) {
    stopWheels();
    wheelMoving = false;
    currentWheelCommand = '\0';

    Serial.println("Wheels auto stop");
  }

  if (liftMoving && millis() - lastLiftCommandTime > LIFT_COMMAND_TIMEOUT) {
    stopLift();
    liftMoving = false;

    Serial.println("Lift auto stop");
  }
}

// ======================================================
// GEAR CONTROL
// ======================================================

void setGear1() {
  currentRightMinWheelSpeed = RIGHT_GEAR1_MIN_SPEED;
  currentRightMaxWheelSpeed = RIGHT_GEAR1_MAX_SPEED;

  currentLeftMinWheelSpeed = LEFT_GEAR1_MIN_SPEED;
  currentLeftMaxWheelSpeed = LEFT_GEAR1_MAX_SPEED;

  currentWheelCommandTimeout = GEAR1_COMMAND_TIMEOUT;
  currentWheelCommand = '\0';
}

void setGear2() {
  currentRightMinWheelSpeed = RIGHT_GEAR2_MIN_SPEED;
  currentRightMaxWheelSpeed = RIGHT_GEAR2_MAX_SPEED;

  currentLeftMinWheelSpeed = LEFT_GEAR2_MIN_SPEED;
  currentLeftMaxWheelSpeed = LEFT_GEAR2_MAX_SPEED;

  currentWheelCommandTimeout = GEAR2_COMMAND_TIMEOUT;
  currentWheelCommand = '\0';
}

// ======================================================
// WHEEL COMMAND CONTROL
// ======================================================

void handleWheelCommand(char command) {
  if (command != currentWheelCommand) {
    currentWheelCommand = command;
    wheelCommandStartTime = millis();
  }

  int currentRightSpeed = calculateRightRampSpeed();
  int currentLeftSpeed = calculateLeftRampSpeed();

  if (command == 'A') {
    moveForward(currentRightSpeed, currentLeftSpeed);
    hm10.print("Forward speed R/L: ");
    hm10.print(currentRightSpeed);
    hm10.print(" / ");
    hm10.println(currentLeftSpeed);

    Serial.print("Forward speed R/L: ");
    Serial.print(currentRightSpeed);
    Serial.print(" / ");
    Serial.println(currentLeftSpeed);
  }

  else if (command == 'B') {
    turnRight(currentRightSpeed, currentLeftSpeed);
    hm10.print("Right turn speed R/L: ");
    hm10.print(currentRightSpeed);
    hm10.print(" / ");
    hm10.println(currentLeftSpeed);

    Serial.print("Right turn speed R/L: ");
    Serial.print(currentRightSpeed);
    Serial.print(" / ");
    Serial.println(currentLeftSpeed);
  }

  else if (command == 'C') {
    moveBackward(currentRightSpeed, currentLeftSpeed);
    hm10.print("Backward speed R/L: ");
    hm10.print(currentRightSpeed);
    hm10.print(" / ");
    hm10.println(currentLeftSpeed);

    Serial.print("Backward speed R/L: ");
    Serial.print(currentRightSpeed);
    Serial.print(" / ");
    Serial.println(currentLeftSpeed);
  }

  else if (command == 'D') {
    turnLeft(currentRightSpeed, currentLeftSpeed);
    hm10.print("Left turn speed R/L: ");
    hm10.print(currentRightSpeed);
    hm10.print(" / ");
    hm10.println(currentLeftSpeed);

    Serial.print("Left turn speed R/L: ");
    Serial.print(currentRightSpeed);
    Serial.print(" / ");
    Serial.println(currentLeftSpeed);
  }

  else if (command == 'R') {
    testRightWheel(currentRightSpeed);
    hm10.print("Right wheel test speed: ");
    hm10.println(currentRightSpeed);

    Serial.print("Right wheel test speed: ");
    Serial.println(currentRightSpeed);
  }

  else if (command == 'L') {
    testLeftWheel(currentLeftSpeed);
    hm10.print("Left wheel test speed: ");
    hm10.println(currentLeftSpeed);

    Serial.print("Left wheel test speed: ");
    Serial.println(currentLeftSpeed);
  }
}

int calculateRightRampSpeed() {
  unsigned long heldTime = millis() - wheelCommandStartTime;

  if (heldTime >= RIGHT_RAMP_TIME) {
    return currentRightMaxWheelSpeed;
  }

  int speedValue = map(
    heldTime,
    0,
    RIGHT_RAMP_TIME,
    currentRightMinWheelSpeed,
    currentRightMaxWheelSpeed
  );

  return constrain(speedValue, currentRightMinWheelSpeed, currentRightMaxWheelSpeed);
}

int calculateLeftRampSpeed() {
  unsigned long heldTime = millis() - wheelCommandStartTime;

  if (heldTime >= LEFT_RAMP_TIME) {
    return currentLeftMaxWheelSpeed;
  }

  int speedValue = map(
    heldTime,
    0,
    LEFT_RAMP_TIME,
    currentLeftMinWheelSpeed,
    currentLeftMaxWheelSpeed
  );

  return constrain(speedValue, currentLeftMinWheelSpeed, currentLeftMaxWheelSpeed);
}

// ======================================================
// LOW-LEVEL WHEEL MOTOR CONTROL
// ------------------------------------------------------
// Positive power = forward
// Negative power = backward
// 0 = stop
// ======================================================

void setRightMotor(int power, int speedValue) {
  if (power > 0) {
    // Right wheel forward
    digitalWrite(RIGHT_IN1, LOW);
    digitalWrite(RIGHT_IN2, HIGH);
    analogWrite(RIGHT_EN, speedValue);
  }

  else if (power < 0) {
    // Right wheel backward
    digitalWrite(RIGHT_IN1, HIGH);
    digitalWrite(RIGHT_IN2, LOW);
    analogWrite(RIGHT_EN, speedValue);
  }

  else {
    digitalWrite(RIGHT_IN1, LOW);
    digitalWrite(RIGHT_IN2, LOW);
    analogWrite(RIGHT_EN, 0);
  }
}

void setLeftMotor(int power, int speedValue) {
  if (power > 0) {
    // Left wheel forward
    digitalWrite(LEFT_IN1, LOW);
    digitalWrite(LEFT_IN2, HIGH);
    analogWrite(LEFT_EN, speedValue);
  }

  else if (power < 0) {
    // Left wheel backward
    digitalWrite(LEFT_IN1, HIGH);
    digitalWrite(LEFT_IN2, LOW);
    analogWrite(LEFT_EN, speedValue);
  }

  else {
    digitalWrite(LEFT_IN1, LOW);
    digitalWrite(LEFT_IN2, LOW);
    analogWrite(LEFT_EN, 0);
  }
}

// ======================================================
// MOVEMENT FUNCTIONS
// ======================================================

// A = forward
void moveForward(int rightSpeedValue, int leftSpeedValue) {
  setRightMotor(1, rightSpeedValue);
  setLeftMotor(1, leftSpeedValue);
}

// C = backward
void moveBackward(int rightSpeedValue, int leftSpeedValue) {
  setRightMotor(-1, rightSpeedValue);
  setLeftMotor(-1, leftSpeedValue);
}

// D = turn left
void turnLeft(int rightSpeedValue, int leftSpeedValue) {
  // Right wheel forward, left wheel backward
  setRightMotor(1, rightSpeedValue);
  setLeftMotor(-1, leftSpeedValue);
}

// B = turn right
void turnRight(int rightSpeedValue, int leftSpeedValue) {
  // Right wheel backward, left wheel forward
  setRightMotor(-1, rightSpeedValue);
  setLeftMotor(1, leftSpeedValue);
}

// R = right wheel only test
void testRightWheel(int speedValue) {
  setRightMotor(1, speedValue);
  setLeftMotor(0, 0);
}

// L = left wheel only test
void testLeftWheel(int speedValue) {
  setRightMotor(0, 0);
  setLeftMotor(1, speedValue);
}

void stopWheels() {
  setRightMotor(0, 0);
  setLeftMotor(0, 0);
}

// ======================================================
// LOW-LEVEL LIFT MOTOR CONTROL
// ------------------------------------------------------
// Positive power = lift up direction
// Negative power = lift down direction
// 0 = stop
// ======================================================

void setLiftMotor1(int power, int speedValue) {
  if (power > 0) {
    digitalWrite(LIFT1_IN1, HIGH);
    digitalWrite(LIFT1_IN2, LOW);
    analogWrite(LIFT1_EN, speedValue);
  }

  else if (power < 0) {
    digitalWrite(LIFT1_IN1, LOW);
    digitalWrite(LIFT1_IN2, HIGH);
    analogWrite(LIFT1_EN, speedValue);
  }

  else {
    digitalWrite(LIFT1_IN1, LOW);
    digitalWrite(LIFT1_IN2, LOW);
    analogWrite(LIFT1_EN, 0);
  }
}

void setLiftMotor2(int power, int speedValue) {
  if (power > 0) {
    digitalWrite(LIFT2_IN1, HIGH);
    digitalWrite(LIFT2_IN2, LOW);
    analogWrite(LIFT2_EN, speedValue);
  }

  else if (power < 0) {
    digitalWrite(LIFT2_IN1, LOW);
    digitalWrite(LIFT2_IN2, HIGH);
    analogWrite(LIFT2_EN, speedValue);
  }

  else {
    digitalWrite(LIFT2_IN1, LOW);
    digitalWrite(LIFT2_IN2, LOW);
    analogWrite(LIFT2_EN, 0);
  }
}

// ======================================================
// LIFTING FUNCTIONS
// ======================================================

// E = lift up
void liftUp() {
  setLiftMotor1(1, LIFT_UP_SPEED);
  setLiftMotor2(1, LIFT_UP_SPEED);
}

// F = lift down
void liftDown() {
  setLiftMotor1(-1, LIFT_DOWN_SPEED);
  setLiftMotor2(-1, LIFT_DOWN_SPEED);
}

void stopLift() {
  setLiftMotor1(0, 0);
  setLiftMotor2(0, 0);
}

// ======================================================
// STOP ALL
// ======================================================

void stopAll() {
  stopWheels();
  stopLift();

  wheelMoving = false;
  liftMoving = false;
  currentWheelCommand = '\0';
}
