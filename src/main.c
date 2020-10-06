#include <avr/io.h>
#include <avr/pgmspace.h>
#include <stdint.h>
#include <stdbool.h>
#include <avr/interrupt.h>
#include "usi_i2c_master.h"
#include "display.h"


uint8_t displayAddress = 0x00;



// Interrupt handling
ISR (TIMER0_OVF_vect){
}

void setup(){
  /*I/Os*/
  //PB4 as output
  DDRB |= (1<<PB4);

  /*Timer0*/
  //fck/256 prescaler
  TCCR0B |= (1<<CS02);
  //Enable interrupt
  TIMSK |= (1<<TOIE0);
  TIFR  |= (1<<TOV0);

  I2Cinit();

  /*Interrupts*/
  //Enable glocal interrupts
  SREG |= (0x80);
}

int main(void){
  setup();

  displayAddress = I2CfindAddress();

  if(displayAddress == 0x3c){
    PORTB |= (1<<PB4); 
    I2Cstop();
  }
  
  displayInit(displayAddress);
  displayClear();
  displaySetXY(0,0);
  

  while(1){
    displayShowImage();
  }
  return 0;
}
