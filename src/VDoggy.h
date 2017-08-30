#include <Arduino.h>
#include "ssd1306xled.h"
#include "bitmaps.h"
#include "tiles.h"

enum DogMode {
  PET,
  IN_MENU
};

enum ButtonID {
  NO_BUTTON,
  MENU_BUTTON,
  OK_BUTTON,
  CANCEL_BUTTON
};

enum DogAnim {
  IDLE,
  WALKING,
  FEEDING,
  CLEANING
};

enum ModePhase {
  START,
  END
};

const unsigned int frameDuration = 10;

class VDoggy {
public:
  bool displayIsOff = false;
  bool waitingForSleep = false;
  uint8_t lastDrawnX = 0;
  uint8_t x = 0;
  uint8_t y = 0;
  
  ButtonID buttonDown = NO_BUTTON;
  ButtonID buttonUp = NO_BUTTON;
  ButtonID buttonHeld = NO_BUTTON;
  
  // int timeTillNextTick = 0;
  bool justChangedMode = false;
  bool justChangedAnim = false;
  DogMode currentMode = PET;
  DogAnim currentAnim = IDLE;
  
  // IDLE animation
  int idle_timeTillWagFlip = 0;
  bool idle_wagState = 0;
  
  // WALK anim
  int walk_startY = 0;
  int walk_startX = 0;
  int walk_targetX = 0;
  int walk_targetY = 0;
  int walk_timeTillNextStep = 0;
  int walk_timeTillWalkAlt = 0;
  bool walk_flipped = false;
  bool walk_alt = false;
  
  VDoggy () {
    enterMode(PET);
    playAnim(IDLE);
    clearScreen();
  }
  
  void tickMainDisplay () {
    
  }
  
  void goToSleep () {
    
  }
  
  void enterMode (DogMode mode) {
    justChangedMode = true;
    justChangedAnim = true;
    currentMode = mode;
  }
  
  void playAnim (DogAnim anim) {
    justChangedMode = currentMode != PET;
    justChangedAnim = true;
    currentMode = PET;
    currentAnim = anim;
  }
  
  void loop () {
    
    delay(frameDuration);
    // timeTillNextTick -= frameDuration;
    
    if (currentMode == PET) {
      bool isFirstFrame = justChangedAnim;
      justChangedAnim = false;
      drawAnimFrame(currentAnim, isFirstFrame);
      
      if (buttonDown == OK_BUTTON) {
        walk_flipped = !walk_flipped;
      } else if (buttonDown == CANCEL_BUTTON) {
        x = 0;
        y = 0;
      }
    }
    
    buttonUp = NO_BUTTON;
    buttonDown = NO_BUTTON;
    // ssd1306_char_f8x16(1, 2, "zzzzz");
    
  }
  
  void drawAnimFrame(DogAnim anim, bool firstFrame) {
    if (anim == IDLE) {
      // IDLE ANIMATION
      
      if (shouldWalk()) {
        playAnim(WALKING);
      }
      
      idle_timeTillWagFlip -= frameDuration;
      if (idle_timeTillWagFlip <= 0 || firstFrame) {
        idle_timeTillWagFlip = 500;
        idle_wagState = !idle_wagState;
        drawDog(idle_wagState ? stand_left_wag : stand_left);
      }
      // timeTillNextTick = 10;
    } else if (anim == WALKING) {
      // WALK ON SCREEN
      if (firstFrame) {
        walk_startY = y;
        walk_startX = x;
        walk_targetX = random(80);
        walk_targetY = random(16);
        if (x < walk_targetX) {
          walk_flipped = true;
        } else {
          walk_flipped = false;
        }
      }
      
      walk_timeTillNextStep -= frameDuration;
      walk_timeTillWalkAlt -= frameDuration;
      if (walk_timeTillNextStep <= 0 || firstFrame) {
        
        if (walk_timeTillWalkAlt <= 0) {
          walk_alt = !walk_alt;
          walk_timeTillWalkAlt = 250;
        }
        
        if (x > walk_targetX) {
          x -= 1;
          if (x <= walk_targetX) {
            x = walk_targetX;
          }
        } else if (x < walk_targetX) {
          x += 1;
          if (x >= walk_targetX) {
            x = walk_targetX;
          }
        }
        y = walk_startY + (walk_targetY - walk_startY) * (1 - (float)(walk_targetX - x) / (float)(walk_targetX - walk_startX));
        
        drawDog(walk_alt ? walk_facing_1 : walk_facing_2);
        walk_timeTillNextStep = 50;
        
        if (x == walk_targetX) {
          playAnim(IDLE);
        }
      }
      // timeTillNextTick = 10;
    }
  }
  
  void drawDog (uint8_t sprite[], SpriteEffect effect) {
    uint8_t startClear;
    if (x > lastDrawnX) {
      // Clear the section in front of the next sprite
      ssd1306_fill_range(lastDrawnX, x, 0x00);
    } else if(x < lastDrawnX) {
      // Clear the section at the back of the sprite
      ssd1306_fill_range(x + 32, lastDrawnX + 32, 0x00);
    }
    lastDrawnX = x;
    drawSprite(x, y, sprite, effect);
    //ssd1306_draw_bmp(x, y, 48, 48, sprite, flipped);
  }
  
  void drawDog (uint8_t sprite[]) {
    drawDog(sprite, walk_flipped ? FLIP_X : NO_EFFECT);
  }
  
  bool shouldWalk () {
    return random(1000) < 5;
  }
  
  void clearScreen () {
    ssd1306_fillscreen(0);
  }
  
};