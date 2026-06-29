# Warehouse Robot Arduino Control

This repository contains the Arduino control code for our DPST1071 warehouse robot project.

The robot is controlled through an iPhone BLE controller app using an HM-10 Bluetooth module. The Arduino receives single-character commands and controls the right wheel motor, left wheel motor, and forklift motor through L293D motor driver ICs.

> **Project Status:** This code is currently under development. The Bluetooth connection and basic motor control have been tested, but the final wiring, speed tuning, and full robot integration are still in progress.

## Hardware Overview

### Main Components

- Arduino UNO
- HM-10 BLE Bluetooth module
- 2 × L293D Dual Full Bridge Motor Driver IC
- 3 × DC motors
  - Right wheel motor
  - Left wheel motor
  - Forklift motor
- External motor battery
- Breadboard and jumper wires
- Resistors for HM-10 voltage divider

## System Overview

The control flow is:

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

The HM-10 only receives commands from the phone and sends them to the Arduino. The Arduino processes the commands and controls the motor drivers. The L293D motor drivers provide the required current and direction control for the DC motors.

## Pin Mapping

### HM-10 Bluetooth Module

| HM-10 Pin | Arduino Pin |
|---|---|
| TXD | D2 |
| RXD | D3 |
| VCC | 5V or 3.3V depending on module |
| GND | GND |

Note: HM-10 RXD should use a voltage divider when connected to Arduino TX because Arduino UNO uses 5V logic.

Recommended voltage divider:

```text
Arduino D3 ── 1kΩ ──┬── HM-10 RXD
                    |
                   2kΩ
                    |
                   GND
```

## Motor Control Pins

The L293D Enable pins must be connected to PWM-capable Arduino pins for speed control.

### Right Wheel Motor

| Function | Arduino Pin | L293D Pin |
|---|---:|---:|
| Speed control / Enable | D5 | Pin 1 |
| Direction IN1 | D6 | Pin 2 |
| Direction IN2 | D7 | Pin 7 |
| Motor output 1 | - | Pin 3 |
| Motor output 2 | - | Pin 6 |

### Left Wheel Motor

| Function | Arduino Pin | L293D Pin |
|---|---:|---:|
| Speed control / Enable | D9 | Pin 9 |
| Direction IN1 | D8 | Pin 15 |
| Direction IN2 | D12 | Pin 10 |
| Motor output 1 | - | Pin 11 |
| Motor output 2 | - | Pin 14 |

### Forklift Motor

The forklift motor uses the second L293D motor driver.

| Function | Arduino Pin | L293D Pin |
|---|---:|---:|
| Speed control / Enable | D10 | Pin 1 |
| Direction IN1 | D11 | Pin 2 |
| Direction IN2 | D13 | Pin 7 |
| Motor output 1 | - | Pin 3 |
| Motor output 2 | - | Pin 6 |

## L293D Power Connections

Each L293D requires both logic power and motor power.

| L293D Pin | Connection |
|---:|---|
| Pin 16 | Arduino 5V |
| Pin 8 | External motor power / Arduino VIN |
| Pin 4 | GND |
| Pin 5 | GND |
| Pin 12 | GND |
| Pin 13 | GND |

Important:

- Arduino GND, HM-10 GND, L293D GND, and battery negative must share common ground.
- Do not connect motors directly to Arduino output pins.
- Do not power DC motors directly from Arduino digital pins.
- If using Arduino barrel jack for external battery input, motor power can be taken from Arduino VIN.
- The L293D Enable pins must not be fixed to 5V if speed control is required. They must be connected to PWM pins.

## BLE Command Mapping

The mobile app sends single-character commands to the Arduino.

| Command | Action |
|---|---|
| A | Move forward |
| C | Move backward |
| D | Turn left |
| B | Turn right |
| E | Forklift up |
| F | Forklift down |
| H | Select Gear 1 |
| G | Select Gear 2 |
| S | Stop all motors |

The code also accepts lowercase versions of the same commands.

## Gear / Speed Control

The robot uses two speed modes.

| Command | Gear | Speed Range |
|---|---|---|
| H | Gear 1 | 50 to 120 |
| G | Gear 2 | 120 to 200 |

The robot starts from the selected minimum speed and gradually increases to the selected maximum speed while the same movement command is repeatedly received.

Current ramp setting:

```text
RAMP_TIME = 5000 ms
```

This means the robot takes approximately 5 seconds to accelerate from the selected minimum speed to the selected maximum speed.

## App Button Mapping

The BLE controller app should be configured as follows:

| App Button | Sent Character | Robot Action |
|---|---|---|
| Up | A | Move forward |
| Down | C | Move backward |
| Left | D | Turn left |
| Right | B | Turn right |
| Triangle | E | Forklift up |
| Circle | F | Forklift down |
| Gear 1 button | H | Select Gear 1 |
| Gear 2 button | G | Select Gear 2 |
| Stop button | S | Stop all motors |

If the app supports repeated sending while a button is held, enable it for movement buttons. This allows smoother control and acceleration.

Recommended repeat interval:

```text
100 ms to 200 ms
```

## Current Code Behaviour

- Movement commands:
  - `A`, `C`, `D`, `B`
- Forklift commands:
  - `E`, `F`
- Gear commands:
  - `H`, `G`
- Emergency stop:
  - `S`

The robot automatically stops the wheel motors if no wheel command is received within:

```text
COMMAND_TIMEOUT = 300 ms
```

The forklift motor also stops automatically if no forklift command is received within the same timeout period.

## How to Upload

1. Open Arduino IDE.
2. Open `arduino/warehouse_robot_control/warehouse_robot_control.ino`.
3. Select board: `Arduino UNO`.
4. Select the correct port.
5. Click Upload.
6. Connect to the HM-10 module using the BLE controller app.
7. Send commands from the app to test the robot.

## Safety Notes

- Do not power DC motors directly from Arduino output pins.
- Use motor driver ICs such as L293D.
- Use an external battery for motor power.
- Connect all grounds together:
  - Arduino GND
  - HM-10 GND
  - L293D GND
  - Battery negative
- Check wiring carefully before powering the circuit.
- If the motor spins at full speed regardless of the speed value, check whether the L293D Enable pin is incorrectly connected to 5V instead of a PWM pin.

## Current Status

- HM-10 Bluetooth connection tested.
- BLE terminal communication confirmed.
- Single motor control through L293D tested.
- PWM speed control is being tested.
- Two-wheel driving and forklift integration are still in progress.
- Final wiring and physical robot testing are not yet complete.
