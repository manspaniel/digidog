# Digi-Dog

Lil digital dog for your pocket, for when you can't have a real dog (or when your real dog doesn't fit in your pocket).

## Work in Progress!

This project isn't finished yet! There's still plenty to do, but the basics are all there.

## PCB Files

Eagle files are provided in the `pcb` folder. I print the circuit board layout onto transparent A4 overhead projector material, then use pre-sensitised PCB boards + developer + chemical etchant to produce the boards. Watch [this video](https://www.youtube.com/watch?v=tWnfnt2rNO0) for the process I follow. It's easy once you have the stuff :)

This is my first PCB design. The layout is a bit shit, but that's ok.

## Uploading

I'm using Platform IO to manage platforms/libraries, and to upload compiled code to the ATTiny85. To program the chip, I'm using [https://www.sparkfun.com/products/11801](SparkFun Tiny Programmer), which is just great; you can program on the chip, or use breakout wires to program onto your soldered prototype. (I've added a breakout header row to my PCB for connecting the microcontroller to the programmer via some breadboard jumper wires).

## Tools

Comes with a custom-made sprite drawing program. To use:

```bash
> cd tools/sprite-draw
> npm install
> npm start
```

Documentation for the sprite drawing tool is [here](./tools/sprite-draw/README.md).