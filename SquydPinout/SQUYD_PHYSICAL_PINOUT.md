# SQUYD Bioprinter — Physical Pinout & Wiring Reference
**Board:** BTT Octopus PRO V1.0 | **MCU:** STM32F446ZET6
**Last updated:** 2026-04-13

---

## 1. MOTOR CONNECTORS

Physical slot order on board: MOTOR0 → MOTOR1 → MOTOR2 → MOTOR3 → MOTOR4 → MOTOR5 → MOTOR6 → MOTOR7

| Board Slot | Firmware Axis | Function | STEP | DIR | EN | TMC UART |
|---|---|---|---|---|---|---|
| MOTOR 0 | X | X Gantry | PF13 | PF12 | PF14 | PC4 |
| MOTOR 1 | Z | Z Gantry (swapped from slot 2) | PG0 | PG1 | PF15 | PD11 |
| MOTOR 2 | Y | Y Gantry (swapped from slot 1) | PF11 | PG3 | PG5 | PC6 |
| MOTOR 3 | E0 | Syringe extruder | PG4 | PC1 | PA0 | PC7 |
| MOTOR 4 | E1 | **Pneumatic extruder** (A4988, no TMC) | PF9 | PF10 | PC3 | PF2 |
| MOTOR 5 | — | Unused | PC13 | PF0 | PF1 | PE4 |
| MOTOR 6 | I | Printhead Z height 1 (TMC2209) | PE2 | PE3 | PD4 | PE1 |
| MOTOR 7 | J | Printhead Z height 2 (TMC2209) | PE6 | PA14 | PE0 | PD3 |

### Notes
- **Y and Z are physically swapped** from the default Octopus layout (firmware compensates)
- **E1 (Motor 4) = Pneumatic extruder**: step pulses on PF9 trigger valve strokes; EN pin PC3 enables/disables driver
- **Motor 5 is unused** — connector unpopulated or driver not installed
- **I and J axes** use the Motor 6 / Motor 7 slots (physically labelled as E2/E3 on some board silkscreens)
- Y2 dual-motor is **not configured** in firmware — if two Y motors are used, they must be wired in parallel/series to the Motor 2 driver output

---

## 2. ENDSTOP CONNECTORS

| Motor / Axis | Endstop GPIO | Board Header | Position |
|---|---|---|---|
| X | PG6 | X-STOP | X MIN |
| Y | PG9 | Y-STOP | Y MIN |
| Z | PG10 | Z-STOP | Z MIN |
| I (Printhead Z1) | PG14 | J32 | I MIN |
| J (Printhead Z2) | PG15 | J34 | J MIN |

> No endstop on E0 (syringe) or E1 (pneumatic) — software limits only.

---

## 3. THERMISTOR CONNECTORS

| Firmware Sensor | GPIO | Board Header | Physical connection |
|---|---|---|---|
| TEMP_SENSOR_0 (T0) | PF4 | TH0 | Extruder 0 thermistor (E0 / Syringe) |
| TEMP_SENSOR_1 (T1) | PF3 | TH1 / J44 | Extruder 1 thermistor (E1 / Pneumatic) |
| TEMP_SENSOR_BED | PF5 | TB / J46 | Bed thermistor |
| TEMP_SENSOR_CHAMBER | PF6 | J48 | Chamber thermistor |
| TEMP_3 (unused) | PF8 | TH3 | Not connected |

> T2 (PF4 on TH2/J45) is disabled (TEMP_SENSOR_2 = 0)

---

## 4. PELTIER POWER CONNECTORS (Heater outputs — PWM via PID)

These drive the Peltier modules — power level controlled by Marlin thermal PID loop.

| Board Header | GPIO | Marlin Heater | Physical Peltier | M-code to set temp |
|---|---|---|---|---|
| HE0 | PA2 | HEATER_0 | Peltier Extruder 0 (Syringe) | `M104 S35` |
| HE1 | PA3 | HEATER_1 | Peltier Extruder 1 (Pneumatic) | `M104 T1 S35` |
| HE2 | PB10 | HEATER_BED | Peltier Bed | `M140 S35` |
| HE3 | PB11 | — | **Unused** | — |
| HB  | PA1 | — | **Unused** (bed remapped to HE2) | — |

---

## 5. PELTIER DIRECTION CONNECTORS (DPDT Relay — M42 direct pin control)

These switch Peltier polarity between heating and cooling via DPDT relay.

| Board Header | GPIO | Marlin Pin | Controls | S255 | S0 | Startup |
|---|---|---|---|---|---|---|
| FAN2 | PD12 | P60 | Peltier E0 DPDT | Heating | Cooling | HIGH (heating) |
| FAN3 | PD13 | P61 | Peltier E1 DPDT | Heating | Cooling | HIGH (heating) |
| FAN4 | PD14 | P62 | Peltier Bed DPDT | Heating | Cooling | HIGH (heating) |

> Active-LOW relay: writing HIGH = relay off = heating mode. Writing LOW = relay on = cooling mode.

---

## 6. UV LED CONNECTORS (PWM — M42 direct pin control)

| Board Header | GPIO | Marlin Pin | Physical | S0 | S127 | S255 |
|---|---|---|---|---|---|---|
| FAN0 | PA8 | P8 | UV LED Array 1 | OFF | 50% | 100% |
| FAN1 | PE5 | P69 | UV LED Array 2 | OFF | 50% | 100% |

> Both LEDs always controlled together via menu. PWM capable — variable intensity.

---

## 7. HEPA FAN CONNECTOR (M42 relay switch)

| Board Header | GPIO | Marlin Pin | Relay Type | S255 | S0 | Startup |
|---|---|---|---|---|---|---|
| FAN5 | PD15 | P63 | Active-HIGH | Fan ON | Fan OFF | LOW (off) |

> Physical: 2× 12V/1.4A fans in parallel through relay contacts. Relay coil from external 12V.
> Flyback diode 1N4007 across coil mandatory.

---

## 8. CHAMBER LIGHT CONNECTOR (M42 digital switch)

| Board Header | GPIO | Marlin Pin | S255 | S0 | Startup |
|---|---|---|---|---|---|
| J43 BL_Touch (signal pin) | PB6 | P22 | Light ON | Light OFF | LOW (off) |

> Use PB6 (signal) + any GND pin from J43.
> Physical: PB6 → MOSFET gate → DPDT relay coil (12V) → relay contacts → 240V AC light.
> Flyback diode 1N4007 across coil mandatory.

---

## 9. SERIAL / DISPLAY CONNECTORS

| Connector | GPIO | Baud | Connected to |
|---|---|---|---|
| USB-C | PA11 / PA12 | 115200 | PC (Pronterface / PlatformIO) |
| USART1 (TFT header) | PA9 (TX) / PA10 (RX) | 250000 | BTT TFT70 V3.0 display |

---

## 10. POWER CONNECTORS

| Connector | Voltage | Powers |
|---|---|---|
| MOTOR POWER | 24V | All motor drivers |
| BED POWER (HB) | 24V input | HB output (unused — bed remapped to HE2) |
| HE0–HE3 power | 24V (from main VIN) | Peltier MOSFETs |
| J38 V_FUSED | 12V + 5V | Fused auxiliary outputs (use for relay coils) |

---

## 11. FREE / SPARE CONNECTORS

| Connector | GPIO | Marlin Pin | Status |
|---|---|---|---|
| HE3 | PB11 | P27 | Fully free — no firmware use |
| HB | PA1 | P1 | Free — bed remapped to HE2 |
| MOTOR 5 | PC13/PF0/PF1 | — | Free — driver not installed |
| J36 PS_ON | PE11 | — | Free — PSU_CONTROL disabled |
| J40 PROBE (signal) | PC5 | — | Initialized as INPUT by probe code — not freely usable as output without firmware change |
| J43 PB7 | PB7 | — | Free — on BL_Touch header alongside PB6 |

---

## 12. FULL GPIO SUMMARY TABLE

| GPIO | Marlin Pin | Board Label | Function |
|---|---|---|---|
| PA1 | P1 | HB | Unused (bed moved to HE2) |
| PA2 | P2 | HE0 | Peltier E0 power (PWM) |
| PA3 | P3 | HE1 | Peltier E1 power (PWM) |
| PA8 | P8 | FAN0 | UV LED 1 (PWM) |
| PA9 | — | USART1 TX | TFT70 display |
| PA10 | — | USART1 RX | TFT70 display |
| PA11 | — | USB D- | USB CDC (PC) |
| PA12 | — | USB D+ | USB CDC (PC) |
| PA14 | — | — | J axis DIR |
| PB6 | P22 | J43 signal | Chamber light |
| PB10 | P26 | HE2 | Peltier Bed power (PWM) |
| PB11 | P27 | HE3 | Unused |
| PC1 | — | — | E0 DIR |
| PC3 | — | — | E1 ENABLE (pneumatic extruder driver EN) |
| PC4 | — | — | X TMC UART |
| PC5 | — | J40 PROBE | Z probe input (initialized as INPUT, avoid as output) |
| PC6 | — | — | Y TMC UART |
| PC7 | — | — | E0 TMC UART |
| PD4 | — | — | I axis ENABLE |
| PD11 | — | — | Z TMC UART |
| PD12 | P60 | FAN2 | Peltier E0 DPDT direction |
| PD13 | P61 | FAN3 | Peltier E1 DPDT direction |
| PD14 | P62 | FAN4 | Peltier Bed DPDT direction |
| PD15 | P63 | FAN5 | HEPA fan relay |
| PE0 | — | — | J axis ENABLE |
| PE1 | — | — | I axis TMC UART |
| PE2 | — | — | I axis STEP |
| PE3 | — | — | I axis DIR |
| PE5 | P69 | FAN1 | UV LED 2 (PWM) |
| PE6 | — | — | J axis STEP |
| PE11 | — | J36 PS_ON | Unused (PSU_CONTROL disabled) |
| PF3 | — | TH1 / J44 | Thermistor T1 (pneumatic extruder) |
| PF4 | — | TH0 | Thermistor T0 (syringe extruder) |
| PF5 | — | TB / J46 | Thermistor Bed |
| PF6 | — | J48 | Thermistor Chamber |
| PF9 | — | MOTOR4 STEP | E1 STEP (pneumatic extruder) |
| PF10 | — | MOTOR4 DIR | E1 DIR (pneumatic extruder) |
| PF11 | — | MOTOR2 STEP | Y STEP |
| PF12 | — | MOTOR0 DIR | X DIR |
| PF13 | — | MOTOR0 STEP | X STEP |
| PF14 | — | MOTOR0 EN | X ENABLE |
| PG0 | — | MOTOR1 STEP | Z STEP |
| PG1 | — | MOTOR1 DIR | Z DIR |
| PG3 | — | MOTOR2 DIR | Y DIR |
| PG4 | — | MOTOR3 STEP | E0 STEP (syringe) |
| PG5 | — | MOTOR2 EN | Y ENABLE |
| PG6 | — | X-STOP | X endstop |
| PG9 | — | Y-STOP | Y endstop |
| PG10 | — | Z-STOP | Z endstop |
| PG14 | — | J32 | I axis endstop |
| PG15 | — | J34 | J axis endstop |
