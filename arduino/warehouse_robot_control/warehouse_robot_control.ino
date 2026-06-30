#include <SoftwareSerial.h>

SoftwareSerial hm10(2, 3);
// Arduino D2 = RX ← HM-10 TXD
// Arduino D3 = TX → HM-10 RXD

// ======================================================
// SPEED CONTROL SECTION
// ======================================================

const int GEAR1_MIN_SPEED = 80;
const int GEAR1_MAX_SPEED = 130;

const int GEAR2_MIN_SPEED = 130;
const int GEAR2_MAX_SPEED = 180;

int currentMinWheelSpeed = GEAR1_MIN_SPEED;
int currentMaxWheelSpeed = GEAR1_MAX_SPEED;

const int FORK_SPEED = 20;

// No acceleration for now because min and max are both 200
const unsigned long RAMP_TIME = 3000; // ms

// If one button press keeps the motor moving too long,
// reduce this value.
// If the app sends repeated commands while held, keep this
// slightly longer than the app repeat interval.
const unsigned long COMMAND_TIMEOUT = 200; // ms

// ======================================================
// MOTOR PINS
// ======================================================

// Right wheel motor
const int RIGHT_EN = 5;   // L293D pin 1, PWM speed control
const int RIGHT_IN1 = 6;  // L293D pin 2
const int RIGHT_IN2 = 7;  // L293D pin 7

// Left wheel motor
const int LEFT_EN = 9;    // L293D pin 9, PWM speed control
const int LEFT_IN1 = 8;   // L293D pin 15
const int LEFT_IN2 = 12;  // L293D pin 10

// Forklift motor
const int FORK_EN = 10;
const int FORK_IN1 = 11;
const int FORK_IN2 = 13;

// ======================================================
// STATE VARIABLES
// ======================================================

unsigned long lastWheelCommandTime = 0;
unsigned long lastForkCommandTime = 0;

bool wheelMoving = false;
bool forkMoving = false;

char currentWheelCommand = '\0';
unsigned long wheelCommandStartTime = 0;

void setup() {
  Serial.begin(9600);
  hm10.begin(9600);

  pinMode(RIGHT_EN, OUTPUT);
  pinMode(RIGHT_IN1, OUTPUT);
  pinMode(RIGHT_IN2, OUTPUT);

  pinMode(LEFT_EN, OUTPUT);
  pinMode(LEFT_IN1, OUTPUT);
  pinMode(LEFT_IN2, OUTPUT);

  pinMode(FORK_EN, OUTPUT);
  pinMode(FORK_IN1, OUTPUT);
  pinMode(FORK_IN2, OUTPUT);

  stopAll();

  Serial.println("BLE robot control ready.");
  Serial.println("A = forward");
  Serial.println("B = turn right");
  Serial.println("C = backward");
  Serial.println("D = turn left");
  Serial.println("R = test right wheel only");
  Serial.println("L = test left wheel only");
  Serial.println("E = fork up");
  Serial.println("F = fork down");
  Serial.println("H = gear 1");
  Serial.println("G = gear 2");
  Serial.println("S = stop all");
}

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
      forkUp();
      forkMoving = true;
      lastForkCommandTime = millis();

      hm10.println("Fork up");
      Serial.println("Fork up");
    }

    else if (data == 'F') {
      forkDown();
      forkMoving = true;
      lastForkCommandTime = millis();

      hm10.println("Fork down");
      Serial.println("Fork down");
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

  if (wheelMoving && millis() - lastWheelCommandTime > COMMAND_TIMEOUT) {
    stopWheels();
    wheelMoving = false;
    currentWheelCommand = '\0';

    Serial.println("Wheels auto stop");
  }

  if (forkMoving && millis() - lastForkCommandTime > COMMAND_TIMEOUT) {
    stopFork();
    forkMoving = false;

    Serial.println("Fork auto stop");
  }
}

// ======================================================
// GEAR CONTROL
// ======================================================

void setGear1() {
  currentMinWheelSpeed = GEAR1_MIN_SPEED;
  currentMaxWheelSpeed = GEAR1_MAX_SPEED;
  currentWheelCommand = '\0';
}

void setGear2() {
  currentMinWheelSpeed = GEAR2_MIN_SPEED;
  currentMaxWheelSpeed = GEAR2_MAX_SPEED;
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

  int currentSpeed = calculateRampSpeed();

  if (command == 'A') {
    moveForward(currentSpeed);
    hm10.print("Forward speed: ");
    hm10.println(currentSpeed);

    Serial.print("Forward speed: ");
    Serial.println(currentSpeed);
  }

  else if (command == 'B') {
    turnRight(currentSpeed);
    hm10.print("Right turn speed: ");
    hm10.println(currentSpeed);

    Serial.print("Right turn speed: ");
    Serial.println(currentSpeed);
  }

  else if (command == 'C') {
    moveBackward(currentSpeed);
    hm10.print("Backward speed: ");
    hm10.println(currentSpeed);

    Serial.print("Backward speed: ");
    Serial.println(currentSpeed);
  }

  else if (command == 'D') {
    turnLeft(currentSpeed);
    hm10.print("Left turn speed: ");
    hm10.println(currentSpeed);

    Serial.print("Left turn speed: ");
    Serial.println(currentSpeed);
  }

  else if (command == 'R') {
    testRightWheel(currentSpeed);
    hm10.print("Right wheel test speed: ");
    hm10.println(currentSpeed);

    Serial.print("Right wheel test speed: ");
    Serial.println(currentSpeed);
  }

  else if (command == 'L') {
    testLeftWheel(currentSpeed);
    hm10.print("Left wheel test speed: ");
    hm10.println(currentSpeed);

    Serial.print("Left wheel test speed: ");
    Serial.println(currentSpeed);
  }
}

int calculateRampSpeed() {
  unsigned long heldTime = millis() - wheelCommandStartTime;

  if (heldTime >= RAMP_TIME) {
    return currentMaxWheelSpeed;
  }

  int speedValue = map(
    heldTime,
    0,
    RAMP_TIME,
    currentMinWheelSpeed,
    currentMaxWheelSpeed
  );

  return constrain(speedValue, currentMinWheelSpeed, currentMaxWheelSpeed);
}

// ======================================================
// LOW-LEVEL MOTOR CONTROL
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
void moveForward(int speedValue) {
  setRightMotor(1, speedValue);
  setLeftMotor(1, speedValue);
}

// C = backward
void moveBackward(int speedValue) {
  setRightMotor(-1, speedValue);
  setLeftMotor(-1, speedValue);
}

// D = turn left
void turnLeft(int speedValue) {
  // Right wheel forward, left wheel backward
  setRightMotor(1, speedValue);
  setLeftMotor(-1, speedValue);
}

// B = turn right
void turnRight(int speedValue) {
  // Right wheel backward, left wheel forward
  setRightMotor(-1, speedValue);
  setLeftMotor(1, speedValue);
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
// FORKLIFT CONTROL
// ======================================================

void forkUp() {
  digitalWrite(FORK_IN1, HIGH);
  digitalWrite(FORK_IN2, LOW);
  analogWrite(FORK_EN, FORK_SPEED);
}

void forkDown() {
  digitalWrite(FORK_IN1, LOW);
  digitalWrite(FORK_IN2, HIGH);
  analogWrite(FORK_EN, FORK_SPEED);
}

void stopFork() {
  digitalWrite(FORK_IN1, LOW);
  digitalWrite(FORK_IN2, LOW);
  analogWrite(FORK_EN, 0);
}

// ======================================================
// STOP ALL
// ======================================================

void stopAll() {
  stopWheels();
  stopFork();

  wheelMoving = false;
  forkMoving = false;
  currentWheelCommand = '\0';
}
