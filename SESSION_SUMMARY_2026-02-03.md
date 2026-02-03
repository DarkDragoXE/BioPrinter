# Bioprinter Firmware Configuration Session Summary
**Date:** 2026-02-03
**Repository:** https://github.com/DarkDragoXE/DesigningAlleyBioPrinter.git

---

## 1. Axis Steps/mm Calibration

### Problem:
- At 1600 steps/mm, commanding 1mm moved 4mm (4x too much)
- Indicates microstepping is 4x, not 16x as configured

### Solution:
Changed X/Y/Z steps/mm from 1600 to **400**

### Calculation:
```
Actual: 1600 steps/mm → 4mm movement for 1mm command
Correct: 1600 / 4 = 400 steps/mm

With 2mm pitch lead screw:
400 = (200 steps/rev × 4 microsteps) / 2mm pitch
```

### Note:
TMC2209 UART microstepping (16x) not being applied - likely hardware jumpers set to 4x on BTT Octopus board.

### Files Modified:
- **Configuration.h (line 1064):** `DEFAULT_AXIS_STEPS_PER_UNIT { 400, 400, 400, 3200, 3200, 3200, 500 }`

---

## 2. Direction Inversions Fixed

| Axis | Old Value | New Value | Reason |
|------|-----------|-----------|--------|
| Y | true | **false** | Direction was inverted |
| Z | true | **false** | Endstop at top = MIN position |

### Files Modified:
- **Configuration.h (line 1488):** `INVERT_Y_DIR false`
- **Configuration.h (line 1489):** `INVERT_Z_DIR false`

---

## 3. Travel Limits Updated

| Axis | Old Value | New Value |
|------|-----------|-----------|
| X | 500mm | **280mm** |
| Y | 115mm | **90mm** |
| Z | 200mm | **10mm** |

### Files Modified:
- **Configuration.h (line 1542):** `X_BED_SIZE 280`
- **Configuration.h (line 1543):** `Y_BED_SIZE 90`
- **Configuration.h (line 1557):** `Z_MAX_POS 10`

---

## 4. Homing Order Changed

### Old Order:
Z → X → Y → U → V

### New Order:
**U → V → Z → X → Y**

### Why:
User requested I and J (U/V) axes home first, then Z, then X/Y.

### Files Modified:
- **Configuration_adv.h (line 916):** `#define HOME_Z_FIRST` enabled
- **G28.cpp (lines 402-409):** Added I/J homing BEFORE Z
- **G28.cpp (lines 492-499):** Skipped I/J in SECONDARY_AXIS_CODE (already homed)

### Code Added (G28.cpp):
```cpp
// BIOPRINTER: Home I and J (U/V) axes FIRST
#if HAS_I_AXIS
  if (doI) homeaxis(I_AXIS);
#endif
#if HAS_J_AXIS
  if (doJ) homeaxis(J_AXIS);
#endif

TERN_(HOME_Z_FIRST, if (doZ) homeaxis(Z_AXIS));
```

---

## 5. Homing Speed Reduced

| Axis | Old Homing Speed | New Homing Speed |
|------|------------------|------------------|
| X | 40 mm/s | **5 mm/s** |
| Y | 40 mm/s | **5 mm/s** |
| Z | 2 mm/s | 2 mm/s (unchanged) |
| U/V | 2 mm/s | 2 mm/s (unchanged) |

### Files Modified:
- **Configuration.h (line 1943):** `HOMING_FEEDRATE_MM_M { (5*60), (5*60), (2*60), (2*60), (2*60) }`

---

## 6. Max Feedrate Reduced

| Axis | Old Max | New Max |
|------|---------|---------|
| X | 120 mm/s | **10 mm/s** |
| Y | 120 mm/s | **10 mm/s** |

### Files Modified:
- **Configuration.h (line 1071):** `DEFAULT_MAX_FEEDRATE { 10, 10, 3, 3, 3, 2, 2 }`

---

## 7. Z Homing Height Disabled

### Problem:
Z was raising before homing X/Y axes (Z_HOMING_HEIGHT not explicitly set to 0).

### Solution:
```cpp
#define Z_HOMING_HEIGHT 0  // No Z movement before homing X/Y
```

### Files Modified:
- **Configuration.h (line 1521):** Explicitly set to 0

---

## 8. Peltier DPDT Initialization (Active-LOW Relays)

### Problem:
DPDT relays (P60, P61, P62) were turning ON at startup because pins weren't initialized to HIGH.

### Solution:
All three Peltier DPDT pins now initialize to **HIGH** (OFF for active-LOW relays).

### Relay Logic:
- **HIGH = Relay OFF** (no current flow)
- **LOW = Relay ON** (current flows)

### M42 Commands:
```gcode
M42 P60 S0     ; Turn relay ON (active-LOW)
M42 P60 S255   ; Turn relay OFF
```

### Files Modified:
- **MarlinCore.cpp (lines 1203-1213):** Added initialization for P60, P61, P62

### Code Added:
```cpp
// BIOPRINTER: Initialize Peltier DPDT pins to HIGH (active-LOW relays: HIGH = OFF)
#ifdef CUSTOM_BED_PIN
  pinMode(CUSTOM_BED_PIN, OUTPUT);
  digitalWrite(CUSTOM_BED_PIN, HIGH);  // P60 - DPDT OFF at startup
#endif
#ifdef CUSTOM_PELTIER1_PIN
  pinMode(CUSTOM_PELTIER1_PIN, OUTPUT);
  digitalWrite(CUSTOM_PELTIER1_PIN, HIGH);  // P61 - DPDT OFF at startup
#endif
#ifdef CUSTOM_PELTIER_BED_PIN
  pinMode(CUSTOM_PELTIER_BED_PIN, OUTPUT);
  digitalWrite(CUSTOM_PELTIER_BED_PIN, HIGH);  // P62 - DPDT OFF at startup
#endif
```

---

## 9. J Axis Endstop Pin Conflict Fixed

### Problem:
J_MIN_PIN (PG15) conflicted with E3_DIAG_PIN and FIL_RUNOUT4_PIN.

### Solution:
Disabled conflicting pins.

### Files Modified:
- **pins_BTT_OCTOPUS_V1_common.h (line 59):** Commented out E3_DIAG_PIN
- **pins_BTT_OCTOPUS_V1_common.h (line 165):** Commented out FIL_RUNOUT4_PIN

---

## Summary Table

| Change | Before | After |
|--------|--------|-------|
| X/Y/Z Steps/mm | 1600 | 400 |
| INVERT_Y_DIR | true | false |
| INVERT_Z_DIR | true | false |
| X_BED_SIZE | 500 | 280 |
| Y_BED_SIZE | 115 | 90 |
| Z_MAX_POS | 200 | 10 |
| Homing Order | Z→X→Y→U→V | U→V→Z→X→Y |
| X/Y Homing Speed | 40 mm/s | 5 mm/s |
| X/Y Max Feedrate | 120 mm/s | 10 mm/s |
| Z_HOMING_HEIGHT | undefined | 0 |
| DPDT Startup | LOW (ON) | HIGH (OFF) |

---

## Git Commits

1. **`17acf9a`** - Major axis calibration and configuration fixes
2. **`ae90ca8`** - Axis calibration and travel limit fixes
3. **`4f7c5e5`** - Custom homing order and Peltier DPDT initialization fixes

---

## Files Modified This Session

1. **Configuration.h**
   - Steps/mm, direction inversions, travel limits, homing speed, max feedrate, Z_HOMING_HEIGHT

2. **Configuration_adv.h**
   - HOME_Z_FIRST enabled

3. **MarlinCore.cpp**
   - Peltier DPDT pin initialization (P60, P61, P62 → HIGH)

4. **G28.cpp**
   - Custom homing order: U → V → Z → X → Y

5. **pins_BTT_OCTOPUS_V1_common.h**
   - Disabled E3_DIAG_PIN and FIL_RUNOUT4_PIN (conflict with J_MIN_PIN)

---

## Peltier Pin Reference

| Pin | Name | STM32 | Function | Startup State |
|-----|------|-------|----------|---------------|
| P60 | CUSTOM_BED_PIN | PD12 | Peltier E0 DPDT | HIGH (OFF) |
| P61 | CUSTOM_PELTIER1_PIN | PD13 | Peltier E1 DPDT | HIGH (OFF) |
| P62 | CUSTOM_PELTIER_BED_PIN | PD14 | Peltier Bed DPDT | HIGH (OFF) |

**Active-LOW Relay Logic:**
- `M42 Pxx S0` → Relay ON
- `M42 Pxx S255` → Relay OFF

---

## Next Steps

1. Build and flash firmware
2. Test all axis movements (1mm = 1mm actual)
3. Test homing order (U → V → Z → X → Y)
4. Verify DPDT relays stay OFF at startup
5. Test endstops with M119

---

**End of Session Summary**
