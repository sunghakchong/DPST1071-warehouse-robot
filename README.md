# Warehouse Robot Arduino Control

This repository contains the Arduino control code and wiring documentation for our DPST1071 warehouse robot project.

The robot is controlled through an iPhone BLE controller app using an HM-10 Bluetooth module. The Arduino UNO receives single-character commands through Bluetooth and controls four DC motors using two L293D motor driver ICs.

The first L293D controls the right and left wheel motors. The second L293D controls two lifting motors for the forklift/lifting system.

> **Project Status:** This project is still in progress. HM-10 Bluetooth communication, right/left wheel control, two-motor lifting control, PWM speed control, gear selection, and auto-stop behaviour have been added to the Arduino code. Final physical integration, speed tuning, lifting mechanism testing, and full robot testing are still being developed and tested.

---

## System Overview

The overall control structure is:

```text
iPhone BLE Controller App
        ↓
HM-10 Bluetooth Module
        ↓
Arduino UNO
        ↓
2 × L293D Motor Driver ICs
        ↓
4 × DC Motors
```

The HM-10 Bluetooth module does not control the motors directly. It only sends single-character commands from the phone to the Arduino. The Arduino reads these commands using `SoftwareSerial`, selects the correct movement or lifting function, and sends direction and PWM speed signals to the L293D motor driver ICs. The L293D motor drivers then control the direction and power of each DC motor.

---

## Hardware Components

- Arduino UNO
- HM-10 BLE Bluetooth module
- 2 × L293D Dual Full Bridge Motor Driver IC
- 4 × DC motors
  - Right wheel motor
  - Left wheel motor
  - Lift motor 1
  - Lift motor 2
- External motor battery
- Breadboard
- Jumper wires
- 1kΩ and 2kΩ resistors for HM-10 RXD voltage divider

---

## Arduino, Bluetooth, and Motor Driver Connection Overview

### 1. Arduino ↔ HM-10 Bluetooth Module

| HM-10 Pin | Arduino Pin | Purpose |
|---|---|---|
| VCC | 5V or 3.3V | Bluetooth module power |
| GND | GND | Common ground |
| TXD | D2 | HM-10 sends data to Arduino |
| RXD | D3 | Arduino sends data to HM-10 |

The Arduino code uses:

```cpp
SoftwareSerial hm10(2, 3);
```

This means:

```text
Arduino D2 = RX ← HM-10 TXD
Arduino D3 = TX → HM-10 RXD
```

Because Arduino UNO uses 5V logic and HM-10 usually uses 3.3V logic, the HM-10 RXD pin should use a voltage divider.

Recommended voltage divider:

```text
Arduino D3 ── 1kΩ ──┬── HM-10 RXD
                    |
                   2kΩ
                    |
                   GND
```

---

## 2. Arduino ↔ L293D Motor Driver ICs

The L293D is used because Arduino pins cannot safely power DC motors directly. The Arduino only sends control signals, while the L293D handles the motor current and direction control.

Each L293D can control two DC motors. This project uses two L293D ICs:

```text
First L293D  → Right wheel motor + Left wheel motor
Second L293D → Lift motor 1 + Lift motor 2
```

### L293D Pin Numbering

When the notch or dot on the L293D is facing upward, the pin numbering is:

```text
        L293D
     ┌─────────┐
  1  │●        │ 16
  2  │         │ 15
  3  │         │ 14
  4  │         │ 13
  5  │         │ 12
  6  │         │ 11
  7  │         │ 10
  8  │         │ 9
     └─────────┘
```

---

## 3. First L293D: Right and Left Wheel Motors

The first L293D controls the right and left wheel motors.

### First L293D — Right and Left Wheel Motors

| First L293D Pin | Arduino / Motor Connection | Purpose |
|---:|---|---|
| Pin 1 | Arduino D5 | Right motor enable / speed |
| Pin 2 | Arduino D6 | Right motor IN1 |
| Pin 3 | Right motor wire 1 | Right motor output |
| Pin 4 | GND | Ground |
| Pin 5 | GND | Ground |
| Pin 6 | Right motor wire 2 | Right motor output |
| Pin 7 | Arduino D7 | Right motor IN2 |
| Pin 8 | VIN / external battery + | Motor power |
| Pin 9 | Arduino D9 | Left motor enable / speed |
| Pin 10 | Arduino D12 | Left motor IN2 |
| Pin 11 | Left motor wire 1 | Left motor output |
| Pin 12 | GND | Ground |
| Pin 13 | GND | Ground |
| Pin 14 | Left motor wire 2 | Left motor output |
| Pin 15 | Arduino D8 | Left motor IN1 |
| Pin 16 | Arduino 5V | Logic power |

### Right Wheel Motor

| L293D Pin | Arduino / Motor Connection | Purpose |
|---:|---|---|
| Pin 1 | Arduino D5 | Enable / PWM speed control |
| Pin 2 | Arduino D6 | Direction control IN1 |
| Pin 3 | Right motor wire 1 | Motor output |
| Pin 4 | GND | Ground |
| Pin 5 | GND | Ground |
| Pin 6 | Right motor wire 2 | Motor output |
| Pin 7 | Arduino D7 | Direction control IN2 |
| Pin 8 | Arduino VIN / external motor battery + | Motor power |

### Left Wheel Motor

| L293D Pin | Arduino / Motor Connection | Purpose |
|---:|---|---|
| Pin 9 | Arduino D9 | Enable / PWM speed control |
| Pin 10 | Arduino D12 | Direction control IN2 |
| Pin 11 | Left motor wire 1 | Motor output |
| Pin 12 | GND | Ground |
| Pin 13 | GND | Ground |
| Pin 14 | Left motor wire 2 | Motor output |
| Pin 15 | Arduino D8 | Direction control IN1 |
| Pin 16 | Arduino 5V | Logic power |

### Full First L293D Wiring Table

```text
L293D #1 pin 1  → Arduino D5
L293D #1 pin 2  → Arduino D6
L293D #1 pin 3  → Right motor wire 1
L293D #1 pin 4  → GND
L293D #1 pin 5  → GND
L293D #1 pin 6  → Right motor wire 2
L293D #1 pin 7  → Arduino D7
L293D #1 pin 8  → VIN / external motor battery +

L293D #1 pin 9  → Arduino D9
L293D #1 pin 10 → Arduino D12
L293D #1 pin 11 → Left motor wire 1
L293D #1 pin 12 → GND
L293D #1 pin 13 → GND
L293D #1 pin 14 → Left motor wire 2
L293D #1 pin 15 → Arduino D8
L293D #1 pin 16 → Arduino 5V
```

---

## 4. Second L293D: Two Lifting Motors

The second L293D controls two lifting motors for the forklift/lifting system.

The lifting system uses two motors so that both sides of the lifting mechanism can move together. Both lifting motors run at a fixed PWM speed of `70`.

### Second L293D — Lifting Motors 1 and 2

| Second L293D Pin | Arduino / Motor Connection | Purpose |
|---:|---|---|
| Pin 1 | Arduino D10 | Lift motor 1 enable / speed |
| Pin 2 | Arduino D4 | Lift motor 1 IN1 |
| Pin 3 | Lift motor 1 wire 1 | Lift motor 1 output |
| Pin 4 | GND | Ground |
| Pin 5 | GND | Ground |
| Pin 6 | Lift motor 1 wire 2 | Lift motor 1 output |
| Pin 7 | Arduino D13 | Lift motor 1 IN2 |
| Pin 8 | VIN / external battery + | Motor power |
| Pin 9 | Arduino D11 | Lift motor 2 enable / speed |
| Pin 10 | Arduino A1 | Lift motor 2 IN2 |
| Pin 11 | Lift motor 2 wire 1 | Lift motor 2 output |
| Pin 12 | GND | Ground |
| Pin 13 | GND | Ground |
| Pin 14 | Lift motor 2 wire 2 | Lift motor 2 output |
| Pin 15 | Arduino A0 | Lift motor 2 IN1 |
| Pin 16 | Arduino 5V | Logic power |

A0 and A1 are used as digital output pins in this code.

### Lift Motor 1

| Second L293D Pin | Arduino / Motor Connection | Purpose |
|---:|---|---|
| Pin 1 | Arduino D10 | Lift motor 1 enable / PWM speed |
| Pin 2 | Arduino D4 | Lift motor 1 direction IN1 |
| Pin 3 | Lift motor 1 wire 1 | Motor output |
| Pin 4 | GND | Ground |
| Pin 5 | GND | Ground |
| Pin 6 | Lift motor 1 wire 2 | Motor output |
| Pin 7 | Arduino D13 | Lift motor 1 direction IN2 |
| Pin 8 | Arduino VIN / external motor battery + | Motor power |

### Lift Motor 2

| Second L293D Pin | Arduino / Motor Connection | Purpose |
|---:|---|---|
| Pin 9 | Arduino D11 | Lift motor 2 enable / PWM speed |
| Pin 10 | Arduino A1 | Lift motor 2 direction IN2 |
| Pin 11 | Lift motor 2 wire 1 | Motor output |
| Pin 12 | GND | Ground |
| Pin 13 | GND | Ground |
| Pin 14 | Lift motor 2 wire 2 | Motor output |
| Pin 15 | Arduino A0 | Lift motor 2 direction IN1 |
| Pin 16 | Arduino 5V | Logic power |

### Full Second L293D Wiring Table

```text
L293D #2 pin 1  → Arduino D10
L293D #2 pin 2  → Arduino D4
L293D #2 pin 3  → Lift motor 1 wire 1
L293D #2 pin 4  → GND
L293D #2 pin 5  → GND
L293D #2 pin 6  → Lift motor 1 wire 2
L293D #2 pin 7  → Arduino D13
L293D #2 pin 8  → VIN / external motor battery +

L293D #2 pin 9  → Arduino D11
L293D #2 pin 10 → Arduino A1
L293D #2 pin 11 → Lift motor 2 wire 1
L293D #2 pin 12 → GND
L293D #2 pin 13 → GND
L293D #2 pin 14 → Lift motor 2 wire 2
L293D #2 pin 15 → Arduino A0
L293D #2 pin 16 → Arduino 5V
```

---

## 5. Ground Connection

All grounds must be connected together.

Required common ground connections:

```text
Arduino GND
HM-10 GND
L293D #1 GND pins
L293D #2 GND pins
External battery negative
```

Recommended breadboard setup:

```text
Arduino GND → Breadboard negative rail

HM-10 GND → Breadboard negative rail

L293D #1 pins 4, 5, 12, 13 → Breadboard negative rail
L293D #2 pins 4, 5, 12, 13 → Breadboard negative rail

Battery negative → Breadboard negative rail
```

If all grounds are not connected together, the Arduino and motor drivers may not share the same reference voltage, and the motors may not respond correctly.

---

## 6. Power Connection

Each L293D requires two different power inputs.

| L293D Pin | Connection | Purpose |
|---:|---|---|
| Pin 16 | Arduino 5V | Logic power |
| Pin 8 | Arduino VIN or external motor battery positive | Motor power |

Important:

```text
Pin 16 = logic power = 5V
Pin 8  = motor power = VIN / external battery positive
```

Do not confuse these two pins.

For both L293D ICs:

```text
Arduino 5V → L293D #1 pin 16
Arduino 5V → L293D #2 pin 16

VIN / external battery + → L293D #1 pin 8
VIN / external battery + → L293D #2 pin 8
```

If the external battery is connected through the Arduino barrel jack, the motor power can be taken from Arduino VIN:

```text
External battery → Arduino barrel jack
Arduino VIN → L293D #1 pin 8
Arduino VIN → L293D #2 pin 8
```

Do not connect the external battery positive directly to Arduino 5V.

---

## Arduino Pin Summary

| Function | Arduino Pin |
|---|---:|
| HM-10 TXD to Arduino RX | D2 |
| HM-10 RXD from Arduino TX | D3 |
| Lift motor 1 IN1 | D4 |
| Right motor enable / speed | D5 |
| Right motor IN1 | D6 |
| Right motor IN2 | D7 |
| Left motor IN1 | D8 |
| Left motor enable / speed | D9 |
| Lift motor 1 enable / speed | D10 |
| Lift motor 2 enable / speed | D11 |
| Left motor IN2 | D12 |
| Lift motor 1 IN2 | D13 |
| Lift motor 2 IN1 | A0 |
| Lift motor 2 IN2 | A1 |

---

## BLE Command Mapping

The mobile app sends single-character commands to the Arduino.

| Command | Action |
|---|---|
| A | Move forward |
| B | Turn right |
| C | Move backward |
| D | Turn left |
| E | Lift up |
| F | Lift down |
| H | Select Gear 1 |
| G | Select Gear 2 |
| S | Stop all motors |
| R | Test right wheel only |
| L | Test left wheel only |

The code also accepts lowercase versions of the same commands.

---

## App Button Mapping

The BLE controller app should be configured as follows:

| App Button | Sent Character | Robot Action |
|---|---|---|
| Up | A | Move forward |
| Right | B | Turn right |
| Down | C | Move backward |
| Left | D | Turn left |
| Triangle | E | Lift up |
| Circle | F | Lift down |
| Gear 1 button | H | Select Gear 1 |
| Gear 2 button | G | Select Gear 2 |
| Stop button | S | Stop all motors |

Optional testing buttons:

| Test Button | Sent Character | Action |
|---|---|---|
| Right wheel test | R | Run right wheel only |
| Left wheel test | L | Run left wheel only |

---

## Current Speed Settings

Current code uses variable speed values for the wheel motors.

```cpp
const int GEAR1_MIN_SPEED = 80;
const int GEAR1_MAX_SPEED = 130;

const int GEAR2_MIN_SPEED = 130;
const int GEAR2_MAX_SPEED = 180;
```

The lifting motors use a fixed speed value:

```cpp
const int LIFT_SPEED = 70;
```

The wheel speed range can be adjusted later. Example:

```cpp
const int GEAR1_MIN_SPEED = 200;
const int GEAR1_MAX_SPEED = 200;

const int GEAR2_MIN_SPEED = 200;
const int GEAR2_MAX_SPEED = 200;
```

If the minimum and maximum values are the same, the wheel motors will run at a fixed speed instead of ramping up.

The L293D Enable pins must be connected to PWM pins for speed control to work.

Current PWM speed pins:

```text
Right motor speed  → Arduino D5
Left motor speed   → Arduino D9
Lift motor 1 speed → Arduino D10
Lift motor 2 speed → Arduino D11
```

The wheel motors currently use ramped acceleration:

```cpp
const unsigned long RAMP_TIME = 3000; // ms
```

When a wheel command starts, the speed begins at the selected gear's minimum speed and increases toward the maximum speed over the ramp time. If a different wheel command is received, the ramp restarts for the new command.

The lifting motors do not use ramped acceleration. They run at the fixed speed of `70`.

---

## Auto Stop Behaviour

The robot automatically stops the wheels or lifting motors if no matching command is received within the timeout period.

Current setting:

```cpp
const unsigned long COMMAND_TIMEOUT = 200;
```

The code tracks wheel and lifting commands separately using:

```cpp
unsigned long lastWheelCommandTime = 0;
unsigned long lastLiftCommandTime = 0;

bool wheelMoving = false;
bool liftMoving = false;
```

This means the wheels and lifting motors can auto-stop independently.

If the robot keeps moving for too long after one button press, reduce this value.

Example:

```cpp
const unsigned long COMMAND_TIMEOUT = 100;
```

If the robot stops too quickly while holding a button, increase this value.

Example:

```cpp
const unsigned long COMMAND_TIMEOUT = 300;
```

The BLE controller app may also have a long-press or repeat-send setting. For smooth control, enable repeated sending while a button is held if the app supports it.

Recommended app settings:

```text
Long Press Delay: 0.10 seconds
Repeat interval: 0.10 to 0.20 seconds
COMMAND_TIMEOUT: 200 to 300 ms
```

---

## How to Upload

1. Open Arduino IDE.
2. Open `arduino/warehouse_robot_control/warehouse_robot_control.ino`.
3. Select board: `Arduino UNO`.
4. Select the correct port.
5. Click Upload.
6. Connect to the HM-10 module using the BLE controller app.
7. Send commands from the app to test the robot.

---

## Testing Procedure

Recommended test order:

1. Test HM-10 connection using BLE Terminal.
2. Send a simple character such as `A` and check Serial Monitor.
3. Test right wheel only using `R`.
4. Test left wheel only using `L`.
5. Test Gear 1 using `H`.
6. Test Gear 2 using `G`.
7. Test forward using `A`.
8. Test backward using `C`.
9. Test right turn using `B`.
10. Test left turn using `D`.
11. Test lift up using `E`.
12. Test lift down using `F`.
13. Test stop using `S`.
14. Test auto-stop behaviour by sending a movement or lift command once and checking that the motors stop after the timeout.

---

## Troubleshooting

### Motor spins at full speed regardless of speed value

Check that the L293D Enable pin is connected to a PWM pin, not fixed to 5V.

```text
Right motor:  L293D #1 pin 1 → Arduino D5
Left motor:   L293D #1 pin 9 → Arduino D9
Lift motor 1: L293D #2 pin 1 → Arduino D10
Lift motor 2: L293D #2 pin 9 → Arduino D11
```

### Motor accelerates too slowly or too quickly

Adjust the ramp time:

```cpp
const unsigned long RAMP_TIME = 3000;
```

Reduce the value for faster wheel acceleration. Increase the value for slower wheel acceleration.

### Lift motors are too fast or too slow

Adjust the fixed lift speed:

```cpp
const int LIFT_SPEED = 70;
```

Increase the value for faster lifting. Reduce the value for slower lifting.

### Motor does not move

Check:

```text
L293D pin 8  → VIN / motor power
L293D pin 16 → Arduino 5V
L293D GND pins → Arduino GND
Motor wires → correct L293D output pins
Enable pin → correct Arduino PWM pin
Direction pins → correct Arduino pins
```

### Bluetooth connects but commands do not control motors

Check:

```text
HM-10 TXD → Arduino D2
HM-10 RXD → Arduino D3
Arduino Serial Monitor baud rate = 9600
BLE app sends ASCII characters
```

### Robot stops too quickly while holding a button

Check whether the BLE app repeatedly sends the command while the button is held. If it does not, the Arduino will stop the motor after `COMMAND_TIMEOUT`.

Possible fixes:

```text
Enable repeat-send or long-press repeat in the app
Increase COMMAND_TIMEOUT to 300 ms
Check that the app sends plain ASCII characters such as A, B, C, D, E, F, G, H, S, R, L
```

### Robot moves backward when pressing forward

Swap the motor output wires or reverse the HIGH/LOW logic in the motor control function.

For example, swap:

```text
Right motor wire 1 ↔ Right motor wire 2
```

or edit the `setRightMotor()` / `setLeftMotor()` functions in the Arduino code.

### Lift moves down when pressing lift up

If `E` makes one of the lifting motors move in the wrong direction, swap that motor's two output wires.

```text
Lift motor 1 wrong direction → swap L293D #2 pin 3 and pin 6 motor wires
Lift motor 2 wrong direction → swap L293D #2 pin 11 and pin 14 motor wires
```

Alternatively, reverse the direction logic in the `setLiftMotor1()` or `setLiftMotor2()` function.

---

## Safety Notes

- Do not power DC motors directly from Arduino output pins.
- Use motor driver ICs such as L293D.
- Use an external battery for motor power.
- Connect all grounds together.
- Check wiring before powering the circuit.
- Disconnect power before changing wires.
- Avoid short-circuiting the breadboard power rails.
- If the L293D becomes very hot, disconnect power and check wiring.
- Do not connect external battery positive directly to Arduino 5V.
- Make sure the lifting mechanism cannot jam before running both lifting motors together.

---

## Current Status

- HM-10 Bluetooth connection tested.
- BLE terminal communication confirmed.
- Single motor control through L293D tested.
- First L293D assigned to right and left wheel motors.
- Second L293D assigned to two lifting motors.
- Gear 1 and Gear 2 speed ranges added for wheel motors.
- Ramped wheel acceleration added.
- Fixed lift speed set to 70.
- Auto-stop behaviour added for both wheel and lift commands.
- PWM speed control is still being tuned.
- Final wiring and physical robot testing are not yet complete.
