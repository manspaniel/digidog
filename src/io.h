#ifndef IO_H
#define IO_H

// Buttons
enum ButtonID {
  NO_BUTTON,
  MENU_BUTTON,
  OK_BUTTON,
  CANCEL_BUTTON
};

void updateButtonState ();
bool buttonWasDown (ButtonID);
bool buttonWasUp (ButtonID);
bool buttonWasHeld (ButtonID);

#endif