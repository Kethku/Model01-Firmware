// -*- mode: c++ -*-

#include "Kaleidoscope.h"
#include "Kaleidoscope-MouseKeys.h"
#include "Kaleidoscope-Macros.h"
#include "Kaleidoscope-LEDControl.h"
#include "LED-Off.h"
#include "Kaleidoscope-LED-Stalker.h"
#include "Kaleidoscope-HostPowerManagement.h"
#include "Kaleidoscope-TapDance.h"
#include "Kaleidoscope-Qukeys.h"

enum { LEFT_BRACKET, RIGHT_BRACKET, ESCAPE };
enum { QWERTY, FUNCTION };

// *INDENT-OFF*

const Key keymaps[][ROWS][COLS] PROGMEM = {

  [QWERTY] = KEYMAP_STACKED
  (___,          Key_1, Key_2, Key_3, Key_4, Key_5, ___,
   Key_Backtick, Key_Q, Key_W, Key_E, Key_R, Key_T, TD(LEFT_BRACKET),
   Key_Escape,   Key_A, Key_S, Key_D, Key_F, Key_G,
   Key_PageDown, Key_Z, Key_X, Key_C, Key_V, Key_B, Key_Tab,
   Key_LeftAlt,  Key_Backspace, Key_LeftControl, Key_LeftShift,
   ShiftToLayer(FUNCTION),

   ___,   Key_6, Key_7, Key_8,     Key_9,         Key_0,         ___,
   TD(RIGHT_BRACKET), Key_Y, Key_U, Key_I,     Key_O,         Key_P,         Key_Equals,
          Key_H, Key_J, Key_K,     Key_L,         Key_Semicolon, Key_Quote,
   ___,   Key_N, Key_M, Key_Comma, Key_Period,    Key_Slash,     Key_Minus,
   Key_RightShift, Key_Enter, Key_Spacebar, Key_LeftGui,
   ShiftToLayer(FUNCTION)),

  [FUNCTION] =  KEYMAP_STACKED
  (___,      Key_F1,           Key_F2,      Key_F3,     Key_F4,        Key_F5,           XXX,
   Key_Tab,  ___,              Key_mouseUp, ___,        Key_mouseBtnR, Key_mouseWarpEnd, Key_mouseWarpNE,
   Key_Home, Key_mouseL,       Key_mouseDn, Key_mouseR, Key_mouseBtnL, Key_mouseWarpNW,
   Key_End,  Key_PrintScreen,  Key_Insert,  ___,        Key_mouseBtnM, Key_mouseWarpSW,  Key_mouseWarpSE,
   ___, Key_Delete, ___, ___,
   ___,

   Consumer_ScanPreviousTrack, Key_F6,                 Key_F7,                   Key_F8,                   Key_F9,          Key_F10,          Key_F11,
   Consumer_PlaySlashPause,    Consumer_ScanNextTrack, Key_LeftCurlyBracket,     Key_RightCurlyBracket,    Key_LeftBracket, Key_RightBracket, Key_F12,
                               Key_LeftArrow,          Key_DownArrow,            Key_UpArrow,              Key_RightArrow,  ___,              ___,
   Key_PcApplication,          Consumer_Mute,          Consumer_VolumeDecrement, Consumer_VolumeIncrement, ___,             Key_Backslash,    Key_Pipe,
   ___, ___, ___, ___,
   ___)

};

// *INDENT-ON*

const macro_t *macroAction(uint8_t macroIndex, uint8_t keyState) {
  switch (macroIndex) {
  }
  return MACRO_NONE;
}

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

void hostPowerManagementEventHandler(kaleidoscope::HostPowerManagement::Event event) {
  toggleLedsOnSuspendResume(event);
}

void setup() {
  Kaleidoscope.setup();

  Kaleidoscope.use(
    &Qukeys,
    &LEDControl,
    &StalkerEffect,
    &Macros,
    &MouseKeys,
    &HostPowerManagement,
    &TapDance
  );

  StalkerEffect.variant = STALKER(Haunt, (CRGB(0, 128, 0)));
  HostPowerManagement.enableWakeup();
  LEDOff.activate();
}

void loop() {
  Kaleidoscope.loop();
}
