# Designing Alley Bioprinter — Firmware Documentation

**Board:** BTT Octopus Pro V1.0
**MCU:** STM32F446ZET6
**Firmware:** Marlin 2.x (bugfix branch)
**Build Environment:** PlatformIO (`STM32F446ZE_btt`)

---

## Serial / Display

| Port | Interface | Pins | Baud | Purpose |
|------|-----------|------|------|---------|
| SERIAL_PORT 1 | USART1 | PA9 (TX) / PA10 (RX) | 250000 | BTT TFT70 V3.0 Touch Mode |
| SERIAL_PORT_2 -1 | USB CDC | — | 115200 | PC terminal / PlatformIO |

**Display:** BTT TFT70 V3.0 — connected via J70 RS232 header (fully reversed cable: pin 1↔5, 2↔4, 3=centre).
TFT config: `serial_port:P1:8` (P1 = 250000 baud). Copy `display/tft70_config.ini` to TFT SD card to apply.

---

## Pin Assignments

### Motor Drivers

| Slot | Axis | Motor | Driver | Current | Steps/mm | Function |
|------|------|-------|--------|---------|----------|----------|
| 0 | X | NEMA 17 | TMC2209 UART | 800 mA | 400 | X-axis linear motion |
| 1 | Y | NEMA 17 | TMC2209 UART | 800 mA | 400 | Y-axis linear motion |
| 2 | Z | NEMA 17 | TMC2209 UART | 800 mA | 400 | Z-axis (bed) height |
| 4 | E0 | NEMA 11 | TMC2209 UART | 670 mA | 1600 | Syringe extruder |
| 5 | E1 | Solenoid | A4988 | — | — | Pneumatic valve |
| 6 | I (U) | NEMA 8 | TMC2209 UART | 800 mA | 1600 | Printhead 0 Z-offset |
| 7 | J (V) | NEMA 8 | TMC2209 UART | 800 mA | 1600 | Printhead 1 Z-offset |

### Peltier DPDT Control

Single Peltier module with bidirectional control via DPDT relay:

| Pin | GPIO | Function | Command |
|-----|------|----------|---------|
| HE0 PWM | PA2 | Peltier power (0–255) | `M104 Sxx` |
| P60 | PD12 | DPDT relay polarity | `M42 P60 S255` = Heat, `M42 P60 S0` = Cool |
| TH0 | PF4 | Peltier thermistor | `M105` |

**PID tuned at 40°C:** `Kp=222.72  Ki=25.08  Kd=494.43` — ±0.25°C stability

```gcode
M301 P222.72 I25.08 D494.43   ; Apply PID values
M500                           ; Save to EEPROM
M104 S40                       ; Set target temperature
M42 P60 S255                   ; DPDT relay → Heating
M42 P60 S0                     ; DPDT relay → Cooling
M104 S0                        ; Heater off
```

### UV LED Control

| Pin | GPIO | Function | Command |
|-----|------|----------|---------|
| P8 | PA8 | UV LED 1 | `M42 P8 S0–S255` |
| P69 | PE5 | UV LED 2 | `M42 P69 S0–S255` |

**Intensity:** S0=OFF, S64=25%, S128=50%, S191=75%, S255=100%

### Endstops

| Axis | Connector | GPIO | Polarity |
|------|-----------|------|----------|
| X | DIAG0 | PG6 | Normal (open = not triggered) |
| Y | DIAG1 | PG9 | Normal |
| Z | DIAG2 | PG10 | Normal |
| I (U) | E1DET | PG14 | Inverted (NC switch) |
| J (V) | E2DET | PG15 | Inverted (NC switch) |
| Syringe | E0DET | PG11 | Inverted (NC switch) |

---

## Axis Configuration

### Travel Limits

| Axis | Min | Max |
|------|-----|-----|
| X | 0 | 280 mm |
| Y | 0 | 90 mm |
| Z | 0 | 60 mm |
| I (U) | 0 | 50 mm |
| J (V) | 0 | 50 mm |

### Speed Settings

| Axis | Max Feedrate | Homing Speed | Acceleration |
|------|--------------|--------------|--------------|
| X | 10 mm/s | 5 mm/s | 500 mm/s² |
| Y | 10 mm/s | 5 mm/s | 500 mm/s² |
| Z | 3 mm/s | 2 mm/s | 100 mm/s² |
| I/J | 2 mm/s | 2 mm/s | 100 mm/s² |
| E0 | 3 mm/s | 1 mm/s | 1000 mm/s² |

### Homing Order

**G28** homes in this sequence: `I → J → Z → X → Y`

---

## Motor Specifications

### NEMA 17 (X/Y/Z)
- Lead screw: 2mm pitch
- Microstepping: 4×
- Steps/mm: `(200 × 4) / 2 = 400`

### NEMA 11 (E0 Syringe) — JK28HST32-0674
- Rated current: 670 mA
- Lead screw: Tr5×1mm (1mm lead)
- Microstepping: 8×
- Steps/mm: `(200 × 8) / 1 = 1600`

### NEMA 8 (I/J Printhead Z)
- Lead screw: 2mm lead
- Microstepping: 16×
- Steps/mm: `(200 × 16) / 2 = 1600`

---

## G-Code Reference

### Axis Movement
```gcode
G28              ; Home all axes (I→J→Z→X→Y)
G28 X Y          ; Home X and Y only
G28 I            ; Home I (U) axis only
G28 J            ; Home J (V) axis only
G1 X50 Y25 F600  ; Move to position
G1 E5 F60        ; Extrude 5mm at 1mm/s
```

### Peltier Control
```gcode
M301 P222.72 I25.08 D494.43   ; Restore tuned PID
M104 S40                       ; Target 40°C
M42 P60 S255                   ; DPDT → Heating
M42 P60 S0                     ; DPDT → Cooling
M105                           ; Read temperature
M104 S0                        ; Heater off
```

### UV LED Control
```gcode
M42 P8 S255    ; UV LED 1 — full power
M42 P8 S128    ; UV LED 1 — 50%
M42 P8 S0      ; UV LED 1 — off
M42 P69 S255   ; UV LED 2 — full power
M42 P69 S0     ; UV LED 2 — off
```

### Syringe / Pneumatic
```gcode
M302 P1     ; Allow cold extrusion
T0          ; Select syringe (E0)
G1 E5 F60   ; Extrude 5mm
G1 E-2 F60  ; Retract 2mm
T1          ; Select pneumatic (E1)
G1 E10 F60  ; Open valve
```

### Temperature Auto-Reporting
```gcode
M155 S2    ; Auto-report every 2s (keeps TFT connected)
M155 S0    ; Stop auto-reporting
```

### Configuration
```gcode
M92 E1600   ; Set E0 steps/mm
M906 E670   ; Set E0 current (mA)
M119        ; Check endstop states
M503        ; Report current settings
M500        ; Save to EEPROM
```

---

## Build Instructions

```bash
# Clone and build
git clone https://github.com/DarkDragoXE/DesigningAlleyBioPrinter.git
cd DesigningAlleyBioPrinter
pio run -e STM32F446ZE_btt

# Firmware output
.pio/build/STM32F446ZE_btt/firmware.bin
```

Flash via SD card: rename to `firmware.bin`, copy to SD root, insert and power cycle the board.

### TFT70 Display Setup

1. Copy contents of `display/` folder to the TFT SD card root
2. Insert SD card into TFT70, power cycle
3. TFT will flash firmware and apply config automatically
4. Connect TFT RS232 to Octopus J70 header with **fully reversed** 5-pin cable

---

## Files Modified from Stock Marlin

| File | Changes |
|------|---------|
| `Configuration.h` | Steps/mm, travel limits, speeds, serial ports |
| `Configuration_adv.h` | Motor currents, startup commands, auto-reporting |
| `MarlinCore.cpp` | Peltier pin initialisation |
| `M42.cpp` | Peltier signal logic |
| `G28.cpp` | Custom homing order (I→J→Z→X→Y) |
| `pins_BTT_OCTOPUS_V1_common.h` | FAN pins repurposed for Peltier/UV |

---

*Last updated: 2026-03-19*
