# Designing Alley Bioprinter

A high-throughput multi-nozzle bioprinter for tissue engineering and biomaterial research.

---

## Overview

This bioprinter features independent dual printheads with precision syringe extrusion, Peltier-based temperature control for bio-ink preservation, and UV LED crosslinking for photopolymer hydrogels.

### Key Features

- **Multi-nozzle system** - Dual independent Z-height printheads (U/V axes)
- **Precision extrusion** - NEMA 11 syringe pump with 0.625μm resolution
- **Temperature control** - 3-channel Peltier heating/cooling for bio-ink
- **UV crosslinking** - Dual UV LED arrays for photopolymer curing
- **Pneumatic dispensing** - Secondary pneumatic valve system

---

## Hardware

| Component | Specification |
|-----------|---------------|
| Controller | BTT Octopus V1.1 (STM32F446) |
| Drivers | TMC2209 UART (x7) |
| Build Volume | 280 x 90 x 10 mm |
| Syringe Resolution | 0.625 μm/step |
| Printheads | 2 (independent Z) |

### Motion System

| Axis | Motor | Lead Screw | Resolution |
|------|-------|------------|------------|
| X | NEMA 17 | 2mm pitch | 2.5 μm/step |
| Y | NEMA 17 | 2mm pitch | 2.5 μm/step |
| Z | NEMA 17 | 2mm pitch | 2.5 μm/step |
| U (Printhead 1) | NEMA 8 | 1mm pitch | 0.3125 μm/step |
| V (Printhead 2) | NEMA 8 | 1mm pitch | 0.3125 μm/step |
| E0 (Syringe) | NEMA 11 | Tr5x1mm | 0.625 μm/step |

### Temperature Control

3 independent Peltier channels with DPDT relay switching for precise bio-ink temperature management:
- **P60** - Peltier channel 0
- **P61** - Peltier channel 1
- **P62** - Peltier channel 2

**PID tuned for 4°C to 40°C** - optimized for cell-laden hydrogels and temperature-sensitive biomaterials.

### Extrusion Systems

Dual extrusion modes for versatile biomaterial deposition:

| System | Motor | Application |
|--------|-------|-------------|
| **Syringe Pump** | NEMA 11 | High-viscosity hydrogels, cell-laden bio-inks |
| **Pneumatic** | Solenoid valve | Low-viscosity materials, rapid dispensing |

### UV Crosslinking

Dual UV LED arrays (365nm/405nm) with PWM intensity control:
- **P8** - UV LED 1
- **P69** - UV LED 2

---

### Axis Calibration

All axes calibrated for precise motion control:
- Steps/mm calculated from lead screw pitch and microstepping
- Empirically verified with dial indicators
- Sub-micron repeatability for consistent layer deposition

---

## Bioprinting Capabilities

- **Hydrogel scaffolds** - Alginate, GelMA, collagen-based bio-inks
- **Cell-laden constructs** - Temperature-controlled for cell viability
- **Multi-material printing** - Dual nozzle for support/structure combinations
- **UV crosslinking** - In-situ photopolymerization of GelMA and similar materials

---

## Firmware

Based on Marlin 2.x with custom modifications for bioprinting:

- Custom homing sequence (U → V → Z → X → Y)
- Peltier DPDT control with signal inversion
- Independent printhead Z-offset (I/J axes displayed as U/V)
- Slow precision movements for syringe extrusion
- PID temperature control tuned for bioprinting range (4-40°C)

### Build

```bash
pio run -e STM32F446ZE_btt
```

Flash `firmware.bin` via SD card.

---

## Control Commands

### Peltier Temperature
```gcode
M42 P60 S255    ; Channel 0 - Heating
M42 P60 S0      ; Channel 0 - Cooling
```

### UV Crosslinking
```gcode
M42 P8 S255     ; UV LED 1 - 100%
M42 P8 S128     ; UV LED 1 - 50%
M42 P8 S0       ; UV LED 1 - Off
```

### Syringe Extrusion (T0)
```gcode
T0              ; Select syringe extruder
G1 E5 F60       ; Extrude 5mm at 1mm/s
G1 E-2 F60      ; Retract 2mm
```

### Pneumatic Dispensing (T1)
```gcode
T1              ; Select pneumatic system
G1 E10 F60      ; Valve open for 10 seconds
G1 E0 F60       ; Valve close
```

---

## Project

**Project Lead:** Debtonu Bose
**Organization:** Designing Alley
**Application:** Tissue engineering, biomaterial deposition, hydrogel scaffolds

---

*See [BIOPRINTER.md](BIOPRINTER.md) for detailed pin assignments and configuration.*
