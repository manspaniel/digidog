#ifndef SPRITE_DISPLAYM_H
#define SPRITE_DISPLAYM_H

#include <stdlib.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include "ssd1306xled.h"
#include "tiles.h"
#include "font6x8.h"

#define MAX_SPRITES               16
#define SPRITE_HEADER_SIZE        3

class SpriteDisplay;

struct Sprite {
  const uint8_t * data;
  bool active = false;
  uint8_t tileset = 0;
  uint8_t x = 0;
  uint8_t y = 0;
  uint8_t width = 0;
  uint8_t height = 0;
  char cols = 0;
  char rows = 0;
  
  uint8_t lastX = 0;
  uint8_t lastY = 0;
  uint8_t lastWidth = 0;
  uint8_t lastHeight = 0;
  
  bool hasRendered = false;
  bool needsUpdate = false;
};

class SpriteDisplay {
public:
  Sprite sprites[MAX_SPRITES];
  uint32_t dirtyCells[8];
  
  const uint8_t * tileset;
  
  SpriteDisplay();
  Sprite * addSprite (const uint8_t * spriteData);
  bool rectsOverlap (int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2);
  uint8_t getTileByte(Sprite * sprite, uint8_t x, uint8_t y, uint8_t shiftX, bool alpha);
  void setDirtyRect (uint8_t x, uint8_t y, uint8_t w, uint8_t h);
  void update ();
  void removeAllSprites ();
  void wipe ();
  void writeText(uint8_t x, uint8_t y, const char * str);
};

#endif