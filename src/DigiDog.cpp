#include <Arduino.h>
#include "DigiDog.h"
#include "io.h"

extern SpriteDisplay mainDisplay;

#define callOnIfScene(SCENE_ID, LOCAL_NAME, FUNC)     if (currentScene == SCENE_ID) { LOCAL_NAME.FUNC(); }

DigiDog::DigiDog() {
  
}

void DigiDog::goToScene(char scene) {
  currentScene = scene;
  
  // Clear the screen of all sprites, and set the whole screen as "dirty" (to be re-rendered)
  mainDisplay.removeAllSprites();
  
  mainDisplay.wipe();
  delay(300);
  
  // Call reset on the new scene
  callOnIfScene(HOME_SCENE, homeScene, reset)
  callOnIfScene(MENU_SCENE, menuScene, reset)
}

void DigiDog::start() {
  haveStarted = true;
  goToScene(HOME_SCENE);
}

bool flipped = false;

void DigiDog::loop() {
  
  // Update the button state for this frame
  updateButtonState();
  
  // Call loop on the current scene
  callOnIfScene(HOME_SCENE, homeScene, loop);
  callOnIfScene(MENU_SCENE, menuScene, loop);
  mainDisplay.update();
}