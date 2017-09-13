#ifndef MENU_SCENE_H
#define MENU_SCENE_H

#include "SpriteDisplay.h"

struct MenuScene {
  void reset (SpriteDisplay * display) {
    display->addSprite(ui_menu_frame);
  };
  void loop (SpriteDisplay * display) {
    
  };
};

#endif