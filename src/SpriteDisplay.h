#ifndef SPRITE_DISPLAY_H
#define SPRITE_DISPLAY_H

#include <stdlib.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include "ssd1306xled.h"
#include "tiles.h"

#define MAX_SPRITES    16
#define SPRITE_HEADER_SIZE 3

struct Sprite {
  bool active = false;
  bool filtered = false;
  uint8_t tileset = 0;
  uint8_t * data;
  int x = 0;
  int y = 0;
  uint8_t width = 0;
  uint8_t height = 0;
  uint8_t cols = 0;
  uint8_t rows = 0;
  int lastX = 0;
  int lastY = 0;
};

class SpriteDisplay {
public:
  uint8_t * tileset;
  Sprite sprites[MAX_SPRITES];
  
  Sprite * addSprite (uint8_t * spriteData) {
    Sprite * sprite;
    for (uint8_t k = 0; k < MAX_SPRITES; k++) {
      sprite = &sprites[k];
      if (!sprite->active) {
        sprite->active = true;
        sprite->data = spriteData;
        sprite->cols = pgm_read_byte(&spriteData[1]);
        sprite->rows = pgm_read_byte(&spriteData[2]);
        sprite->width = sprite->cols * 8;
        sprite->height = sprite->rows * 8;
        sprite->tileset = pgm_read_byte(&spriteData[0]);
        return &sprites[k];
      }
    }
    return NULL;
  }
  
  bool rectsOverlap (int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2) {
    return !(x1 + w1 < x2 || y1 + h1 < y2 || x1 > x2 + w2 || y1 > y2 + h2);
  }
  
  uint8_t getTileByte(Sprite * sprite, uint8_t x, uint8_t y, uint8_t shiftX) {
    
    // Grab the type reference byte, which contains flipX/flipY/ID
    uint8_t tileRef = pgm_read_byte(&sprite->data[SPRITE_HEADER_SIZE + y * sprite->cols + x]);
    
    // We flip?
    bool flipX = (tileRef & 0x80) == 0x80;
  	bool flipY = (tileRef & 0x40) == 0x40;
    
    // Calculate the index on the global tileset
    uint16_t tileIndex = sprite->tileset * 64 + (tileRef & 0x3f);
    
    // Read and return the cell byte, compensaiting for x offset
    uint8_t value = pgm_read_byte(&TILESET[tileIndex * 2 * 8 + (flipX ? 7 - shiftX : shiftX) * 2]);
    
    if (flipY) {
      value = (value & 0xF0) >> 4 | (value & 0x0F) << 4;
      value = (value & 0xCC) >> 2 | (value & 0x33) << 2;
      value = (value & 0xAA) >> 1 | (value & 0x55) << 1;
    }
    
    return value;
    
  }
  
  /*
    Render a rectangle to the display
  */
  void renderRect (int screenX, int screenY, int width, int height) {
    
    for (int spriteID = 0; spriteID < MAX_SPRITES; spriteID++) {
      Sprite * sprite = &sprites[spriteID];
      sprite->filtered = rectsOverlap(screenX, screenY, width, height, sprite->x, sprite->y, sprite->width, sprite->height);
    }
    
    for (uint8_t y = (screenY / 8) * 8; y < screenY + height; y += 8) {
      ssd1306_setpos(screenX, y / 8);
      ssd1306_send_data_start();
      for (uint8_t x = screenX; x < screenX + width; x++) {
        uint8_t val = 0;
        // Loop over each sprite which overlaps this rect
        for (int spriteID = 0; spriteID < MAX_SPRITES; spriteID++) {
          Sprite * sprite = &sprites[spriteID];
          if (!sprite->active) continue;
          if (sprite->y + sprite->height < y || sprite->y > y + 8) continue;
          if (sprite->x + sprite->width < x || sprite->x > x) continue;
          
          // Offset is the distance between the current cell and the start of the sprite.
          // It's always positive
          int offsetX = x - sprite->x;
          int offsetY = y - sprite->y;
          
          int shiftY = 0;
          int shiftX = offsetX % 8;
          
          // We're currently in a screen cell, which is 1x8 pixels
          // We've already determined that the current sprite overlaps that cell in some way
          // Either 1 or 2 tiles of this sprite are overlapping
          // - 1 tile taking up the entire space, if this sprite is aligned by 8
          // - 1 tile, offset downwards, if sprite.y > y
          // - 1 tile, offset upwards, if sprite.y + sprite.height > y && sprite.y < y + 8
          // - or 2 tiles otherwise
          //  - in this case, the topmost
          bool showFirst = false;
          uint8_t firstCol = 0;
          uint8_t firstRow = 0;
          
          bool showSecond = false;
          uint8_t secondCol = 0;
          uint8_t secondRow = 0;
          
          if (sprite->y % 8 == 0) {
            // The sprite is aligned by 8 on the Y axis, so only one to show, and no y shifting needed
            showFirst = true;
            firstCol = offsetX / 8;
            firstRow = offsetY / 8;
            if (firstRow == sprite->rows) {
              showFirst = false;
            }
          } else if (sprite->y > y) {
            // The sprite starts further down in this cell
            showFirst = true;
            firstCol = offsetX / 8;
            firstRow = 0;
            shiftY = sprite->y % 8;
          } else if (sprite->y + sprite->height < y + 8) {
            // The sprite ends during this cell
            showSecond = true;
            secondCol = offsetX / 8;
            secondRow = sprite->rows - 1;
            shiftY = sprite->y % 8;
          } else {
            // Both!
            showFirst = true;
            firstCol = offsetX / 8;
            firstRow = offsetY / 8 + 1;
            shiftY = sprite->y % 8;
            
            showSecond = true;
            secondCol = offsetX / 8;
            secondRow = offsetY / 8;
          }
          
          if (showFirst) {
            uint8_t cell = getTileByte(sprite, firstCol, firstRow, shiftX);
            val = val | (cell << shiftY);
          }
          if (showSecond) {
            uint8_t cell = getTileByte(sprite, secondCol, secondRow, shiftX);
            val = val | (cell >> (8 - shiftY));
          }
          
          // int spriteX = x - sprite->x;
          // int spriteY = y - sprite->y;
          // uint8_t offset = (sprite->y % 8);
          // int8_t tileX = spriteX / 8;
          // int8_t tileY = spriteY / 8;
          // uint8_t spriteCols = sprite->cols;
          // uint8_t spriteRows = sprite->rows;
          // int effect = 0;
    			// uint8_t first = y == 0 ? 0 : getTileByte(spriteX % 8, pgm_read_byte(&sprite->data[(tileY - 1) * spriteCols + (effect & FLIP_X ? spriteCols - tileX - 1 : tileX) + SPRITE_HEADER_SIZE]), sprite->tileset, effect & FLIP_X);
  				// uint8_t cur = y == spriteRows ? 0 : getTileByte(spriteX % 8, pgm_read_byte(&sprite->data[tileY * spriteCols + (effect & FLIP_X ? spriteCols - tileX - 1 : tileX) + SPRITE_HEADER_SIZE]), sprite->tileset, effect & FLIP_X);
          
          // bool skipFirst = false;//(tileY == 0 && offset > 0);
          //
          // uint8_t tileRef = skipFirst ? 0 : pgm_read_byte(&sprite->data[SPRITE_HEADER_SIZE + (offset == 0 ? tileY : tileY) * spriteCols + tileX]);
          // uint16_t tileIndex = skipFirst ? 0 : sprite->tileset * 64 + (tileRef & 0x3f);
          // uint8_t first = skipFirst ? 0 : pgm_read_byte(&TILESET[tileIndex * 2 * 8 + (spriteX % 8) * 2]);
          // // first = 0;
          //
          // bool skipSecond = false;//spriteY == 0;//(tileY == 0 && offset > 0);
          // tileRef = skipSecond ? 0 : pgm_read_byte(&sprite->data[SPRITE_HEADER_SIZE + (tileY) * spriteCols + tileX]);
          // tileIndex = skipSecond ? 0 : sprite->tileset * 64 + (tileRef & 0x3f);
          // uint8_t second = skipSecond ? 0 : pgm_read_byte(&TILESET[tileIndex * 2 * 8 + (spriteX % 8) * 2]);
          // second = 0;
          
          // bool skipFirst = false;//(tileY == 0 && offset > 0);
          // uint8_t tileRef = skipFirst ? 0 : pgm_read_byte(&sprite->data[SPRITE_HEADER_SIZE + (tileY - 1) * spriteCols + tileX]);
          // uint16_t tileIndex = skipFirst ? 0 : sprite->tileset * 64 + (tileRef & 0x3f);
          // uint8_t first = skipFirst ? 0 : pgm_read_byte(&TILESET[tileIndex * 2 * 8 + (spriteX % 8) * 2]);
          // // first = 0;
          
          
          // bool skipSecond = spriteY == 0;//(tileY == 0 && offset > 0);
          // tileRef = skipSecond ? 0 : pgm_read_byte(&sprite->data[SPRITE_HEADER_SIZE + (tileY) * spriteCols + tileX]);
          // tileIndex = skipSecond ? 0 : sprite->tileset * 64 + (tileRef & 0x3f);
          // uint8_t cur = skipSecond ? 0 : pgm_read_byte(&TILESET[tileIndex * 2 * 8 + (spriteX % 8) * 2]);
          // cur = 0;
          
          // if (offset == 0) {
          //   val = val | first;
          // } else {
          //   val = val | (first >> (8 - offset));
          // }
          
          // val = val | (first >> (8 - offset)) | (second << (offset));
  				// val = val | (first << (8 - offset)) | (cur >> offset);
        }
        ssd1306_send_byte(val);
      }
      ssd1306_send_data_stop();
    }
  }
  
  void clear () {
  }
};

#endif