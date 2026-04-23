# SQUYD Bioprinter — Pin & Command Reference
**Board:** BTT Octopus PRO V1.0 | **MCU:** STM32F446ZET6 | **Firmware:** Marlin 2.x bugfix
**Last updated:** 2026-04-13

---

## 1. MOTION — Motors

| Axis | Motor Slot | STEP | DIR | EN | Steps/mm | Notes |
|---|---|---|---|---|---|---|
| X | Motor 0 | PF13 | PF12 | PF14 | 400 | Invert: false |
| Y | Motor 2 | PF11 | PG3 | PG5 | 400 | Invert: false (swapped from Motor 1) |
| Z | Motor 1 | PG0 | PG1 | PF15 | 400 | Invert: false (swapped from Motor 2) |
| E0 (Syringe 1) | Motor 3 | PG4 | PC1 | PA0 | 1600 | Invert: true |
| E1 (Syringe 2) | Motor 4 | PF9 | PF10 | PC3 | 1600 | Invert: false |
| I (Printhead Z1) | Motor 6 | PE2 | PE3 | PD4 | 1600 | Invert: false |
| J (Printhead Z2) | Motor 7 | PE6 | PA14 | PE0 | 1600 | Invert: false |

### Motion G-codes

```
G28          → Home all axes
G28 X        → Home X only
G28 Y        → Home Y only
G28 Z        → Home Z only
G28 I        → Home I axis (printhead Z1)
G28 J        → Home J axis (printhead Z2)

G1 X100 F600         → Move X to 100mm at 600mm/min
G1 Y50 Z10 F300      → Move Y and Z simultaneously
G1 E10 F200          → Extrude 10mm on E0 (syringe 1)
G1 A10 F200          → Move I axis 10mm
G1 B10 F200          → Move J axis 10mm

G91              → Relative positioning mode
G90              → Absolute positioning mode
G92 E0           → Reset extruder position to 0
G92 X0 Y0        → Set current position as X0 Y0

M17              → Enable all steppers
M18              → Disable all steppers
M84              → Disable steppers (same as M18)

M203 X10 Y10 Z3 E2   → Set max feedrate (mm/s)
M92 X400 Y400        → Set steps/mm
```

### Speed Limits (configured)

| Axis | Max Feedrate |
|---|---|
| X, Y | 10 mm/s |
| Z, I, J | 3 mm/s |
| E0, E1 | 2 mm/s |

---

## 2. ENDSTOPS

| Axis | Pin | Header | Type |
|---|---|---|---|
| X MIN | PG6 | X-STOP | TMC DIAG (sensorless or switch) |
| Y MIN | PG9 | Y-STOP | TMC DIAG (sensorless or switch) |
| Z MIN | PG10 | Z-STOP | TMC DIAG (sensorless or switch) |
| I MIN | PG14 | J32 | Motor 6 endstop |
| J MIN | PG15 | J34 | Motor 7 endstop |

```
M119    → Report current endstop states (OPEN / TRIGGERED)
```

---

## 3. TEMPERATURE SENSORS (Thermistors)

| Sensor | Pin | Header | Connected to |
|---|---|---|---|
| T0 (TEMP_SENSOR_0) | PF4 | TH0 | Extruder 0 thermistor |
| T1 (TEMP_SENSOR_1) | PF3 | TH1 / J44 | Extruder 1 thermistor |
| Bed (TEMP_SENSOR_BED) | PF5 | TB / J46 | Bed thermistor |
| Chamber (TEMP_SENSOR_CHAMBER) | PF6 | J48 | Chamber thermistor |

```
M105    → Report all temperatures (T0, T1, Bed, Chamber)
```

---

## 4. PELTIER POWER CONTROL (Heater outputs — PWM)

Power is controlled by Marlin's PID thermal loop via the heater MOSFET outputs.

| Peltier | Board Header | GPIO | Marlin Pin | Controls |
|---|---|---|---|---|
| Peltier E0 | HE0 | PA2 | P2 | Extruder 0 peltier power |
| Peltier E1 | HE1 | PA3 | P3 | Extruder 1 peltier power |
| Peltier Bed | HE2 | PB10 | P26 | Bed peltier power (remapped from HB/PA1) |

```
M104 S35         → Set extruder 0 target temp to 35°C (no wait)
M104 T1 S35      → Set extruder 1 target temp to 35°C (no wait)
M109 S35         → Set extruder 0 temp and wait until reached
M109 T1 S35      → Set extruder 1 temp and wait until reached
M140 S35         → Set bed target temp to 35°C (no wait)
M190 S35         → Set bed temp and wait until reached
M141 S35         → Set chamber target temp to 35°C (no wait)
M191 S35         → Set chamber temp and wait until reached
M303 E0 S35 C8   → PID autotune extruder 0 at 35°C, 8 cycles
M303 E-1 S35 C8  → PID autotune bed at 35°C, 8 cycles
M503             → Report all current settings including PID values
M500             → Save settings to EEPROM
```

---

## 5. PELTIER DIRECTION CONTROL (DPDT Relays — M42)

DPDT relays switch Peltier polarity between heating and cooling mode.
These are controlled via M42 (direct pin control).

| Peltier | GPIO | Marlin Pin | Board Header | S255 = | S0 = |
|---|---|---|---|---|---|
| Peltier E0 DPDT | PD12 | P60 | FAN2 | Heating mode | Cooling mode |
| Peltier E1 DPDT | PD13 | P61 | FAN3 | Heating mode | Cooling mode |
| Peltier Bed DPDT | PD14 | P62 | FAN4 | Heating mode | Cooling mode |

**Note:** Active-LOW relays — pin HIGH = relay OFF. Startup state = HIGH (heating mode).

```
M42 P60 S255    → Peltier E0: Heating mode
M42 P60 S0      → Peltier E0: Cooling mode

M42 P61 S255    → Peltier E1: Heating mode
M42 P61 S0      → Peltier E1: Cooling mode

M42 P62 S255    → Peltier Bed: Heating mode
M42 P62 S0      → Peltier Bed: Cooling mode
```

---

## 6. UV LED CONTROL (PWM — M42)

Both UV LEDs are controlled together. PWM supports variable intensity.

| LED | GPIO | Marlin Pin | Board Header | Startup |
|---|---|---|---|---|
| UV LED 1 | PA8 | P8 | FAN0 | OFF (LOW) |
| UV LED 2 | PE5 | P69 | FAN1 | OFF (LOW) |

```
M42 P8 S0                       → UV LED 1 OFF
M42 P8 S255                     → UV LED 1 100%
M42 P8 S127                     → UV LED 1 ~50%

M42 P69 S0                      → UV LED 2 OFF
M42 P69 S255                    → UV LED 2 100%

M42 P8 S0 \n M42 P69 S0        → Both UV LEDs OFF
M42 P8 S127 \n M42 P69 S127    → Both UV LEDs 50%
M42 P8 S255 \n M42 P69 S255    → Both UV LEDs 100%
```

**Display menu:** Main → Bioprinter → UV LED → OFF / 50% / 100%

---

## 7. HEPA FAN (On/Off relay — M42)

| GPIO | Marlin Pin | Board Header | Relay type | Startup |
|---|---|---|---|---|
| PD15 | P63 | FAN5 | Active-HIGH | OFF (LOW) |

```
M42 P63 S255    → HEPA Fan ON  (pin HIGH → relay closes → fans run)
M42 P63 S0      → HEPA Fan OFF (pin LOW  → relay open  → fans stop)
```

**Display menu:** Main → Bioprinter → HEPA Fan ON / HEPA Fan OFF

**Physical:** 2× fans (12V, 1.4A each = 2.8A total) wired in parallel through relay contacts.
Relay coil powered from external 12V supply switched by FAN5 MOSFET output.

---

## 8. CHAMBER LIGHT (On/Off — M42)

| GPIO | Marlin Pin | Board Header | Startup |
|---|---|---|---|
| PB6 | P22 | J43 BL_Touch (signal pin) | OFF (LOW) |

```
M42 P22 S255    → Chamber Light ON  (pin HIGH)
M42 P22 S0      → Chamber Light OFF (pin LOW)
```

**Display menu:** Main → Bioprinter → Chamber Light ON / Chamber Light OFF

**Physical:** J43 signal (PB6) → MOSFET gate → DPDT relay coil (12V) → relay contacts switch 240V AC chamber light.
Flyback diode (1N4007) mandatory across relay coil: cathode → coil(+), anode → coil(−).

---

## 9. SERIAL / COMMUNICATION

| Port | Interface | GPIO | Baud | Purpose |
|---|---|---|---|---|
| SERIAL_PORT = -1 | USB CDC | — | 115200 | PC (Pronterface, PlatformIO) |
| SERIAL_PORT_2 = 1 | USART1 | PA9(TX)/PA10(RX) | 250000 | BTT TFT70 display |

---

## 10. FREE / UNUSED PINS

| GPIO | Marlin Pin | Board Header | Notes |
|---|---|---|---|
| PA1 | P1 | HB | Unused (bed peltier moved to HE2/PB10) |
| PB11 | P27 | HE3 | Unused |
| PE11 | — | J36 PS_ON | PSU_CONTROL disabled |
| PB7 | — | J43 BL_Touch | Unused |
| PC5 | — | J40 PROBE | Probe disabled (Z_MIN_PROBE_PIN = PC5, initialized as input) |

---

## 11. DISPLAY MENU STRUCTURE

```
Main Menu
├── Tune
├── Motion
├── Temperature
├── Configuration
├── Bioprinter          ← custom menu
│   ├── HEPA Fan ON     → M42 P63 S255
│   ├── HEPA Fan OFF    → M42 P63 S0
│   ├── Chamber Light ON  → M42 P22 S255
│   ├── Chamber Light OFF → M42 P22 S0
│   └── UV LED          ← submenu
│       ├── UV LED OFF  → M42 P8 S0  + M42 P69 S0
│       ├── UV LED 50%  → M42 P8 S127 + M42 P69 S127
│       └── UV LED 100% → M42 P8 S255 + M42 P69 S255
└── Print Setup         ← custom menu (CUSTOM_MENU_MAIN)
    ├── 1-Home Extruder
    ├── 2-Set Full Position
    └── 3-Home XYZ
```

---

## 12. QUICK REFERENCE — ALL M42 COMMANDS

| Command | Action |
|---|---|
| `M42 P60 S255` | Peltier E0 → Heating mode |
| `M42 P60 S0` | Peltier E0 → Cooling mode |
| `M42 P61 S255` | Peltier E1 → Heating mode |
| `M42 P61 S0` | Peltier E1 → Cooling mode |
| `M42 P62 S255` | Peltier Bed → Heating mode |
| `M42 P62 S0` | Peltier Bed → Cooling mode |
| `M42 P8 S0` | UV LED 1 OFF |
| `M42 P8 S127` | UV LED 1 50% |
| `M42 P8 S255` | UV LED 1 100% |
| `M42 P69 S0` | UV LED 2 OFF |
| `M42 P69 S127` | UV LED 2 50% |
| `M42 P69 S255` | UV LED 2 100% |
| `M42 P63 S255` | HEPA Fan ON |
| `M42 P63 S0` | HEPA Fan OFF |
| `M42 P22 S255` | Chamber Light ON |
| `M42 P22 S0` | Chamber Light OFF |
