Dog Editor
==========

A quick and hacky tile-based bitmap editor, specifically built for the purpose
of creating virtual pet dog sprites and related imagery.

## Running the tool

You'll need Node JS 8 or higher installed for this tool to work.

After you've cloned the project repository, cd into the dog editor folder, and start her up!

```bash
> cd tools/dog-editor
> npm install
> npm start
```

## Tile system

The system uses a tile system.

A **tileset** is a group of 64 tiles. Tilesets are all stored together, however they are paged in chunks of 64 to reduce the size of the sprites themselves, while expanding the amount of tiles that can be used by the program.

A **tiles** is an 8x8 monochrome bitmap. Each column (or row) can be represented in 1 byte (8 bits), since there are only two possible values for each pixel — black or white. This means each tile takes up 8 bytes in total.

A **sprite** (probably poorly named) has a variable width and height, and is composed entirely of tiles. A sprite must also reference a single **tileset**, which means that no sprite can use tiles from two different tilesets.

Tilesets are in groups of 64, because it means that each tile within a sprite can be referenced with only one byte. Two bits of each sprite tile each is reserved for flipX and flipY, and the remaining 6 bits can be used to reference a tile. The tileset number attached to the sprite just provides the pagination value.

## Loading and Saving

Hit the "Save" button to persist your tiles and sprites to disk.

Hit the "Reload" button to reload the persisted data.

Hit the "Publish Headers" button to generate and publish the `tiles.h` file to the parent project (ie. `../../src/tiles.h`).

## Persisting to a different file

All data is persisted in a JSON file, which by default is `data.json` within the dog-editor project folder. When booting the server app, you can actually specify another file using:

```bash
> npm start -- doggytest.json
```

## Tips and Functions

In the "Tiles" section:

* Drag with the mouse from one tile to another to copy sprite data!
* Do the same while holding SHIFT to *swap* those tiles. Sprites using either of those tiles will also be updated to point to the swapped tile, so your tiles wont break. This is really useful for organising your tileset.
* Clicking on a tile will highlight in green any sprite which uses that tile. This is good for determining if a tile is actually in use by any sprite.

In the sprite editor:

* Tools are represented by letters. You can press that letter on your keyboard to activate the tool!
* Some tools do something different when you right click.
  * The pencil tool (P) draws white by default, and erases to black with right mouse button.
  * The tile tool (T) draws the selected tile, but if you right click on a tile in your active sprite it acts as an eye dropper!