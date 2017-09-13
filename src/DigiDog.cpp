#include <Arduino.h>
#include "DigiDog.h"

#define callOnIfScene(SCENE_ID, LOCAL_NAME, FUNC)     if (currentScene == SCENE_ID) { LOCAL_NAME.FUNC(&display); }

DigiDog::DigiDog() {
  display.tileset = TILESET;
  // display.addSprite(scene_room);
  
  // display.update();
  // currentScene = HomeScene;
}

void DigiDog::goToScene(SceneID scene) {
  currentScene = scene;
  // callOnIfScene(HOME_SCENE, homeScene, reset)
  // callOnIfScene(MENU_SCENE, menuScene, reset)
  // if (currentScene == HOME_SCENE) { homeScene.reset(&display); }
  homeScene.reset(&display);
}

void DigiDog::start() {
  goToScene(HOME_SCENE);
}

void DigiDog::loop() {
  homeScene.loop(&display);
  // callOnIfScene(HOME_SCENE, homeScene, loop)
  // callOnIfScene(MENU_SCENE, menuScene, loop)
  display.update();
}