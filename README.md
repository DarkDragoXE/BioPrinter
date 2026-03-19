# Designing Alley Bioprinter

A high-throughput multi-nozzle bioprinter for tissue engineering and biomaterial research.

---

## Overview

This bioprinter features independent dual printheads with precision syringe extrusion, Peltier-based temperature control for bio-ink preservation, and UV LED crosslinking for photopolymer hydrogels. It runs custom Marlin 2.x firmware on a BTT Octopus Pro V1.0 controller.

### Key Features

- **Multi-nozzle system** — Dual independent Z-height printheads (U/V axes)
- **Precision extrusion** — NEMA 11 syringe pump with sub-micron resolution
- **Peltier temperature control** — Bidirectional heating/cooling via DPDT relay for bio-ink temperature management
- **UV crosslinking** — Dual UV LED arrays for photopolymer curing
- **Pneumatic dispensing** — Solenoid valve system for low-viscosity materials
- **Touch display** — BTT TFT70 V3.0 connected via UART Touch Mode

---

## Hardware

| Component | Specification |
|-----------|---------------|
| Controller | BTT Octopus Pro V1.0 (STM32F446ZET6) |
| Display | BTT TFT70 V3.0 (UART Touch Mode via RS232) |
| Drivers | TMC2209 UART (motion axes) + A4988 (pneumatic) |
| Build Volume | 280 × 90 × 60 mm |
| Syringe Resolution | sub-micron / step |
| Printheads | 2 (independent Z) |

### Motion System

| Axis | Purpose | Motor | Lead Screw | Steps/mm |
|------|---------|-------|------------|----------|
| X | X stage | NEMA 17 | 2mm pitch | 400 |
| Y | Y stage | NEMA 17 | 2mm pitch | 400 |
| Z | Z stage | NEMA 17 | 2mm pitch | 400 |
| U | Printhead 0 Z-height | NEMA 8 | 1mm pitch | 1600 |
| V | Printhead 1 Z-height | NEMA 8 | 1mm pitch | 1600 |
| E0 (Syringe) | Syringe pump | NEMA 11 | Tr5×1mm | 1600 |

### Temperature Control

Single Peltier module with DPDT relay for bidirectional heat/cool control:

- **PWM power** — PA2 (HE0 pin, 0–255 duty cycle)
- **Polarity relay** — PD12 (DPDT, HIGH = heat, LOW = cool)
- **Thermistor** — PF4 (TH0 channel)

**PID tuned at 40°C:** `Kp=222.72  Ki=25.08  Kd=494.43` — ±0.25°C stability

```gcode
M301 P222.72 I25.08 D494.43   ; Apply PID values
M104 S40                       ; Set target temperature
M42 P60 S255                   ; DPDT → Heating mode
M42 P60 S0                     ; DPDT → Cooling mode
M104 S0                        ; Turn heater off
```

### Extrusion Systems

| System | Driver | Application |
|--------|--------|-------------|
| **Syringe Pump (E0)** | TMC2209 UART | High-viscosity hydrogels, cell-laden bio-inks |
| **Pneumatic (E1)** | A4988 / solenoid | Low-viscosity materials, rapid dispensing |

### UV Crosslinking

Dual UV LED arrays with PWM intensity control:

```gcode
M42 P8 S255    ; UV LED 1 — 100%
M42 P8 S128    ; UV LED 1 — 50%
M42 P8 S0      ; UV LED 1 — Off
M42 P69 S255   ; UV LED 2 — 100%
M42 P69 S0     ; UV LED 2 — Off
```

---

## Firmware

Based on Marlin 2.x (bugfix branch) with custom modifications for bioprinting.

### Key Customisations

- **Serial port** — USART1 (PA9/PA10) as primary serial for BTT TFT70 UART Touch Mode
- **Startup sequence** — Auto cold extrusion enable + temperature auto-reporting for TFT handshake
- **Custom homing** — U → V → Z → X → Y sequence
- **Peltier DPDT control** — HE0 pin PWM + M42 relay switching
- **Syringe homing** — Forward-to-endstop with configurable retract
- **Precision motion** — Slow feedrates for accurate syringe dispensing

### Serial Port Configuration

```c
#define SERIAL_PORT   1    // USART1 — PA9 (TX) / PA10 (RX) — TFT70 Touch Mode
#define BAUDRATE      250000
#define SERIAL_PORT_2 -1   // USB CDC — PC terminal / PlatformIO
#define BAUDRATE_2    115200
```

### Build

```bash
pio run -e STM32F446ZE_btt
```

Flash `firmware.bin` via SD card.

---

## Control Commands

### Peltier Temperature

```gcode
M301 P222.72 I25.08 D494.43   ; Restore tuned PID
M104 S40                       ; Target 40°C
M42 P60 S255                   ; DPDT relay → Heating
M42 P60 S0                     ; DPDT relay → Cooling
M105                           ; Read current temperature
M104 S0                        ; Heater off
```

### UV Crosslinking

```gcode
M42 P8 S255    ; UV LED 1 — full power
M42 P8 S128    ; UV LED 1 — 50%
M42 P8 S0      ; UV LED 1 — off
```

### Syringe Extrusion (T0)

```gcode
T0              ; Select syringe extruder
M302 P1         ; Allow cold extrusion
G1 E5 F60       ; Extrude 5mm at 1mm/s
G1 E-2 F60      ; Retract 2mm
```

### Pneumatic Dispensing (T1)

```gcode
T1              ; Select pneumatic system
G1 E10 F60      ; Open valve for dispensing
G1 E0 F60       ; Close valve
```

### Temperature Auto-Reporting (TFT connection)

```gcode
M155 S2         ; Auto-report temperatures every 2 seconds
M155 S0         ; Stop auto-reporting
```

---

## Bioprinting Capabilities

- **Hydrogel scaffolds** — Alginate, GelMA, collagen-based bio-inks
- **Cell-laden constructs** — Peltier temperature control for cell viability
- **Multi-material printing** — Dual nozzle for support/structure combinations
- **UV crosslinking** — In-situ photopolymerisation of GelMA and similar materials

---

## Project

**Project Lead:** Debtonu Bose
**Organisation:** Designing Alley
**Application:** Tissue engineering, biomaterial deposition, hydrogel scaffolds

---

*See [BIOPRINTER.md](BIOPRINTER.md) for detailed pin assignments and configuration.*
