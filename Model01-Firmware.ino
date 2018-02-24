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

enum { LEFT_BRACKET, RIGHT_BRACKET, ESCAPE };
enum { QWERTY, GAMING, STENO, FUNCTION};

// *INDENT-OFF*

const Key keymaps[][ROWS][COLS] PROGMEM = {

  [QWERTY] = KEYMAP_STACKED
  (XXX,           Key_1, Key_2, Key_3, Key_4, Key_5, XXX,
   Key_Backtick,  Key_Q, Key_W, Key_E, Key_R, Key_T, TD(LEFT_BRACKET),
   Key_Escape,    Key_A, Key_S, Key_D, Key_F, Key_G,
   Key_LeftShift, Key_Z, Key_X, Key_C, Key_V, Key_B, Key_Tab,
   
   Key_LeftAlt, Key_Backspace, Key_LeftControl, Key_LeftShift,
   ShiftToLayer(FUNCTION),

   XXX,               Key_6, Key_7, Key_8,     Key_9,         Key_0,         XXX,
   TD(RIGHT_BRACKET), Key_Y, Key_U, Key_I,     Key_O,         Key_P,         Key_Equals,
                      Key_H, Key_J, Key_K,     Key_L,         Key_Semicolon, Key_Quote,
   XXX,               Key_N, Key_M, Key_Comma, Key_Period,    Key_Slash,     Key_Minus,
   
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
   XXX, XXX,        Key_mouseUp, Key_mouseScrollUp, Key_mouseBtnR, XXX,    XXX,
   XXX, Key_mouseL, Key_mouseDn, Key_mouseR,        Key_mouseBtnL, XXX,
   XXX, XXX,        XXX,         Key_mouseScrollDn, Key_mouseBtnM, XXX,    LockLayer(GAMING),
   
   Key_LeftAlt, Key_Delete, Key_LeftControl, Key_LeftShift,
   ShiftToLayer(FUNCTION),

   XXX,               Key_F6,               Key_F7,               Key_F8,             Key_F9,                Key_F10,       Key_F11,
   LockLayer(STENO),  LCTRL(Key_LeftArrow), LCTRL(Key_DownArrow), LCTRL(Key_UpArrow), LCTRL(Key_RightArrow), XXX,           Key_F12,
                      Key_LeftArrow,        Key_DownArrow,        Key_UpArrow,        Key_RightArrow,        XXX,           XXX,
   LockLayer(QUERTY), Key_Home,             Key_PageDown,         Key_PageUp,         Key_End,               Key_Backslash, Key_Pipe,
   
   Key_RightShift, Key_Enter, Key_Tab, Key_RightGui,
   ShiftToLayer(FUNCTION)),

};

// *INDENT-ON*

static bool primed = false;
void tapDanceAction(uint8_t tap_dance_index, byte row, byte col, uint8_t tap_count, kaleidoscope::TapDance::ActionType tap_dance_action) {
  switch (tap_dance_index) {
    case LEFT_BRACKET:
      return tapDanceActionKeys(tap_count, tap_dance_action, Key_LeftParen, Key_LeftCurlyBracket, Key_LeftBracket);
    case RIGHT_BRACKET:
      return tapDanceActionKeys(tap_count, tap_dance_action, Key_RightParen, Key_RightCurlyBracket, Key_RightBracket);
    case ESCAPE:
      if (tap_dance_action == kaleidoscope::TapDance::Interrupt && primed) {
        kaleidoscope::hid::pressKey(Key_F);
        primed = false;
      }
      if (tap_dance_action == kaleidoscope::TapDance::Timeout && primed) {
        kaleidoscope::hid::pressKey(Key_F);
        primed = false;
      }

      if (tap_dance_action == kaleidoscope::TapDance::Tap) {
        if (col == 4 && row == 2) {
          if (primed) {
            kaleidoscope::hid::pressKey(Key_F);
            kaleidoscope::hid::pressKey(Key_F);
            primed = false;
          } else {
            primed = true;
          }
        } else if (col == 3 && row == 2) {
          if (primed) {
            kaleidoscope::hid::pressKey(Key_Escape);
          } else {
            kaleidoscope::hid::pressKey(Key_D);
          }
          primed = false; 
        }
      }
  }
}


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
    kaleidoscope::Qukey(0, 3, 15, Key_RightShift)
  );

  StalkerEffect.variant = STALKER(Haunt, (CRGB(0, 255, 0)));
  HostPowerManagement.enableWakeup();
  LEDOff.activate();
}

void loop() {
  Kaleidoscope.loop();
}
