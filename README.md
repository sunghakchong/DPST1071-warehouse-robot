# Warehouse Robot Arduino Control

This repository contains the Arduino control code for our DPST1071 warehouse robot project.

The robot is controlled through an iPhone BLE controller app using an HM-10 Bluetooth module. The Arduino receives single-character commands and controls the right wheel motor, left wheel motor, and forklift motor through L293D motor driver ICs.

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

## Pin Mapping

### Bluetooth Module

| HM-10 Pin | Arduino Pin |
|---|---|
| TXD | D2 |
| RXD | D3 |
| VCC | 5V or 3.3V depending on module |
| GND | GND |

Note: HM-10 RXD should use a voltage divider when connected to Arduino TX because Arduino UNO uses 5V logic.

### Motor Control Pins

| Function | Arduino Pins |
|---|---|
| Right wheel motor | D6, D7 |
| Left wheel motor | D8, D9 |
| Forklift motor | D10, D11 |

Each DC motor requires two control pins because the L293D uses an H-bridge to control direction.

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
| H | Stop all motors |

The code also accepts lowercase versions of the same commands.

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
| Square | H | Stop all |

## Safety Notes

- Do not power DC motors directly from Arduino output pins.
- Use motor driver ICs such as L293D.
- Use an external battery for motor power.
- Connect Arduino GND, L293D GND, and battery negative together as common ground.
- Check wiring before powering the circuit.

## How to Upload

1. Open Arduino IDE.
2. Open `arduino/warehouse_robot_control/warehouse_robot_control.ino`.
3. Select board: `Arduino UNO`.
4. Select the correct port.
5. Click Upload.
6. Connect to the HM-10 module using the BLE controller app.
7. Send commands from the app to test the robot.

## Current Status

- HM-10 Bluetooth connection tested.
- BLE terminal communication confirmed.
- Motor control code prepared for 3 motors using L293D motor drivers.
- Final wiring and physical robot testing are still in progress.