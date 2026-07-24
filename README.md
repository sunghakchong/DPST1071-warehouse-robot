# Warehouse Robot Arduino Control

This repository contains the Arduino control code and wiring documentation for the DPST1071 warehouse robot project.

The robot is controlled from an iPhone BLE controller app through an HM-10 Bluetooth module. An Arduino UNO receives single-character commands and controls four TT DC motors through two L298N dual H-bridge motor driver modules.

- L298N #1 controls the right and left wheel motors.
- L298N #2 controls the two lifting motors.
- Wheel speeds are calibrated independently to compensate for the weaker right-side drive channel.
- Lift motor 1 `IN2` is connected to Arduino `A2` instead of `D13`.

> **Current status:** The code includes BLE control, two driving gears, independent right/left speed calibration, acceleration ramps, separate lift-up/down speeds, and automatic wheel/lift stopping. Final speed values can still be adjusted after physical testing.

---

## System Overview

```text
iPhone BLE Controller App
        ↓
HM-10 Bluetooth Module
        ↓
Arduino UNO
        ↓
L298N #1 ── Right and left wheel motors
L298N #2 ── Lift motors 1 and 2
```

The HM-10 only sends commands. The Arduino interprets each command and sends PWM speed and digital direction signals to the L298N modules. The L298N modules supply the motor current and reverse motor polarity when the direction changes.

---

## Hardware Components

- Arduino UNO
- HM-10 BLE Bluetooth module
- 2 × Jaycar/Duinotech XC4492 L298N dual motor driver modules
- 4 × TT DC geared motors
  - Right wheel motor
  - Left wheel motor
  - Lift motor 1
  - Lift motor 2
- Existing external motor power supply
- Breadboard
- Jumper wires
- 1 kΩ and 2 kΩ resistors for the HM-10 RX voltage divider

---

## Important L298N Jumper Configuration

For both L298N modules:

```text
ENA jumper  → Remove
ENB jumper  → Remove
5VEN jumper → Remove
```

The `ENA` and `ENB` jumpers must be removed because speed is controlled by Arduino PWM pins.

The `5VEN` jumper must be removed when the L298N logic `5V` terminal is supplied from the Arduino `5V` pin.

Make sure both driver boards use the same jumper configuration. If only one enable jumper remains installed, one motor channel may run at full power while the other channel uses the lower PWM value.

---

## HM-10 Bluetooth Wiring

| HM-10 pin | Arduino connection | Purpose |
|---|---|---|
| VCC | Existing HM-10 power connection | Module power |
| GND | GND | Common ground |
| TXD | D2 | HM-10 transmits to Arduino RX |
| RXD | D3 through voltage divider | Arduino transmits to HM-10 RX |

The code uses:

```cpp
SoftwareSerial hm10(2, 3);
```

Therefore:

```text
HM-10 TXD → Arduino D2
Arduino D3 → voltage divider → HM-10 RXD
```

Recommended voltage divider:

```text
Arduino D3 ── 1 kΩ ──┬── HM-10 RXD
                     |
                    2 kΩ
                     |
                    GND
```

---

## L298N Module Connections

Each L298N module controls two motors:

```text
Channel A: ENA + IN1 + IN2 → Motor A
Channel B: ENB + IN3 + IN4 → Motor B
```

The `ENA` and `ENB` pins control speed using PWM. The `IN1–IN4` pins control direction.

### L298N #1 — Wheel Motors

| L298N #1 connection | Arduino/motor connection | Purpose |
|---|---|---|
| ENA | D5 | Right motor PWM speed |
| IN1 | D6 | Right motor direction 1 |
| IN2 | D7 | Right motor direction 2 |
| Motor A | Right wheel motor | Right motor output |
| IN3 | D8 | Left motor direction 1 |
| IN4 | D12 | Left motor direction 2 |
| ENB | D9 | Left motor PWM speed |
| Motor B | Left wheel motor | Left motor output |
| 5V | Arduino 5V | Driver logic power |
| GND | Common GND | Shared ground |
| VMS | Existing motor power positive | Motor power input |

Full signal wiring:

```text
Arduino D5  → L298N #1 ENA
Arduino D6  → L298N #1 IN1
Arduino D7  → L298N #1 IN2
Arduino D8  → L298N #1 IN3
Arduino D12 → L298N #1 IN4
Arduino D9  → L298N #1 ENB

L298N #1 Motor A → Right wheel motor
L298N #1 Motor B → Left wheel motor
```

### L298N #2 — Lifting Motors

| L298N #2 connection | Arduino/motor connection | Purpose |
|---|---|---|
| ENA | D10 | Lift motor 1 PWM speed |
| IN1 | D4 | Lift motor 1 direction 1 |
| IN2 | **A2** | Lift motor 1 direction 2 |
| Motor A | Lift motor 1 | Lift motor 1 output |
| IN3 | A0 | Lift motor 2 direction 1 |
| IN4 | A1 | Lift motor 2 direction 2 |
| ENB | D11 | Lift motor 2 PWM speed |
| Motor B | Lift motor 2 | Lift motor 2 output |
| 5V | Arduino 5V | Driver logic power |
| GND | Common GND | Shared ground |
| VMS | Existing motor power positive | Motor power input |

Full signal wiring:

```text
Arduino D10 → L298N #2 ENA
Arduino D4  → L298N #2 IN1
Arduino A2  → L298N #2 IN2
Arduino A0  → L298N #2 IN3
Arduino A1  → L298N #2 IN4
Arduino D11 → L298N #2 ENB

L298N #2 Motor A → Lift motor 1
L298N #2 Motor B → Lift motor 2
```

`A0`, `A1`, and `A2` are used as digital output pins. The code still controls them with `pinMode(..., OUTPUT)` and `digitalWrite()`.

---

## Power and Ground Connections

The blue screw terminal block on each L298N contains:

```text
VMS → Motor power positive
GND → Common ground
5V  → Arduino 5V logic power
```

For both modules:

```text
Existing motor power + → L298N #1 VMS
Existing motor power + → L298N #2 VMS

Motor power - → Common GND
Arduino GND   → Common GND
HM-10 GND     → Common GND
L298N #1 GND  → Common GND
L298N #2 GND  → Common GND

Arduino 5V → L298N #1 5V
Arduino 5V → L298N #2 5V
```

The screw-terminal `5V/GND` connections and pin-header `5V/GND` connections are electrically shared. This build uses the screw terminals, so the duplicate header pins do not need additional wires.

Do not connect `VMS` to the Arduino `5V` pin. `VMS` is the motor power input; `5V` is the L298N logic supply.

---

## Arduino Pin Summary

| Function | Arduino pin |
|---|---|
| HM-10 TXD → Arduino RX | D2 |
| Arduino TX → HM-10 RXD | D3 |
| Lift motor 1 IN1 | D4 |
| Right motor ENA/PWM | D5 |
| Right motor IN1 | D6 |
| Right motor IN2 | D7 |
| Left motor IN3 | D8 |
| Left motor ENB/PWM | D9 |
| Lift motor 1 ENA/PWM | D10 |
| Lift motor 2 ENB/PWM | D11 |
| Left motor IN4 | D12 |
| Lift motor 2 IN3 | A0 |
| Lift motor 2 IN4 | A1 |
| Lift motor 1 IN2 | **A2** |

---

## BLE Command Mapping

| Command | Action |
|---|---|
| `A` | Move forward |
| `B` | Turn right |
| `C` | Move backward |
| `D` | Turn left |
| `E` | Lift up |
| `F` | Lift down |
| `H` | Select Gear 1 |
| `G` | Select Gear 2 |
| `S` | Stop all motors |
| `R` | Test right wheel only |
| `L` | Test left wheel only |

Lowercase commands are converted to uppercase by the code.

Suggested app mapping:

| App button | Character |
|---|---|
| Up | `A` |
| Right | `B` |
| Down | `C` |
| Left | `D` |
| Lift up | `E` |
| Lift down | `F` |
| Gear 1 | `H` |
| Gear 2 | `G` |
| Stop | `S` |

---

## Current Speed Settings

PWM values use the Arduino range `0–255`.

### Right Wheel

```cpp
const int RIGHT_GEAR1_MIN_SPEED = 90;
const int RIGHT_GEAR1_MAX_SPEED = 90;

const int RIGHT_GEAR2_MIN_SPEED = 100;
const int RIGHT_GEAR2_MAX_SPEED = 120;

const unsigned long RIGHT_RAMP_TIME = 2000;
```

### Left Wheel

```cpp
const int LEFT_GEAR1_MIN_SPEED = 80;
const int LEFT_GEAR1_MAX_SPEED = 80;

const int LEFT_GEAR2_MIN_SPEED = 90;
const int LEFT_GEAR2_MAX_SPEED = 110;

const unsigned long LEFT_RAMP_TIME = 2000;
```

The right wheel currently receives a slightly higher PWM value to compensate for the weaker Motor A/ENA drive path.

### Lifting Motors

```cpp
const int LIFT_UP_SPEED = 50;
const int LIFT_DOWN_SPEED = 130;
```

Lift-up and lift-down speeds are separate because lifting requires different torque and control from lowering.

---

## Automatic Stop Settings

```cpp
const unsigned long GEAR1_COMMAND_TIMEOUT = 150; // ms
const unsigned long GEAR2_COMMAND_TIMEOUT = 330; // ms
const unsigned long LIFT_COMMAND_TIMEOUT = 80;   // ms
```

The phone app must repeatedly send the active command while a button is held. If the Arduino does not receive another matching command before the relevant timeout, it stops that motor group.

```text
Shorter timeout → faster stop after button release
Longer timeout  → smoother holding but slower stop
```

---

## Upload Procedure

1. Open the `.ino` file in Arduino IDE.
2. Select **Arduino UNO**.
3. Select the correct serial port.
4. Compile the sketch.
5. Upload it to the Arduino.
6. Open Serial Monitor at `9600` baud if debugging is required.
7. Connect the phone to the HM-10 using the BLE controller app.

---

## Testing Procedure

Test with the robot lifted so the wheels and lift mechanism can move without load:

1. Send `R` to test only the right wheel.
2. Send `L` to test only the left wheel.
3. Send `H`, then test `A`, `B`, `C`, and `D` in Gear 1.
4. Send `G`, then test `A`, `B`, `C`, and `D` in Gear 2.
5. Send `E` to test lift up.
6. Send `F` to test lift down.
7. Send `S` to stop all motors.
8. Place the robot on the ground and check straight-line movement.
9. Adjust right/left speed values in increments of approximately 5 if required.

---

## Troubleshooting

### One wheel is weaker than the other

The current code compensates by using a higher PWM value for the right wheel.

```cpp
RIGHT_GEAR1_MIN_SPEED = 90;
LEFT_GEAR1_MIN_SPEED  = 80;
```

If the weaker side changes when the two motor output connections are exchanged, the difference follows the L298N channel rather than the motor. Check that:

- Both `ENA` and `ENB` jumpers are removed.
- Both PWM wires are firmly connected.
- Both motor screw terminals are tight.
- Both channels share the same power and ground.

### Lift motor 1 goes up but does not go down

For lift motor 1:

```text
Arduino D4 → L298N #2 IN1 / L1
Arduino A2 → L298N #2 IN2 / L2
```

When lifting up, `L1` and `L3` should normally illuminate. When lifting down, `L2` and `L4` should normally illuminate.

If `L4` illuminates but `L2` does not:

- Check the `A2 → IN2` jumper wire.
- Confirm the uploaded code contains `const int LIFT1_IN2 = A2;`.
- Check the L298N #2 IN2 connection.

### Motor direction is reversed

Turn the power off and exchange the two wires of only the affected motor at the `Motor A` or `Motor B` screw terminal.

Do not change the power or ground wires to reverse a motor.

### Motor runs at full speed

Check that the corresponding `ENA` or `ENB` jumper has been removed and that the enable pin is connected to the correct Arduino PWM pin.

### Bluetooth connects but commands do not work

Check:

```text
HM-10 TXD → Arduino D2
Arduino D3 → voltage divider → HM-10 RXD
Baud rate  → 9600
All grounds connected together
```

### Robot stops while holding a button

The BLE app must repeatedly send the command. Increase the relevant timeout slightly only if the app repeat interval is longer than the current timeout.

---

## Safety Notes

- Disconnect power before changing motor, signal, or screw-terminal wiring.
- Never power a DC motor directly from an Arduino output pin.
- Keep motor wiring clear of moving gears and lifting components.
- Tighten all L298N screw terminals before testing.
- Stop immediately if a driver, motor, battery, or wire becomes unusually hot.
- Do not hold the lift against its mechanical end stop because stalled motors draw high current.
- Confirm that the motor supply voltage is appropriate for the installed TT motors.

---

## Current Project Configuration

- HM-10 BLE communication through Arduino D2/D3.
- Two L298N modules replacing the previous two L293D ICs.
- L298N #1 assigned to the right and left wheel motors.
- L298N #2 assigned to two lifting motors.
- Lift motor 1 `IN2` moved from D13 to A2.
- Independent right/left wheel speed calibration enabled.
- Right wheel PWM increased to compensate for the weaker drive path.
- Separate Gear 1 and Gear 2 speed and timeout settings.
- Separate lift-up, lift-down, and lift timeout settings.
- Automatic wheel and lift stopping enabled.
