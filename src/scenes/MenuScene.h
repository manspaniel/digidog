#ifndef MENU_SCENE_H
#define MENU_SCENE_H

#include "SpriteDisplay.h"

enum MenuName {
  MENU_MAIN,
  MENU_TAKE,
  MENU_CLEAN,
  MENU_FEED
};

struct MenuScene {
  Sprite * bgSprite;
  Sprite * arrowSprite;
  
  bool arrowFlip = false;
  bool fastArrow = false;
  char ticksTillAlt = 0;
  
  char totalItems = 0;
  char menuIndex = 0;
  char currentMenu = MENU_MAIN;
  
  void reset ();
  void loop ();
  
  void setMenu (MenuName menu);
  void selectedItem (MenuName menu, char index);
  
  void drawMenu (const char * items[]);
  void redraw ();
};

#endif