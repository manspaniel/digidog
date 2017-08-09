#include <Arduino.h>
#define F_CPU 8000000UL      //ATtiny85 CPU Clock speed (8MHz optimal[8000000], 1MHz Default[1000000])

#define SSD1306_SCL   PB0 // SCL, Pin 3 on SSD1306 Board
#define SSD1306_SDA   PB1 // SDA, Pin 4 on SSD1306 Board
 
#define SSD1306_SA    0x78  // Slave address

#include <stdlib.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <avr/sleep.h>

#include "ssd1306xled.h"
#include "VDoggy.h"
// #include "out.h"

VDoggy doggy;

volatile int watchdog_counter;

//This runs each time the watch dog wakes us up from sleep
ISR(WDT_vect) {
  watchdog_counter++;
}

// 0=16ms, 1=32ms, 2=64ms, 3=128ms, 4=250ms, 5=500ms
// 6=1sec, 7=2sec, 8=4sec, 9=8sec
// From: http://interface.khm.de/index.php/lab/experiments/sleep_watchdog_battery/
void setup_watchdog(int timerPrescaler) {

  if (timerPrescaler > 9 ) timerPrescaler = 9; //Correct incoming amount if need be

  byte bb = timerPrescaler & 7;
  if (timerPrescaler > 7) bb |= (1<<5); //Set the special 5th bit if necessary

  //This order of commands is important and cannot be combined
  MCUSR &= ~(1<<WDRF); //Clear the watch dog reset
  WDTCR |= (1<<WDCE) | (1<<WDE); //Set WD_change enable, set WD enable
  WDTCR = bb; //Set new watchdog timeout value
  WDTCR |= _BV(WDIE); //Set the interrupt enable, this will keep unit from resetting after each int
}

void setup(){
  // Small delay is necessary if ssd1306_initis the first operation in the application.
  // _delay_ms(40);
  ssd1306_init();
 
  ssd1306_fillscreen(0x00);
  ssd1306_char_f8x16(1, 2, "VDOG");
  ssd1306_char_f6x8(1, 5, "cutie");
  _delay_ms(1000);
 
  watchdog_counter = 0;
  // set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  // setup_watchdog(6);
 
  // uint8_t p = 0xff;
 
  // for (uint8_t i = 0; i < 4; i++) {
  //   p = (p >> i);
  //   ssd1306_fillscreen(~p);
  //   _delay_ms(200);
  // }
  
  pinMode(A2, INPUT);
  
}
 
void loop() {
  
  // sleep_mode();
  
  doggy.loop();
  
  // delay(100);
  
  // watchdog_counter = analogRead(A2);
  //
  // char buffer[10];         //the ASCII of the integer will be stored in this char array
  // itoa(watchdog_counter, buffer, 10);
  //
  // ssd1306_char_f8x16(1, 2, "zzzzz");
  // ssd1306_char_f8x16(1, 2, buffer);
  //
  // itoa(millis(), buffer, 10);
  //
  // ssd1306_char_f8x16(20, 3, buffer);
  //
  // delay(100);
  
  // ssd1306_fillscreen(0x00);
  // ssd1306_char_f6x8(1, 0, "DANIEL LEVER"); //Cannot start at 0 for x. Gives problem
 
  // ssd1306_char_f6x8(1, 0, "I'm a small and mighty.");
  // _delay_ms(5000);
  
  // ssd1306_setpos(60, 60);
  // ssd1306_send_data_start();
  
  for (int i = 0; i < 10; i++) {
    // int r = random() * 255;
    // ssd1306_send_byte(214);
  }
  
  // ssd1306_send_data_stop();
  
  // _delay_ms(1000);
  
  // ssd1306_draw_bmp(0, 0, 128, 8, tiny);
  
  // ssd1306_init();
  
  // delay(5000);
  
  // delay(100);
  
  // _delay_ms(500);
  
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