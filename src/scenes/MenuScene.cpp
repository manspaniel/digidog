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
  arrowSprite->y = 8;
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
      menuIndex = 0;
    }
  }
  
  // Move the arrow sprite
  arrowSprite->y = 8 + menuIndex * 8;
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