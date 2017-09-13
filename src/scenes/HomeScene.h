#ifndef HOME_SCENE_H
#define HOME_SCENE_H

#include "SpriteDisplay.h"

struct HomeScene {
  int8_t x;
  int8_t y;
  int8_t xDir;
  int8_t yDir;
  Sprite * dogSprite;
  bool wagged = false;
  uint8_t ticksTillAlt = 0;
  
  void reset (SpriteDisplay * display);
  void loop (SpriteDisplay * display);
};

#endif