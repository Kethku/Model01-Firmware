// -*- mode: c++ -*-

#include "Kaleidoscope.h"
#include "Kaleidoscope-MouseKeys.h"
#include "Kaleidoscope-LEDControl.h"
#include "Kaleidoscope-LED-Stalker.h"
#include "Kaleidoscope-HostPowerManagement.h"
#include "Kaleidoscope-TapDance.h"
#include "Kaleidoscope-Qukeys.h"
#include "Kaleidoscope-Steno.h"
#include "LED-Off.h"
#include "kaleidoscope/hid.h"
#include "MultiReport/Keyboard.h"
#include "key_defs_keymaps.h"

enum { LEFT_BRACKET, RIGHT_BRACKET };
enum { QWERTY, GAMING, STENO, FUNCTION };
enum { MACRO_F, MACRO_D };

// *INDENT-OFF*

const Key keymaps[][ROWS][COLS] PROGMEM = {

  [QWERTY] = KEYMAP_STACKED
  (XXX,          Key_1, Key_2, Key_3, Key_4, Key_5, Key_Equals,
   Key_Tab,      Key_Q, Key_W, Key_E, Key_R, Key_T, TD(LEFT_BRACKET),
   Key_Escape,   Key_A, Key_S, Key_D, Key_F, Key_G,
   Key_Backtick, Key_Z, Key_X, Key_C, Key_V, Key_B, XXX,
   
   Key_LeftAlt, Key_Backspace, Key_LeftControl,  Key_LeftShift,
   ShiftToLayer(FUNCTION),

   Key_Minus,         Key_6, Key_7, Key_8,     Key_9,         Key_0,         XXX,
   TD(RIGHT_BRACKET), Key_Y, Key_U, Key_I,     Key_O,         Key_P,         Key_Backslash,
                      Key_H, Key_J, Key_K,     Key_L,         Key_Semicolon, Key_Quote,
   XXX,               Key_N, Key_M, Key_Comma, Key_Period,    Key_Slash,     XXX,
   
 Key_RightShift, Key_Enter, Key_Spacebar, Key_LeftGui,
 ShiftToLayer(FUNCTION)),

[GAMING] = KEYMAP_STACKED
(___, ___, ___, ___, ___, ___, ___,
 ___, ___, ___, ___, ___, ___, ___,
 ___, ___, ___, ___, ___, ___,
 ___, ___, ___, ___, ___, ___, ___,
 
 Key_Space, Key_Backspace, ___, ___,
 ___,

 ___, ___, ___, ___, ___, ___, ___,
 ___, ___, ___, ___, ___, ___, ___,
 ___, ___, ___, ___, ___, ___,
 ___, ___, ___, ___, ___, ___, ___,

 ___, ___, ___, ___,
 ___),

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
(XXX, Key_F1,     Key_F2,      Key_F3,            Key_F4,        Key_F5, XXX,
 XXX, XXX, XXX,        Key_mouseUp, Key_mouseScrollUp, Key_mouseBtnR,    XXX,
 XXX, XXX, Key_mouseL, Key_mouseDn, Key_mouseR,        Key_mouseBtnL,
 XXX, XXX, XXX,        XXX,         Key_mouseScrollDn, Key_mouseBtnM,    LockLayer(GAMING),
 
 Key_LeftAlt, Key_Delete, Key_LeftControl, Key_LeftShift,
 ShiftToLayer(FUNCTION),

XXX,               Key_F6,               Key_F7,               Key_F8,             Key_F9,                Key_F10, Key_F11,
LockLayer(STENO),  LCTRL(Key_LeftArrow), LCTRL(Key_DownArrow), LCTRL(Key_UpArrow), LCTRL(Key_RightArrow), XXX,     Key_F12,
Key_LeftArrow,        Key_DownArrow,        Key_UpArrow,        Key_RightArrow,        XXX,     XXX,
LockLayer(QWERTY), Key_Home,             Key_PageDown,         Key_PageUp,         Key_End,               XXX,     XXX,

Key_RightShift, Key_Enter, Key_Tab, Key_RightGui,
ShiftToLayer(FUNCTION)),

};

// *INDENT-ON*


void hostPowerManagementEventHandler(kaleidoscope::HostPowerManagement::Event event) {
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

static bool primed = false;
void tapDanceAction(uint8_t tap_dance_index, byte row, byte col, uint8_t tap_count, kaleidoscope::TapDance::ActionType tap_dance_action) {
switch (tap_dance_index) {
    case LEFT_BRACKET:
      return tapDanceActionKeys(tap_count, tap_dance_action, Key_LeftParen, Key_LeftCurlyBracket, Key_LeftBracket);
    case RIGHT_BRACKET:
      return tapDanceActionKeys(tap_count, tap_dance_action, Key_RightParen, Key_RightCurlyBracket, Key_RightBracket);
  }
}

bool skip = false;
void typeKey(Key key, uint8_t modifiers) {
  HID_KeyboardReport_Data_t hid_report;
  memcpy(hid_report.allkeys, Keyboard.lastKeyReport.allkeys, sizeof(hid_report));
  Keyboard.keyReport.modifiers = modifiers;
  skip = true;
  handleKeyswitchEvent(key, UNKNOWN_KEYSWITCH_LOCATION, IS_PRESSED | INJECTED);
  kaleidoscope::hid::sendKeyboardReport();
  skip = true;
  handleKeyswitchEvent(key, UNKNOWN_KEYSWITCH_LOCATION, WAS_PRESSED | INJECTED);
  kaleidoscope::hid::sendKeyboardReport();
  memcpy(Keyboard.keyReport.allkeys, hid_report.allkeys, sizeof(Keyboard.keyReport));
}

uint8_t stored_modifiers;
Key f_stored = Key_NoKey;
bool f_handeled = false;
bool d_handeled = false;
uint32_t start_time;
uint16_t time_out = 200;
bool debug = true;
Key eventHandlerHook(Key mapped_key, byte row, byte col, uint8_t key_state) {
  if (skip) {
    skip = false;
    return mapped_key;
  }
  if (Layer.isOn(QWERTY)) {
    if (keyToggledOn(key_state)) {
      if (row == 2) {
        if (col == 4) { // f column
          start_time = millis();
          f_handeled = true;
          if (f_stored != Key_NoKey) {
            if (debug) Serial.print("repeated\n");
            f_stored = mapped_key;
            return f_stored;
          } else {
            if (debug) Serial.print("first press\n");
            stored_modifiers = Keyboard.lastKeyReport.modifiers;
            f_stored = mapped_key;
            return Key_NoKey;
          }
        } else if (col == 3) { // d column
          if (f_stored != Key_NoKey) {
            if (debug) Serial.print("escaped\n");
            f_stored = Key_NoKey;
            d_handeled = true;
            return Key_Escape;
          } else {
            if (debug) Serial.print("d\n");
            return mapped_key;
          }
        }
      }

      // interrupted
      if (f_stored != Key_NoKey) {
        if (debug) Serial.print("interrupted\n");
        typeKey(f_stored, stored_modifiers);
        f_stored = Key_NoKey;
      }
      return mapped_key;
    } else if (keyIsPressed(key_state) && keyWasPressed(key_state)) {
      if (row == 2) {
        if (col == 4) {
          if (f_handeled) {
            if (debug) Serial.print("f suppressed\n");
            return Key_NoKey;
          }
        } else if (col == 3) {
          if (d_handeled) {
            if (debug) Serial.print("d suppressed\n");
            return Key_NoKey;
          }
        }
      }
    } else if (keyToggledOff(key_state)) {
      if (row == 2) {
        if (col == 4) {
          if (debug) Serial.print("f released\n");
          if (f_stored != Key_NoKey) {
            typeKey(f_stored, stored_modifiers);
            f_stored = Key_NoKey;
          }
          f_handeled = false;
        } else if (col == 3) {
          if (debug) Serial.print("d released\n");
          d_handeled = false;
        }
      }
    }
  }
  return mapped_key;
}

void loopHook(bool is_post_clear) {
  if (!is_post_clear)
    return;

  if (millis() - start_time > time_out) {
    f_handeled = false;
    d_handeled = false;
    if (f_stored != Key_NoKey) {
      if (debug) Serial.print("f timed out\n");
      typeKey(f_stored, stored_modifiers);
      f_stored = Key_NoKey;
    }
  }
}

void setup() {
  Serial.begin(9600);
  Kaleidoscope.setup();

  Kaleidoscope.use(
    &GeminiPR,
    &Qukeys,
    &LEDControl,
    &StalkerEffect,
    &HostPowerManagement,
    &TapDance,
    &MouseKeys
  );

  QUKEYS(
    kaleidoscope::Qukey(0, 3, 0, Key_LeftShift),
    kaleidoscope::Qukey(0, 3, 15, Key_RightShift),
    kaleidoscope::Qukey(0, 0, 0, Key_LeftShift)
  );

  StalkerEffect.variant = STALKER(Haunt, (CRGB(0, 255, 0)));
  HostPowerManagement.enableWakeup();
  LEDOff.activate();

  Kaleidoscope.useEventHandlerHook(eventHandlerHook);
  Kaleidoscope.useLoopHook(loopHook);
}

void loop() {
  Kaleidoscope.loop();
}
