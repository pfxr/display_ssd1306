#include <avr/io.h>
#include <avr/pgmspace.h>
#include <stdint.h>
#include <stdbool.h>
#include <avr/interrupt.h>
#include "usi_i2c_master.h"
#include "uart.h"
#include "display.h"


uint8_t displayAddress = 0x00;

void setup(){
  UARTinit();
  I2Cinit();

  /*Interrupts*/
  //Enable glocal interrupts
  SREG |= (0x80);
}

int main(void){
  setup();
  displayAddress = I2CfindAddress();
  I2Cstop();
  displayInit(displayAddress);
  displayClear();
  displaySetXY(0,0);
  UARTsendstr("Hello World");
  while(1){
    displayShowImage();
  }
  return 0;
}
