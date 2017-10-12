#include <Arduino.h>
// #define F_CPU 8000000UL      //ATtiny85 CPU Clock speed (8MHz optimal[8000000], 1MHz Default[1000000])

#define SSD1306_SA    0x78  // Slave address

// #include <stdlib.h>
// #include <avr/io.h>
// #include <util/delay.h>
// #include <avr/pgmspace.h>
// #include <avr/sleep.h>

#include "ssd1306xled.h"
#include "DigiDog.h"
#include "SpriteDisplay.h"

// void * operator new(size_t size) {
//   return malloc(size);
// }
//
// void operator delete(void * ptr) {
//   free(ptr);
// }

// #define CALIBRATON_MODE true

#ifndef CALIBRATON_MODE
  SpriteDisplay mainDisplay;
  DigiDog doggy;
#endif

void setup() {
  // Small delay is necessary if ssd1306_initis the first operation in the application.
  // _delay_ms(40);
  ssd1306_init();
 
  ssd1306_fillscreen(0x0);
  // ssd1306_char_f8x16(1, 2, "VDOG");
  delay(100);
 
  // set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  // setup_watchdog(6);
 
  // uint8_t p = 0xff;
 
  // for (uint8_t i = 0; i < 4; i++) {
  //   p = (p >> i);
  //   ssd1306_fillscreen(~p);
  //   _delay_ms(200);
  // }
  
  // pinMode(A2, INPUT);
  
  #ifndef CALIBRATON_MODE
    doggy.start();
  #endif
  
}

void loop() {
  
  // Figure out the current button
  
  #ifdef CALIBRATON_MODE
    int signal = analogRead(A2);
    
    ssd1306_fill_range(0, 60, 0x00);
    char buffer[10];         //the ASCII of the integer will be stored in this char array
    itoa(signal, buffer, 10);
    ssd1306_char_f6x8(0, 0, buffer);
  #endif
  
  #ifndef CALIBRATON_MODE
    doggy.loop();
  #endif
  
  // delay(100);
  
  // watchdog_counter = analogRead(A2);
  //
  // ssd1306_fill_range(0, 60, 0x00);
  // char buffer[10];         //the ASCII of the integer will be stored in this char array
  // itoa(val, buffer, 10);
  // ssd1306_char_f6x8(0, 0, buffer);
  
  // setup_watchdog(8);
  // sleep_mode();
  
  
  
  // ssd1306_send_byte(252);
 
  // ssd1306_char_f6x8(1, 0, "Who says I can't run this LED screen?");
  // _delay_ms(5000);
  //
  //
  // ssd1306_char_f6x8(1, 0, "There am I running it now.");
  // _delay_ms(5000);
 
}