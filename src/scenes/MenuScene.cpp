#include "MenuScene.h"
#include "io.h"
#include "DigiDog.h"
#include "strings.h"
#include <avr/pgmspace.h>

const char STR_FEED [] PROGMEM = {"Feed"};
const char STR_GO [] PROGMEM = {"Go"};
const char STR_CLEAN [] PROGMEM = {"Clean"};
const char STR_PLAY [] PROGMEM = {"Play"};
const char STR_TRAIN [] PROGMEM = {"Train"};

const char * mainMenu [] = { STR_FEED, STR_GO, STR_CLEAN, STR_PLAY, STR_TRAIN, NULL };

extern SpriteDisplay mainDisplay;
extern DigiDog doggy;

void MenuScene::reset () {
  arrowSprite = mainDisplay.addSprite(ui_selection_arrow);
  arrowSprite->x = 8;
  arrowSprite->y = 0;
  bgSprite = mainDisplay.addSprite(ui_menu_frame);
  setMenu(MENU_MAIN);
};
  
void MenuScene::loop () {
  if (buttonWasUp(CANCEL_BUTTON)) {
    doggy.goToScene(HOME_SCENE);
    return;
  }
  
  if (buttonWasDown(MENU_BUTTON)) {
    menuIndex++;
    if (menuIndex > totalItems) {
      fastArrow = true;
      menuIndex = 0;
    }
  } else if (buttonWasHeld(OK_BUTTON)) {
    mainDisplay.wipe();
  }
  
  // Move the arrow sprite
  char arrowTarget = 8 + menuIndex * 8;
  if (arrowSprite->y == 0) {
    arrowFlip = false;
    ticksTillAlt = 0;
    arrowSprite->y = arrowTarget;
    arrowSprite->x = 8;
  } else if (arrowTarget < arrowSprite->y) {
    delay(fastArrow ? 2 : 15);
    arrowFlip = false;
    ticksTillAlt = 0;
    arrowSprite->y--;
    arrowSprite->x = 8;
  } else if (arrowTarget > arrowSprite->y) {
    delay(fastArrow ? 2 : 15);
    arrowFlip = false;
    ticksTillAlt = 0;
    arrowSprite->y++;
    arrowSprite->x = 8;
  } else {
    fastArrow = false;
    if (ticksTillAlt == 0) {
      arrowFlip = !arrowFlip;
      ticksTillAlt = 200;
    } else {
      ticksTillAlt--;
    }
  }
  arrowSprite->data = arrowFlip ? ui_selection_arrow : ui_selection_arrow_alt;
  arrowSprite->needsUpdate = true;
};

void MenuScene::redraw () {
  bgSprite->needsUpdate = true;
  mainDisplay.update();
  
  if (currentMenu == MENU_MAIN) {
    drawMenu(mainMenu);
  }
}

void MenuScene::drawMenu (const char * items[]) {
  int index = 0;
  while (true) {
    const char * item = items[index];
    if (item == NULL) {
      break;
    }
    mainDisplay.writeText(16, 8 + index * 8, item);
    totalItems = index;
    index++;
  }
}

void MenuScene::setMenu (MenuName menu) {
  currentMenu = menu;
  menuIndex = 0;
  redraw();
}

void selectedItem (MenuName menu, char index) {
  
}