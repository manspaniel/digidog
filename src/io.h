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
bool wasButtonDown (ButtonID);
bool wasButtonUp (ButtonID);
bool wasButtonHeld (ButtonID);

#endif