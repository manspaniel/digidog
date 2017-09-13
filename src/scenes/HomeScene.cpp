#include "HomeScene.h"
// #include "io.h"

void HomeScene::reset (SpriteDisplay * display) {
  x = 1;
  y = 1;
  xDir = 1;
  yDir = 1;
  ticksTillAlt = 5;
  dogSprite = display->addSprite(walk_facing_1);
  display->addSprite(scene_room);
};

void HomeScene::loop (SpriteDisplay * display) {
  
  // if (wasButtonDown(MENU_BUTTON)) {
  //   // dogInstance->goToScene(MENU_SCENE);
  //   // goToScene
  // }
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
  // // dogSprite->y = 24;
  dogSprite->needsUpdate = true;
};