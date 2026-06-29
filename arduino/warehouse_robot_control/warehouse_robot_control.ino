#include <SoftwareSerial.h>

SoftwareSerial hm10(2, 3);
// Arduino D2 = RX ← HM-10 TXD
// Arduino D3 = TX → HM-10 RXD

// ======================================================
// SPEED CONTROL SECTION
// ------------------------------------------------------
// L293D Enable pins must be connected to PWM pins.
// Speed range: 0 ~ 255
// 255 = full speed
// 180 = medium-fast
// 140 = medium
// 100 = slow
// Too low may not start the motor.
// ======================================================

const int RIGHT_SPEED = 250;
const int LEFT_SPEED = 250;
const int FORK_SPEED = 250;

// Right wheel motor
const int RIGHT_EN = 5;   // L293D pin 1, PWM speed control
const int RIGHT_IN1 = 6;  // L293D pin 2
const int RIGHT_IN2 = 7;  // L293D pin 7

// Left wheel motor
const int LEFT_EN = 9;    // L293D pin 9, PWM speed control
const int LEFT_IN1 = 8;   // L293D pin 15
const int LEFT_IN2 = 12;  // L293D pin 10

// Forklift motor
const int FORK_EN = 10;   // Second L293D pin 1, PWM speed control
const int FORK_IN1 = 11;  // Second L293D pin 2
const int FORK_IN2 = 13;  // Second L293D pin 7

// Auto-stop timeout when no command is received from the button
const unsigned long COMMAND_TIMEOUT = 400; // ms

unsigned long lastWheelCommandTime = 0;
unsigned long lastForkCommandTime = 0;

bool wheelMoving = false;
bool forkMoving = false;

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
  Serial.println("A = forward while pressed");
  Serial.println("C = backward while pressed");
  Serial.println("D = left while pressed");
  Serial.println("B = right while pressed");
  Serial.println("E = fork up while pressed");
  Serial.println("F = fork down while pressed");
  Serial.println("H = stop all");
}

void loop() {
  if (hm10.available()) {
    char data = hm10.read();

    Serial.print("Received: ");
    Serial.println(data);

    // -------------------- Wheels --------------------

    if (data == 'A' || data == 'a') {
      moveForward();
      wheelMoving = true;
      lastWheelCommandTime = millis();

      hm10.println("Forward");
      Serial.println("Forward");
    }

    else if (data == 'C' || data == 'c') {
      moveBackward();
      wheelMoving = true;
      lastWheelCommandTime = millis();

      hm10.println("Backward");
      Serial.println("Backward");
    }

    else if (data == 'D' || data == 'd') {
      turnLeft();
      wheelMoving = true;
      lastWheelCommandTime = millis();

      hm10.println("Left");
      Serial.println("Left");
    }

    else if (data == 'B' || data == 'b') {
      turnRight();
      wheelMoving = true;
      lastWheelCommandTime = millis();

      hm10.println("Right");
      Serial.println("Right");
    }

    // -------------------- Forklift --------------------

    else if (data == 'E' || data == 'e') {
      forkUp();
      forkMoving = true;
      lastForkCommandTime = millis();

      hm10.println("Fork up");
      Serial.println("Fork up");
    }

    else if (data == 'F' || data == 'f') {
      forkDown();
      forkMoving = true;
      lastForkCommandTime = millis();

      hm10.println("Fork down");
      Serial.println("Fork down");
    }

    // -------------------- Emergency / full stop --------------------

    else if (data == 'H' || data == 'h') {
      stopAll();

      hm10.println("Stop all");
      Serial.println("Stop all");
    }
  }

  // Stop the wheels if no wheel command is received for a certain time
  if (wheelMoving && millis() - lastWheelCommandTime > COMMAND_TIMEOUT) {
    stopWheels();
    wheelMoving = false;
    Serial.println("Wheels auto stop");
  }

  // Stop the forklift if no forklift command is received for a certain time
  if (forkMoving && millis() - lastForkCommandTime > COMMAND_TIMEOUT) {
    stopFork();
    forkMoving = false;
    Serial.println("Fork auto stop");
  }
}

// -------------------- Wheel control --------------------

void moveForward() {
  digitalWrite(RIGHT_IN1, HIGH);
  digitalWrite(RIGHT_IN2, LOW);
  analogWrite(RIGHT_EN, RIGHT_SPEED); // speed control

  digitalWrite(LEFT_IN1, HIGH);
  digitalWrite(LEFT_IN2, LOW);
  analogWrite(LEFT_EN, LEFT_SPEED);   // speed control
}

void moveBackward() {
  digitalWrite(RIGHT_IN1, LOW);
  digitalWrite(RIGHT_IN2, HIGH);
  analogWrite(RIGHT_EN, RIGHT_SPEED); // speed control

  digitalWrite(LEFT_IN1, LOW);
  digitalWrite(LEFT_IN2, HIGH);
  analogWrite(LEFT_EN, LEFT_SPEED);   // speed control
}

void turnLeft() {
  // Right wheel moves forward, left wheel moves backward
  digitalWrite(RIGHT_IN1, HIGH);
  digitalWrite(RIGHT_IN2, LOW);
  analogWrite(RIGHT_EN, RIGHT_SPEED); // speed control

  digitalWrite(LEFT_IN1, LOW);
  digitalWrite(LEFT_IN2, HIGH);
  analogWrite(LEFT_EN, LEFT_SPEED);   // speed control
}

void turnRight() {
  // Right wheel moves backward, left wheel moves forward
  digitalWrite(RIGHT_IN1, LOW);
  digitalWrite(RIGHT_IN2, HIGH);
  analogWrite(RIGHT_EN, RIGHT_SPEED); // speed control

  digitalWrite(LEFT_IN1, HIGH);
  digitalWrite(LEFT_IN2, LOW);
  analogWrite(LEFT_EN, LEFT_SPEED);   // speed control
}

void stopWheels() {
  digitalWrite(RIGHT_IN1, LOW);
  digitalWrite(RIGHT_IN2, LOW);
  analogWrite(RIGHT_EN, 0); // stop speed signal

  digitalWrite(LEFT_IN1, LOW);
  digitalWrite(LEFT_IN2, LOW);
  analogWrite(LEFT_EN, 0);  // stop speed signal
}

// -------------------- Forklift control --------------------

void forkUp() {
  digitalWrite(FORK_IN1, HIGH);
  digitalWrite(FORK_IN2, LOW);
  analogWrite(FORK_EN, FORK_SPEED); // speed control
}

void forkDown() {
  digitalWrite(FORK_IN1, LOW);
  digitalWrite(FORK_IN2, HIGH);
  analogWrite(FORK_EN, FORK_SPEED); // speed control
}

void stopFork() {
  digitalWrite(FORK_IN1, LOW);
  digitalWrite(FORK_IN2, LOW);
  analogWrite(FORK_EN, 0); // stop speed signal
}

// -------------------- Stop all --------------------

void stopAll() {
  stopWheels();
  stopFork();

  wheelMoving = false;
  forkMoving = false;
}
