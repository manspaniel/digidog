#ifndef DIGIDOG_H
#define DIGIDOG_H

#include <Arduino.h>
#include "ssd1306xled.h"
#include "SpriteDisplay.h"
#include "tiles.h"

#include "scenes/HomeScene.h"
#include "scenes/MenuScene.h"

enum SceneID {
  NO_SCENE = 0,
  HOME_SCENE = 1,
  MENU_SCENE = 2
};

static char currentScene = HOME_SCENE;

const unsigned int frameDuration = 10;

class DigiDog {
public:
  
  bool haveStarted = false;
  
  // Scenes
  HomeScene homeScene;
  MenuScene menuScene;
  
  DigiDog();
  void start();
  void loop();
  void goToScene (char scene);
  
};

// static DigiDog * dogInstance;

#endif