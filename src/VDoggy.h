#include <Arduino.h>
#include "ssd1306xled.h"
#include "SpriteDisplay.h"
#include "bitmaps.h"
#include "tiles.h"

enum ButtonID {
  NO_BUTTON,
  MENU_BUTTON,
  OK_BUTTON,
  CANCEL_BUTTON
};

const unsigned int frameDuration = 10;

class VDoggy {
public:
  SpriteDisplay display;
  
  ButtonID buttonDown = NO_BUTTON;
  ButtonID buttonUp = NO_BUTTON;
  ButtonID buttonHeld = NO_BUTTON;
  
  Sprite * sprite1;
  Sprite * sprite2;
  
  int xDir = 1;
  int yDir = 1;
  
  VDoggy () {
    display.tileset = TILESET;
    
    sprite1 = display.addSprite(scene_room);
    sprite1->x = 0;
    sprite1->y = 0;
    
    sprite2 = display.addSprite(walk_facing_1);
    sprite2->x = 16;
    sprite2->y = 0;
    
  }
  
  float t = 0;
  
  void loop () {
    // sprite2->x += xDir;
    // sprite2->y += yDir;
    
    if (buttonDown == MENU_BUTTON) {
      sprite2->y--;
      if (sprite2->y <= 0) sprite2->y = 0;
    } else if (buttonDown == CANCEL_BUTTON) {
      sprite2->y++;
    } else if (buttonDown == OK_BUTTON) {
      sprite2->y = 0;
    }
    
    sprite2->y += yDir;
    sprite2->x += xDir;
    if (sprite2->x >= 96) {
      xDir = -1;
    } else if (sprite2->x <= 0) {
      xDir = 1;
    }
    
    if (sprite2->y > 32) {
      yDir = -1;
    } else if (sprite2->y <= 0) {
      yDir = 1;
      sprite2->y = 0;
    }
    
    display.renderRect(0, 0, 128, 64);
    
    // if (sprite2->y == 0) {
    //   delay(500);
    // }
    
    // ssd1306_char_f8x16(64, 0, "---");
    //
    // char buffer[10];         //the ASCII of the integer will be stored in this char array
    // itoa(sprite2->y, buffer, 10);
    // ssd1306_char_f8x16(64, 0, buffer);
  }
  
};