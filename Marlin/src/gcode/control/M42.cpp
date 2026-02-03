/**
 * Marlin 3D Printer Firmware
 * Copyright (c) 2020 MarlinFirmware [https://github.com/MarlinFirmware/Marlin]
 *
 * Based on Sprinter and grbl.
 * Copyright (c) 2011 Camiel Gubbels / Erik van der Zalm
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */

#include "../../inc/MarlinConfig.h"

#if ENABLED(DIRECT_PIN_CONTROL)

#include "../gcode.h"
#include "../../MarlinCore.h" // for pin_is_protected

#if HAS_FAN
  #include "../../module/temperature.h"
#endif

#ifdef MAPLE_STM32F1
  // these are enums on the F1...
  #define INPUT_PULLDOWN INPUT_PULLDOWN
  #define INPUT_ANALOG INPUT_ANALOG
  #define OUTPUT_OPEN_DRAIN OUTPUT_OPEN_DRAIN
#endif

void protected_pin_err() {
  SERIAL_ERROR_MSG(STR_ERR_PROTECTED_PIN);
}

/**
 * M42: Change pin status via GCode
 *
 *  P<pin>  Pin number (LED if omitted)
 *          For LPC1768 specify pin P1_02 as M42 P102,
 *                                  P1_20 as M42 P120, etc.
 *
 *  S<byte> Pin status from 0 - 255
 *  I       Flag to ignore Marlin's pin protection
 *
 *  T<mode> Pin mode: 0=INPUT  1=OUTPUT  2=INPUT_PULLUP  3=INPUT_PULLDOWN
 */
void GcodeSuite::M42() {
  const int pin_index = PARSED_PIN_INDEX('P', GET_PIN_MAP_INDEX(LED_PIN));
  if (pin_index < 0) return;

  const pin_t pin = GET_PIN_MAP_PIN(pin_index);

  if (!parser.boolval('I') && pin_is_protected(pin)) return protected_pin_err();

  bool avoidWrite = false;
  if (parser.seenval('T')) {
    switch (parser.value_byte()) {
      case 0: pinMode(pin, INPUT); avoidWrite = true; break;
      case 1: pinMode(pin, OUTPUT); break;
      case 2: pinMode(pin, INPUT_PULLUP); avoidWrite = true; break;
      #ifdef INPUT_PULLDOWN
        case 3: pinMode(pin, INPUT_PULLDOWN); avoidWrite = true; break;
      #endif
      #ifdef INPUT_ANALOG
        case 4: pinMode(pin, INPUT_ANALOG); avoidWrite = true; break;
      #endif
      #ifdef OUTPUT_OPEN_DRAIN
        case 5: pinMode(pin, OUTPUT_OPEN_DRAIN); break;
      #endif
      default: SERIAL_ECHOLNPGM("Invalid Pin Mode"); return;
    }
  }

  if (!parser.seenval('S')) return;
  const byte pin_status = parser.value_byte();

  #if HAS_FAN
    switch (pin) {
      #define _CASE(N) case FAN##N##_PIN: thermalManager.fan_speed[N] = pin_status; return;
      REPEAT(FAN_COUNT, _CASE)
    }
  #endif

  if (avoidWrite) {
    SERIAL_ECHOLNPGM("?Cannot write to INPUT");
    return;
  }

  // An OUTPUT_OPEN_DRAIN should not be changed to normal OUTPUT (STM32)
  // Use M42 Px T1/5 S0/1 to set the output type and then set value
  #ifndef OUTPUT_OPEN_DRAIN
    pinMode(pin, OUTPUT);
  #endif

  // BIOPRINTER: Invert output for Peltier pins (hardware inverts signal)
  // S255 = heating (relay ON), S0 = cooling (relay OFF)
  byte output_value = pin_status;
  #if CUSTOM_BED_PIN
    if (pin == CUSTOM_BED_PIN) output_value = 255 - pin_status;
  #endif
  #if CUSTOM_HEATER_1_PIN
    if (pin == CUSTOM_HEATER_1_PIN) output_value = 255 - pin_status;
  #endif
  #if CUSTOM_HEATER_2_PIN
    if (pin == CUSTOM_HEATER_2_PIN) output_value = 255 - pin_status;
  #endif
  extDigitalWrite(pin, output_value);

  #ifdef ARDUINO_ARCH_STM32
    // A simple I/O will be set to 0 by hal.set_pwm_duty()
    if (output_value <= 1 && !PWM_PIN(pin)) return;
  #endif
  hal.set_pwm_duty(pin, output_value);
}

#endif // DIRECT_PIN_CONTROL
