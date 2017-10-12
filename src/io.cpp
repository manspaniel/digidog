#include "io.h"
#include <Arduino.h>

#define MIN_BUTTON_VALUE 100
#define MENU_BUTTON_VALUE 310
#define OK_BUTTON_VALUE 512
#define CANCEL_BUTTON_VALUE 1000

#define BUTTON_TICKS_DELAY 1

ButtonID getButtonNumber () {
  int val = analogRead(A2);
  if (val < MIN_BUTTON_VALUE) return NO_BUTTON;
  int menuDiff = abs(val - MENU_BUTTON_VALUE);
  int okDiff = abs(val - OK_BUTTON_VALUE);
  int cancelDiff = abs(val - CANCEL_BUTTON_VALUE);
  if (menuDiff < okDiff && menuDiff < cancelDiff) {
    return MENU_BUTTON;
  } else if (okDiff < menuDiff && okDiff < cancelDiff) {
    return OK_BUTTON;
  } else if (cancelDiff < menuDiff && cancelDiff < okDiff) {
    return CANCEL_BUTTON;
  }
  return NO_BUTTON;
}

ButtonID lastButton = NO_BUTTON;
uint8_t buttonCount = 0;

ButtonID _buttonDown = NO_BUTTON;
ButtonID _buttonUp = NO_BUTTON;
ButtonID _buttonHeld = NO_BUTTON;

void updateButtonState() {
  // Figure out the current button
  ButtonID val = getButtonNumber();
  if (lastButton != val) {
    // Button has changed, but wait a few cycles before notifying doggy
    buttonCount = 0;
    // Button up
    _buttonUp = _buttonHeld;
    _buttonDown = NO_BUTTON;
    _buttonHeld = NO_BUTTON;
  } else {
    // Button hasn't changed, but maybe we haven't notified
    if (buttonCount < BUTTON_TICKS_DELAY) {
      buttonCount++;
      if (buttonCount == BUTTON_TICKS_DELAY) {
        // Button was pressed
        _buttonUp = _buttonHeld;
        _buttonDown = val;
        _buttonHeld = val;
      }
    } else {
      // Button was held
      _buttonDown = NO_BUTTON;
      _buttonHeld = val;
      _buttonUp = NO_BUTTON;
    }
  }
  
  lastButton = val;
}

bool buttonWasDown (ButtonID button) {
  return (_buttonDown == button);
}

bool buttonWasUp (ButtonID button) {
  return (_buttonUp == button);
}

bool buttonWasHeld (ButtonID button) {
  return (_buttonHeld == button);
}