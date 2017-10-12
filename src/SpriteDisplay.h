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

struct Bog {
  char x = 0;
};

class SpriteDisplay {
public:
  
  Sprite sprites[MAX_SPRITES];
  uint32_t dirtyCells[8];
  
  const uint8_t * tileset;
  
  SpriteDisplay() {
    // Sprite * sprite;
    // for (uint8_t k = 0; k < MAX_SPRITES; k++) {
    //   sprite = &sprites[k];
    //   sprite->display = this;
    // }
  }
  
  Sprite * addSprite (const uint8_t * spriteData) {
    Sprite * sprite;
    for (uint8_t k = 0; k < MAX_SPRITES; k++) {
      sprite = &sprites[k];
      if (!sprite->active) {
        sprite->hasRendered = false;
        sprite->needsUpdate = true;
        sprite->active = true;
        sprite->data = spriteData;
        sprite->x = 0;
        sprite->y = 0;
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
  
  uint8_t getTileByte(Sprite * sprite, uint8_t x, uint8_t y, uint8_t shiftX, bool alpha) {
    
    // Grab the type reference byte, which contains flipX/flipY/ID
    uint8_t tileRef = pgm_read_byte(&sprite->data[SPRITE_HEADER_SIZE + y * sprite->cols + x]);
    
    // We flip?
    bool flipX = (tileRef & 0x80) == 0x80;
  	bool flipY = (tileRef & 0x40) == 0x40;
    
    // Calculate the index on the global tileset
    uint16_t tileIndex = sprite->tileset * 64 + (tileRef & 0x3f);
    
    // Read and return the cell byte, compensaiting for x offset
    uint8_t value = pgm_read_byte(&TILESET[tileIndex * 2 * 8 + (flipX ? 7 - shiftX : shiftX) * 2 + (alpha ? 1 : 0)]);
    
    if (flipY) {
      value = (value & 0xF0) >> 4 | (value & 0x0F) << 4;
      value = (value & 0xCC) >> 2 | (value & 0x33) << 2;
      value = (value & 0xAA) >> 1 | (value & 0x55) << 1;
    }
    
    return value;
    
  }
  
  void setDirtyRect(int16_t x, int16_t y, int16_t w, int16_t h) {
    // if (x)
    uint8_t endRow = y / 8 + h / 8 + 1;
    uint8_t endCol = x / 4 + w / 4 + (x / 4 * 4 == x ? 0 : 1);
    for (uint8_t row = y / 8; row < endRow; row++) {
      for (uint32_t col = x / 4; col < endCol; col++) {
        dirtyCells[row] = dirtyCells[row] | ((uint32_t)1 << col);
      }
    }
  }
  
  void update () {
    // Find the regions on the screen which need updating
    for (uint8_t k = 0; k < MAX_SPRITES; k++) {
      Sprite * sprite = &sprites[k];
      if (sprite->needsUpdate) {
        // Re-render where the sprite currently is
        setDirtyRect(sprite->x, sprite->y, sprite->width, sprite->height);
        
        // Also re-render where the sprite was
        if (sprite->hasRendered) {
          setDirtyRect(sprite->lastX, sprite->lastY, sprite->lastWidth, sprite->lastHeight);
        }
        
        // Mark as rendered
        sprite->lastX = sprite->x;
        sprite->lastY = sprite->y;
        sprite->lastWidth = sprite->width;
        sprite->lastHeight = sprite->height;
        sprite->hasRendered = true;
        sprite->needsUpdate = false;
      }
    }
    
    for (int y = 0; y < 8; y++) {
      // dirtyCells[y] = ~0;
    }
    
    // Render the dirty cells
    // Since dirtyCells is structured row > col, we iterate over rows first, so that we can skip entire clean rows
    uint8_t x = 0;
    uint8_t y = 0;
    for (uint8_t cellRow = 0; cellRow < 8; cellRow++) {
      uint32_t dirtyMapRow = dirtyCells[cellRow];
      
      // Skip this row, if there's nothing
      if (dirtyMapRow == 0) continue;
      
      // Mark the row as clean
      dirtyCells[cellRow] = 0;
      
      bool rendering = false;
      
      // Start the cell bit (used for mask testing) at zero
      // It'll be set to 1 and shifted each time
      uint32_t cellBit = 0;
      
      // Render the row, cell by cell (a cell being 4 bytes)
      for (uint8_t cellCol = 0; cellCol < 32; cellCol++) {
        cellBit = cellBit == 0 ? 1 : (cellBit << 1);
        // Skip this cell, if there's nothing
        if ((dirtyMapRow & cellBit) == 0) {
          // Send the stop signal if we rendered the last cell
          if (rendering) {
            ssd1306_send_data_stop();
            rendering = false;
          }
          continue;
        }
        
        if (!rendering) {
          ssd1306_setpos(cellCol * 4, cellRow);
          ssd1306_send_data_start();
          rendering = true;
        }
        
        // Render 4 bytes for this cell
        // The display requires that each byte sent (1 byte = 8 x pixels)
        // is byte-aligned on the Y axis (0-7).
        // We group by 4 as a balancing act, so that the dirtyCells
        // grid isn't too big, but also the grid is small enough that
        // we're not over-rendering too much.
        y = cellRow * 8;
        for (uint8_t cellN = 0; cellN < 4; cellN++) {
          x = cellCol * 4 + cellN;
          
          uint8_t color = 0;
          uint8_t alpha = 0;
          
          // Loop over each sprite which overlaps this rect
          for (int spriteID = 0; spriteID < MAX_SPRITES; spriteID++) {
            Sprite * sprite = &sprites[spriteID];
            if (!sprite->active) continue;
            if (sprite->y + sprite->height < y || sprite->y >= y + 8) continue;
            if (sprite->x + sprite->width <= x || sprite->x > x) continue;
            
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
            
            uint8_t cellColor = 0;
            uint8_t cellAlpha = 0;
            
            if (showFirst) {
              uint8_t a = getTileByte(sprite, firstCol, firstRow, shiftX, true);
              cellAlpha = cellAlpha | (a << shiftY);
              uint8_t b = getTileByte(sprite, firstCol, firstRow, shiftX, false);
              cellColor = cellColor | (b << shiftY);
              // cellAlpha = getTileByte(sprite, firstCol, firstRow, shiftX, true);
              // val = val | (cell << shiftY);
            }
            if (showSecond) {
              uint8_t a = getTileByte(sprite, secondCol, secondRow, shiftX, true);
              cellAlpha = cellAlpha | (a >> (8 - shiftY));
              uint8_t b = getTileByte(sprite, secondCol, secondRow, shiftX, false);
              cellColor = cellColor | (b >> (8 - shiftY));
            }
            
            color = color | (~alpha & cellColor & cellAlpha);
            alpha = alpha | cellAlpha;
            
          }
          
          // Send the byte!
          ssd1306_send_byte(color);
        }
      }
      
      // If we rendered the last cell of this row, don't forget to send the stop signal
      if (rendering) {
        ssd1306_send_data_stop();
      }
    }
  }
  
  void removeAllSprites () {
    Sprite * sprite;
    for (uint8_t k = 0; k < MAX_SPRITES; k++) {
      sprite = &sprites[k];
      sprite->active = false;
      sprite->needsUpdate = false;
    }
    setDirtyRect(0, 0, 128, 64);
  }
  
  void wipe () {
    ssd1306_fillscreen(0x00);
  }
  
  void writeText(uint8_t x, uint8_t y, const char * str) {
  	uint8_t chr = 0;
    uint8_t byte = 0;
    uint8_t index = 0;
    uint8_t fontIndex = 0;
  	y = y / 8;
  	while (true) {
  		chr = pgm_read_byte_near(str + index);
      if (chr == '\0') break;
  		if (x > 126) {
  			x = 0;
  			y++;
  		}
      if (chr >= 65 && chr <= 91) {
        fontIndex = chr - 55;
      } else if (chr >= 97 && chr <= 122) {
        fontIndex = chr - 61;
      } else if (chr >= 48 && chr <= 57) {
        fontIndex = chr - 49;
      }
  		ssd1306_setpos(x, y);
  		ssd1306_send_data_start();
  		for (byte = 0; byte < 6; byte ++) {
  			ssd1306_send_byte(pgm_read_byte(&ssd1306xled_font6x8[fontIndex * 6 + byte]));
  		}
  		ssd1306_send_data_stop();
  		x += 6;
  		index++;
  	}
  }
};

#endif