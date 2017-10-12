#include "HomeScene.h"
#include "io.h"
#include "DigiDog.h"

// #include "SpriteDisplay.h"

extern SpriteDisplay mainDisplay;
extern DigiDog doggy;

void HomeScene::reset () {
  x = 1;
  y = 1;
  xDir = 1;
  yDir = 1;
  ticksTillAlt = 5;
  dogSprite = mainDisplay.addSprite(walk_facing_1);
  dogSprite->y = 24;
  mainDisplay.addSprite(scene_room);
};

void HomeScene::loop () {
  
  if (buttonWasUp(MENU_BUTTON)) {
    // x = 1;
    doggy.goToScene(MENU_SCENE);
    return;
    // goToScene
  } else if (buttonWasHeld(OK_BUTTON)) {
    mainDisplay.wipe();
  }
  
  //
  if (x < 0) {
    xDir = 1;
    dogSprite->y = 24;
  } else if (x > 80) {
    xDir = -1;
    dogSprite->y = 30;
  }
  x += xDir;
  y += yDir;
  //
  ticksTillAlt--;
  if (ticksTillAlt == 0) {
    ticksTillAlt = 5;
    wagged = !wagged;
    dogSprite->data = wagged ? walk_facing_1 : walk_facing_2;
  }
  dogSprite->x = 8 + x;
  // dogSprite->y = 24;
  dogSprite->needsUpdate = true;
};