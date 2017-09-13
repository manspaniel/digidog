#ifndef DIGIDOG_H
#define DIGIDOG_H

#include <Arduino.h>
#include "ssd1306xled.h"
#include "SpriteDisplay.h"
#include "tiles.h"

#include "scenes/HomeScene.h"
#include "scenes/MenuScene.h"

enum SceneID {
  NO_SCENE,
  HOME_SCENE,
  MENU_SCENE
};

const unsigned int frameDuration = 10;

class DigiDog {
public:
  SpriteDisplay display;
  
  SceneID currentScene = NO_SCENE;
  
  // Scenes
  HomeScene homeScene;
  MenuScene menuScene;
  
  DigiDog();
  void start();
  void loop();
  void goToScene (SceneID scene);
  
};

// const DigiDog dogInstance;

#endif