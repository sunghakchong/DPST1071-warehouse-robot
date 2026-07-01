# Warehouse Robot Arduino Control

This repository contains the Arduino control code and wiring documentation for our DPST1071 warehouse robot project.

The robot is controlled through an iPhone BLE controller app using an HM-10 Bluetooth module. The Arduino UNO receives single-character commands through Bluetooth and controls the right wheel motor, left wheel motor, and forklift motor using L293D motor driver ICs.

> **Project Status:** This project is still in progress. Bluetooth communication and basic motor control have been tested. Two-wheel control, speed tuning, forklift control, and final robot integration are still being developed and tested.

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
L293D Motor Driver ICs
        ↓
DC Motors
```

The HM-10 Bluetooth module does not control the motors directly. It only sends commands from the phone to the Arduino. The Arduino reads these commands and sends control signals to the L293D motor driver. The L293D then controls the direction and power of each DC motor.

---

## Hardware Components

- Arduino UNO
- HM-10 BLE Bluetooth module
- 2 × L293D Dual Full Bridge Motor Driver IC
- 3 × DC motors
  - Right wheel motor
  - Left wheel motor
  - Forklift motor
- External motor battery
- Breadboard
- Jumper wires
- Resistors for HM-10 RXD voltage divider

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

## 2. Arduino ↔ L293D Motor Driver IC

The L293D is used because Arduino pins cannot safely power DC motors directly. The Arduino only sends control signals, while the L293D handles the motor current and direction control.

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
| Pin 8 | Arduino VIN | Motor power |

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
L293D pin 1  → Arduino D5
L293D pin 2  → Arduino D6
L293D pin 3  → Right motor wire 1
L293D pin 4  → GND
L293D pin 5  → GND
L293D pin 6  → Right motor wire 2
L293D pin 7  → Arduino D7
L293D pin 8  → Arduino VIN

L293D pin 9  → Arduino D9
L293D pin 10 → Arduino D12
L293D pin 11 → Left motor wire 1
L293D pin 12 → GND
L293D pin 13 → GND
L293D pin 14 → Left motor wire 2
L293D pin 15 → Arduino D8
L293D pin 16 → Arduino 5V
```

---

## 4. Second L293D: Forklift Motor

The second L293D is used for the forklift motor.

| Second L293D Pin | Arduino / Motor Connection | Purpose |
|---:|---|---|
| Pin 1 | Arduino D10 | Enable / PWM speed control |
| Pin 2 | Arduino D11 | Direction control IN1 |
| Pin 3 | Forklift motor wire 1 | Motor output |
| Pin 4 | GND | Ground |
| Pin 5 | GND | Ground |
| Pin 6 | Forklift motor wire 2 | Motor output |
| Pin 7 | Arduino D13 | Direction control IN2 |
| Pin 8 | Arduino VIN | Motor power |
| Pin 16 | Arduino 5V | Logic power |

The unused side of the second L293D can remain unused, but its GND pins should still be connected to common ground.

---

## 5. Ground Connection

All grounds must be connected together.

Required common ground connections:

```text
Arduino GND
HM-10 GND
L293D GND pins
External battery negative
```

If using the two GND pins on Arduino UNO, they can be divided like this:

```text
Arduino GND 1 → L293D motor driver GND
Arduino GND 2 → HM-10 Bluetooth module GND
```

However, both Arduino GND pins are internally connected. The important point is that all parts share the same ground reference.

Recommended breadboard setup:

```text
Arduino GND → Breadboard negative rail
HM-10 GND → Breadboard negative rail
L293D pins 4, 5, 12, 13 → Breadboard negative rail
Battery negative → Breadboard negative rail
```

---

## 6. Power Connection

The L293D requires two different power inputs.

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

If the external battery is connected through the Arduino barrel jack, the motor power can be taken from Arduino VIN:

```text
External battery → Arduino barrel jack
Arduino VIN → L293D pin 8
```

Do not connect the external battery positive directly to Arduino 5V.

---

## Arduino Pin Summary

| Function | Arduino Pin |
|---|---:|
| HM-10 TXD to Arduino RX | D2 |
| HM-10 RXD from Arduino TX | D3 |
| Right motor enable / speed | D5 |
| Right motor IN1 | D6 |
| Right motor IN2 | D7 |
| Left motor IN1 | D8 |
| Left motor enable / speed | D9 |
| Forklift motor enable / speed | D10 |
| Forklift motor IN1 | D11 |
| Left motor IN2 | D12 |
| Forklift motor IN2 | D13 |

---

## BLE Command Mapping

The mobile app sends single-character commands to the Arduino.

| Command | Action |
|---|---|
| A | Move forward |
| B | Turn right |
| C | Move backward |
| D | Turn left |
| E | Forklift up |
| F | Forklift down |
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
| Triangle | E | Forklift up |
| Circle | F | Forklift down |
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

Current code uses variable speed values for testing.

```cpp
const int GEAR1_MIN_SPEED = 80;
const int GEAR1_MAX_SPEED = 130;

const int GEAR2_MIN_SPEED = 130;
const int GEAR2_MAX_SPEED = 180;
```

The speed range can be adjusted later. Example:

```cpp
const int GEAR1_MIN_SPEED = 200;
const int GEAR1_MAX_SPEED = 200;

const int GEAR2_MIN_SPEED = 200;
const int GEAR2_MAX_SPEED = 200;
```
This means both gear modes currently run at the same speed.

The L293D Enable pins must be connected to PWM pins for speed control to work.

Current PWM speed pins:

```text
Right motor speed → Arduino D5
Left motor speed  → Arduino D9
Forklift speed    → Arduino D10
```

---

## Auto Stop Behaviour

The robot automatically stops if no command is received within the timeout period.

Current setting:

```cpp
const unsigned long COMMAND_TIMEOUT = 200;
```

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
5. Test forward using `A`.
6. Test backward using `C`.
7. Test right turn using `B`.
8. Test left turn using `D`.
9. Test stop using `S`.
10. Test forklift up/down using `E` and `F`.

---

## Troubleshooting

### Motor spins at full speed regardless of speed value

Check that the L293D Enable pin is connected to a PWM pin, not fixed to 5V.

```text
Right motor: L293D pin 1 → Arduino D5
Left motor:  L293D pin 9 → Arduino D9
```

### Motor does not move

Check:

```text
L293D pin 8  → VIN / motor power
L293D pin 16 → Arduino 5V
L293D GND pins → Arduino GND
Motor wires → correct L293D output pins
```

### Bluetooth connects but commands do not control motors

Check:

```text
HM-10 TXD → Arduino D2
HM-10 RXD → Arduino D3
Arduino Serial Monitor baud rate = 9600
BLE app sends ASCII characters
```

### Robot moves backward when pressing forward

Swap the motor output wires or reverse the HIGH/LOW logic in the motor control function.

For example, swap:

```text
Right motor wire 1 ↔ Right motor wire 2
```

or edit the `setRightMotor()` / `setLeftMotor()` functions in the Arduino code.

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

---

## Current Status

- HM-10 Bluetooth connection tested.
- BLE terminal communication confirmed.
- Single motor control through L293D tested.
- Right and left wheel control is being tested.
- PWM speed control is being tuned.
- Forklift motor integration is still in progress.
- Final wiring and physical robot testing are not yet complete.
