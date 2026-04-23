/**
 * menu_bioprinter.cpp
 * Designing Alley — SQUYD Bioprinter custom menu
 * Controls: HEPA fan on/off, UV LED off/50%/100%
 */

#include "../../inc/MarlinConfigPre.h"

#if HAS_MARLINUI_MENU

#include "menu_item.h"
#include "../../gcode/queue.h"

// ── UV LED submenu ────────────────────────────────────────────────────────────
void menu_bioprinter_uv() {
  START_MENU();
  BACK_ITEM(MSG_BACK);
  ACTION_ITEM_P(PSTR("UV LED OFF"),  []{ queue.inject_P(PSTR("M42 P8 S0\nM42 P69 S0")); });
  ACTION_ITEM_P(PSTR("UV LED 50%"),  []{ queue.inject_P(PSTR("M42 P8 S127\nM42 P69 S127")); });
  ACTION_ITEM_P(PSTR("UV LED 100%"), []{ queue.inject_P(PSTR("M42 P8 S255\nM42 P69 S255")); });
  END_MENU();
}

// ── Bioprinter main submenu ───────────────────────────────────────────────────
void menu_bioprinter() {
  START_MENU();
  BACK_ITEM(MSG_BACK);
  ACTION_ITEM_P(PSTR("HEPA Fan ON"),       []{ queue.inject_P(PSTR("M42 P63 S255")); });
  ACTION_ITEM_P(PSTR("HEPA Fan OFF"),      []{ queue.inject_P(PSTR("M42 P63 S0")); });
  ACTION_ITEM_P(PSTR("Chamber Light ON"),  []{ queue.inject_P(PSTR("M42 P22 S255")); });
  ACTION_ITEM_P(PSTR("Chamber Light OFF"), []{ queue.inject_P(PSTR("M42 P22 S0")); });
  SUBMENU_P(PSTR("UV LED"), menu_bioprinter_uv);
  END_MENU();
}

#endif // HAS_MARLINUI_MENU
