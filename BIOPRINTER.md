# Designing Alley Bioprinter - Firmware Documentation

**Board:** BTT Octopus V1.1
**MCU:** STM32F446ZET6
**Firmware:** Marlin 2.x (bugfix branch)
**Build Environment:** PlatformIO (`STM32F446ZE_btt`)

---

## Pin Assignments

### Motor Drivers (TMC2209 UART)

| Slot | Axis | Motor Type | Current | Steps/mm | Function |
|------|------|------------|---------|----------|----------|
| 0 | X | NEMA 17 | 800mA | 400 | X-axis linear motion |
| 1 | Y | NEMA 17 | 800mA | 400 | Y-axis linear motion |
| 2 | Z | NEMA 17 | 800mA | 400 | Z-axis (bed) height |
| 4 | E0 | NEMA 11 | 670mA | 1600 | Syringe extruder |
| 5 | E1 | Valve | N/A | 500 | Pneumatic system |
| 6 | I (U) | NEMA 8 | 800mA | 3200 | Printhead 0 Z-offset |
| 7 | J (V) | NEMA 8 | 800mA | 3200 | Printhead 1 Z-offset |

### Peltier DPDT Control

| Pin | GPIO | Function | M42 Command |
|-----|------|----------|-------------|
| P60 | PD12 | Peltier 0 DPDT | `M42 P60 S0/S255` |
| P61 | PD13 | Peltier 1 DPDT | `M42 P61 S0/S255` |
| P62 | PD14 | Peltier 2 DPDT | `M42 P62 S0/S255` |

**Control Logic (hardware inverts signal):**
- `S0` = LOW = Relay OFF = **Cooling**
- `S255` = HIGH = Relay ON = **Heating**
- Startup state: Cooling (relay OFF)

### UV LED Control

| Pin | GPIO | Function | M42 Command |
|-----|------|----------|-------------|
| P8 | PA8 | UV LED 1 | `M42 P8 S0-S255` |
| P69 | PE5 | UV LED 2 | `M42 P69 S0-S255` |

**Intensity:** S0=OFF, S64=25%, S128=50%, S191=75%, S255=100%

### Endstops

| Axis | Connector | GPIO | Direction |
|------|-----------|------|-----------|
| X | DIAG0 | PG6 | MIN |
| Y | DIAG1 | PG9 | MIN |
| Z | DIAG2 | PG10 | MIN |
| I (U) | E1DET | PG13 | MIN |
| J (V) | E2DET | PG14 | MIN |

---

## Axis Configuration

### Travel Limits

| Axis | Min | Max | Bed Size |
|------|-----|-----|----------|
| X | 0 | 280mm | 280mm |
| Y | 0 | 90mm | 90mm |
| Z | 0 | 10mm | - |
| I (U) | 0 | 50mm | - |
| J (V) | 0 | 50mm | - |

### Speed Settings

| Axis | Max Feedrate | Homing Speed | Acceleration |
|------|--------------|--------------|--------------|
| X | 10 mm/s | 5 mm/s | 500 mm/s² |
| Y | 10 mm/s | 5 mm/s | 500 mm/s² |
| Z | 3 mm/s | 2 mm/s | 100 mm/s² |
| I/J | 2 mm/s | 2 mm/s | 100 mm/s² |
| E0 | 3 mm/s | 1 mm/s | 1000 mm/s² |

### Homing Order

**G28** homes in this sequence: `U → V → Z → X → Y`

### Direction Inversion

| Axis | Inverted |
|------|----------|
| X | false |
| Y | false |
| Z | false |

---

## Motor Specifications

### NEMA 17 (X/Y/Z)
- Lead screw: 2mm pitch
- Microstepping: 4x (hardware)
- Steps/mm: `(200 × 4) / 2 = 400`

### NEMA 11 (E0 Syringe)
- Model: JK28HST32-0674
- Rated current: 670mA
- Lead screw: Tr5×1mm (1mm lead)
- Microstepping: 8x
- Steps/mm: `(200 × 8) / 1 = 1600`

### NEMA 8 (I/J Printhead Z)
- Lead screw: 1mm pitch
- Microstepping: 16x
- Steps/mm: `(200 × 16) / 1 = 3200`

---

## G-Code Reference

### Axis Movement
```gcode
G28           ; Home all axes (U→V→Z→X→Y)
G28 X Y       ; Home X and Y only
G28 I         ; Home I (U) axis only
G28 J         ; Home J (V) axis only
G1 X50 Y25 F600   ; Move to position
G1 E5 F60     ; Extrude 5mm at 1mm/s
```

### Peltier Control
```gcode
M42 P60 S255  ; Peltier 0 - Heating
M42 P60 S0    ; Peltier 0 - Cooling
M42 P61 S255  ; Peltier 1 - Heating
M42 P62 S255  ; Peltier 2 - Heating
```

### UV LED Control
```gcode
M42 P8 S255   ; UV LED 1 - Full power
M42 P8 S128   ; UV LED 1 - 50%
M42 P8 S0     ; UV LED 1 - Off
M42 P69 S255  ; UV LED 2 - Full power
```

### Configuration
```gcode
M92 E1600     ; Set E0 steps/mm
M906 E670     ; Set E0 current (mA)
M119          ; Check endstop states
M500          ; Save to EEPROM
```

---

## Build Instructions

```bash
# Using PlatformIO CLI
pio run -e STM32F446ZE_btt

# Firmware output
.pio/build/STM32F446ZE_btt/firmware.bin
```

Flash via SD card: rename to `firmware.bin`, copy to SD, insert and power cycle.

---

## Files Modified

| File | Changes |
|------|---------|
| `Configuration.h` | Steps/mm, travel limits, speeds, Peltier pins |
| `Configuration_adv.h` | HOME_Z_FIRST, motor currents |
| `MarlinCore.cpp` | Peltier pin initialization |
| `M42.cpp` | Peltier signal inversion |
| `G28.cpp` | Custom homing order (U→V→Z→X→Y) |
| `pins_BTT_OCTOPUS_V1_common.h` | FAN pins disabled for Peltier use |

---

## Development Notes

- **Iterations:** 4 sessions to reach stable configuration
- **Key fix:** Hardware signal inversion on PD12/PD13/PD14 required software compensation
- **Microstepping:** Actual hardware microstepping differs from TMC2209 UART config; calibrate empirically

---

*Last updated: 2026-02-03*
