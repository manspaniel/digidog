#include <Arduino.h>
#include "ssd1306xled.h"
#include "bitmaps.h"

enum DogMode {
  PET,
  IN_MENU
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
      // if (timeTillNextTick <= 0) {
        bool isFirstFrame = justChangedAnim;
        justChangedAnim = false;
        drawAnimFrame(currentAnim, isFirstFrame);
      // }
    }
    
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
        drawDog(idle_wagState ? dog_wag : dog_standing);
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
        
        drawDog(walk_alt ? dog_walk_1 : dog_walk_2);
        walk_timeTillNextStep = 50;
        
        if (x == walk_targetX) {
          playAnim(IDLE);
        }
      }
      // timeTillNextTick = 10;
    }
  }
  
  void drawDog (uint8_t bitmap[], bool flipped) {
    uint8_t startClear;
    if (x > lastDrawnX) {
      // Clear the section in front of the next bitmap
      ssd1306_fill_range(lastDrawnX, x, 0x00);
    } else if(x < lastDrawnX) {
      // Clear the section at the back of the bitmap
      ssd1306_fill_range(x + 48, lastDrawnX + 48, 0x00);
    }
    lastDrawnX = x;
    ssd1306_draw_bmp(x, y, 48, 48, bitmap, flipped);
  }
  
  void drawDog (uint8_t bitmap[]) {
    drawDog(bitmap, walk_flipped);
  }
  
  bool shouldWalk () {
    return random(1000) < 5;
  }
  
  void clearScreen () {
    ssd1306_fillscreen(0);
  }
  
};