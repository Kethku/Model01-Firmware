// -*- mode: c++ -*-
// Copyright 2016 Keyboardio, inc. <jesse@keyboard.io>
// See "LICENSE" for license details

#ifndef BUILD_INFORMATION
#define BUILD_INFORMATION "locally built"
#endif

#include "Kaleidoscope.h"
#include "Kaleidoscope-MouseKeys.h"
#include "Kaleidoscope-LEDControl.h"
#include "Kaleidoscope-LED-Stalker.h"
#include "Kaleidoscope-HostPowerManagement.h"
#include "Kaleidoscope-USB-Quirks.h"
#include "Kaleidoscope-Macros.h"
#include "Kaleidoscope-MagicCombo.h"
#include "Kaleidoscope-TapDance.h"
#include "Kaleidoscope-Steno.h"
#include "Kaleidoscope-Leader.h"
#include "kaleidoscope/hid.h"
#include "MultiReport/Keyboard.h"
#include "key_defs_keymaps.h"
#include "layers.h"

enum { LEFT_BRACKET, RIGHT_BRACKET };
enum { QWERTY, STENO, FUNCTION };

/* This comment temporarily turns off astyle's indent enforcement
 *   so we can make the keymaps actually resemble the physical key layout better
 */
// *INDENT-OFF*

KEYMAPS(
[QWERTY] = KEYMAP_STACKED
(XXX,          Key_1, Key_2, Key_3, Key_4, Key_5, Key_Equals,
 Key_Tab,      Key_Q, Key_W, Key_E, Key_R, Key_T, TD(LEFT_BRACKET),
 Key_Escape,   Key_A, Key_S, Key_D, Key_F, Key_G,
 Key_Backtick, Key_Z, Key_X, Key_C, Key_V, Key_B, XXX,
   
 Key_Tab, Key_Backspace, Key_LeftShift,  Key_LeftControl,
 ShiftToLayer(FUNCTION),

 Key_Minus,         Key_6, Key_7, Key_8,     Key_9,         Key_0,         XXX,
 TD(RIGHT_BRACKET), Key_Y, Key_U, Key_I,     Key_O,         Key_P,         Key_Backslash,
                    Key_H, Key_J, Key_K,     Key_L,         Key_Semicolon, Key_Quote,
 XXX,               Key_N, Key_M, Key_Comma, Key_Period,    Key_Slash,     XXX,
 
 Key_RightAlt, Key_Enter, Key_Spacebar, Key_RightGui,
 ShiftToLayer(FUNCTION)),
 
[STENO] = KEYMAP_STACKED
(XXX,    XXX,   XXX,   XXX,   XXX,   XXX,   S(N6),
 XXX,    S(N1), S(N2), S(N3), S(N4), S(N5), S(ST1),
 S(FN),  S(S1), S(TL), S(PL), S(HL), S(ST1),
 S(PWR), S(S2), S(KL), S(WL), S(RL), S(ST2), S(ST2),

 S(RE1), S(A), S(O), XXX,
 ShiftToLayer(FUNCTION),

 S(N7),  XXX,    XXX,   XXX,   XXX,   XXX,   XXX,
 S(ST3), S(N8),  S(N9), S(NA), S(NB), S(NC), XXX,
 S(ST3), S(FR),  S(PR), S(LR), S(TR), S(DR),
 S(ST4), S(ST4), S(RR), S(BR), S(GR), S(SR), S(ZR),

 XXX, S(E), S(U), S(RE2),
 ShiftToLayer(FUNCTION)),

[FUNCTION] =  KEYMAP_STACKED
(XXX, Key_F1, Key_F2,     Key_F3,      Key_F4,            Key_F5,        XXX,
 XXX, XXX,    XXX,        Key_mouseUp, Key_mouseScrollUp, Key_mouseBtnR, XXX,
 XXX, XXX,    Key_mouseL, Key_mouseDn, Key_mouseR,        Key_mouseBtnL,
 XXX, XXX,    XXX,        XXX,         Key_mouseScrollDn, Key_mouseBtnM, XXX,
 
 Key_LeftAlt, Key_Delete, Key_LeftControl, Key_LeftShift,
 ShiftToLayer(FUNCTION),

 XXX, Key_F6,               Key_F7,               Key_F8,             Key_F9,                Key_F10, Key_F11,
 XXX, LCTRL(Key_LeftArrow), LCTRL(Key_DownArrow), LCTRL(Key_UpArrow), LCTRL(Key_RightArrow), XXX,     Key_F12,
      Key_LeftArrow,        Key_DownArrow,        Key_UpArrow,        Key_RightArrow,        XXX,     XXX,
 XXX, Key_Home,             Key_PageDown,         Key_PageUp,         Key_End,               XXX,     XXX,

 Key_RightShift, Key_Enter, Key_Tab, Key_RightGui,
 ShiftToLayer(FUNCTION))
) // KEYMAPS(

/* Re-enable astyle's indent enforcement */
// *INDENT-ON*

/** toggleLedsOnSuspendResume toggles the LEDs off when the host goes to sleep,
 * and turns them back on when it wakes up.
 */
void toggleLedsOnSuspendResume(kaleidoscope::HostPowerManagement::Event event) {
  switch (event) {
  case kaleidoscope::HostPowerManagement::Suspend:
    LEDControl.paused = true;
    LEDControl.set_all_leds_to({0, 0, 0});
    LEDControl.syncLeds();
    break;
  case kaleidoscope::HostPowerManagement::Resume:
    LEDControl.paused = false;
    LEDControl.refreshAll();
    break;
  case kaleidoscope::HostPowerManagement::Sleep:
    break;
  }
}

/** hostPowerManagementEventHandler dispatches power management events (suspend,
 * resume, and sleep) to other functions that perform action based on these
 * events.
 */
void hostPowerManagementEventHandler(kaleidoscope::HostPowerManagement::Event event) {
  toggleLedsOnSuspendResume(event);
}

void tapDanceAction(uint8_t tap_dance_index, byte row, byte col, uint8_t tap_count, kaleidoscope::TapDance::ActionType tap_dance_action) {
  switch (tap_dance_index) {
    case LEFT_BRACKET:
      return tapDanceActionKeys(tap_count, tap_dance_action, Key_LeftParen, Key_LeftCurlyBracket, Key_LeftBracket);
    case RIGHT_BRACKET:
      return tapDanceActionKeys(tap_count, tap_dance_action, Key_RightParen, Key_RightCurlyBracket, Key_RightBracket);
  }
}

bool skip = false;
void typeKey(Key key, uint8_t modifiers, bool tap) {
  HID_KeyboardReport_Data_t hid_report;
  memcpy(hid_report.allkeys, Keyboard.lastKeyReport.allkeys, sizeof(hid_report));
  Keyboard.keyReport.modifiers = modifiers;
  skip = true;
  handleKeyswitchEvent(key, UNKNOWN_KEYSWITCH_LOCATION, IS_PRESSED | INJECTED);
  kaleidoscope::hid::sendKeyboardReport();
  if (tap) {
    handleKeyswitchEvent(key, UNKNOWN_KEYSWITCH_LOCATION, WAS_PRESSED | INJECTED);
    kaleidoscope::hid::sendKeyboardReport();
  }
  memcpy(Keyboard.keyReport.allkeys, hid_report.allkeys, sizeof(Keyboard.keyReport));
}

namespace kaleidoscope {
  class FDEscape : public kaleidoscope::Plugin {
    public: 
      FDEscape() {}
      
      EventHandlerResult onKeyswitchEvent(Key &mapped_key, byte row, byte col, uint8_t key_state);
      EventHandlerResult afterEachCycle();

    private:
      static uint8_t stored_modifiers;
      static Key f_stored;
      static bool f_handled;
      static bool d_handled;
      static uint32_t start_time;
      static uint16_t time_out;
  };
  
  uint8_t FDEscape::stored_modifiers;
  Key FDEscape::f_stored = Key_NoKey;
  bool FDEscape::f_handled = false;
  bool FDEscape::d_handled = false;
  uint32_t FDEscape::start_time;
  uint16_t FDEscape::time_out = 200;
  
  EventHandlerResult FDEscape::onKeyswitchEvent(Key &mapped_key, byte row, byte col, uint8_t key_state) {
    if (skip) {
      skip = false;
      return EventHandlerResult::OK;
    }

    if (Layer.isOn(QWERTY)) {
      if (keyToggledOn(key_state)) {
        if (row == 2) {
          if (col == 4) {
            start_time = Kaleidoscope.millisAtCycleStart();
            f_handled = true;
            if (f_stored != Key_NoKey) {
              f_stored = mapped_key;
              return EventHandlerResult::OK;
            } else {
              stored_modifiers = Keyboard.lastKeyReport.modifiers;
              f_stored = mapped_key;
              return EventHandlerResult::EVENT_CONSUMED;
            }
          } else if (col == 3) {
            if (f_stored != Key_NoKey) {
              f_stored = Key_NoKey;
              d_handled = true;
              typeKey(Key_Escape, Keyboard.lastKeyReport.modifiers, true); 
              return EventHandlerResult::EVENT_CONSUMED;
            } else {
              return EventHandlerResult::OK;
            }
          }
        }

        // interrupted
        if (f_stored != Key_NoKey) {
          typeKey(f_stored, stored_modifiers, true);
          f_stored = Key_NoKey;
        }
        return EventHandlerResult::OK;
      } else if (keyIsPressed(key_state) && keyWasPressed(key_state)) {
        if (row == 2) {
          if (col == 4) {
            if (f_handled) {
              return EventHandlerResult::EVENT_CONSUMED;
            }
          } else if (col == 3) {
            if (d_handled) {
              return EventHandlerResult::EVENT_CONSUMED;
            }
          }
        }
      } else if (keyToggledOff(key_state)) {
        if (row == 2) {
          if (col == 4) {
            if (f_stored != Key_NoKey) {
              typeKey(f_stored, stored_modifiers, true);
              f_stored = Key_NoKey;
            }
            f_handled = false;
          } else if (col == 3) {
            d_handled = false;
          }
        }
      }
    }
    return EventHandlerResult::OK;
  }

  EventHandlerResult FDEscape::afterEachCycle() {
    if (Kaleidoscope.millisAtCycleStart() - start_time > time_out) {
      f_handled = false;
      if (f_stored != Key_NoKey) {
        typeKey(f_stored, stored_modifiers, false);
        f_stored = Key_NoKey;
      }
    }
  }

  class LEDStatus : public kaleidoscope::Plugin {
    public: 
      LEDStatus() {}
      
      EventHandlerResult afterEachCycle();
    private:
      static bool cleaned;
  };

  bool LEDStatus::cleaned = true;
  EventHandlerResult LEDStatus::afterEachCycle() {
    cRGB alert_color = CRGB(0, 0, 0);
    bool draw = true;
    if (Layer.isOn(STENO)) {
      cleaned = false;
      alert_color = CRGB(0, 255, 0);
    } else if (Layer.isOn(FUNCTION)) {
      cleaned = false;
      alert_color = CRGB(255, 255, 255);
    } else if (!cleaned) {
      cleaned = true;
    } else {
      draw = false;
    }
  
    if (draw) {
      for (uint8_t c = 0; c < 16; c++) {
        if (c != 7 && c != 8) {
          KeyboardHardware.setCrgbAt(0, c, alert_color);
        }
      }
    }
  }
}

kaleidoscope::FDEscape FDEscape;
kaleidoscope::LEDStatus LEDStatus;

enum { TOGGLE_STENO, TOGGLE_PROTOCOL };

void toggleSteno(uint8_t combo_index) {
  if (Layer.isOn(STENO)) {
    Layer.off(STENO);
  } else {
    Layer.on(STENO);
  }
}

/** A tiny wrapper, to be used by MagicCombo.
 * This simply toggles the keyboard protocol via USBQuirks, and wraps it within
 * a function with an unused argument, to match what MagicCombo expects.
 */
static void toggleKeyboardProtocol(uint8_t combo_index) {
  USBQuirks.toggleKeyboardProtocol();
}


USE_MAGIC_COMBOS(
  [TOGGLE_STENO] = {.action = toggleSteno, .keys = {R3C6, R3C9, R2C2}},
  [TOGGLE_PROTOCOL] = {.action = toggleKeyboardProtocol, .keys = {R3C6, R3C9, R1C14}});

// First, tell Kaleidoscope which plugins you want to use.
// The order can be important. For example, LED effects are
// added in the order they're listed here.
KALEIDOSCOPE_INIT_PLUGINS(
  LEDControl,
  StalkerEffect,
  MouseKeys,
  HostPowerManagement,
  USBQuirks,
  MagicCombo, 
  Macros,
  TapDance,
  FDEscape,
  LEDStatus,
  GeminiPR
);

/** The 'setup' function is one of the two standard Arduino sketch functions.
 * It's called when your keyboard first powers up. This is where you set up
 * Kaleidoscope and any plugins.
 */
void setup() {
  // First, call Kaleidoscope's internal setup function
  Kaleidoscope.setup();
  
  StalkerEffect.variant = STALKER(BlazingTrail);
  StalkerEffect.activate();
}

/** loop is the second of the standard Arduino sketch functions.
  * As you might expect, it runs in a loop, never exiting.
  *
  * For Kaleidoscope-based keyboard firmware, you usually just want to
  * call Kaleidoscope.loop(); and not do anything custom here.
  */

void loop() {
  Kaleidoscope.loop();
}
