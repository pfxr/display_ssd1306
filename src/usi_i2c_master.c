#include <stdio.h>
#include <stdint.h>
#include <avr/io.h>
#include "usi_i2c_master.h"

// Constants
// Prepare register value to: Clear flags, and set USI to shift 8 bits i.e. count 16 clock edges.
static unsigned char USISR_8bit = 1<<USISIF | 1<<USIOIF | 1<<USIPF | 1<<USIDC | 0x0<<USICNT0;
// Prepare register value to: Clear flags, and set USI to shift 1 bit i.e. count 2 clock edges.
static unsigned char USISR_1bit = 1<<USISIF | 1<<USIOIF | 1<<USIPF | 1<<USIDC | 0xE<<USICNT0;


static int I2Ccount;

// Minimal Tiny I2C Routines **********************************************
uint8_t I2CfindAddress(){
  for(uint8_t i=0; i<128; i++){
    if(!(I2Cstart(i,0)))
      I2Cstop();
    else
      return i;
  }
  return 255; //impossible address
}


uint8_t I2Ctransfer (uint8_t data) {
  USISR = data;                               // Set USISR according to data.
                                              // Prepare clocking.
  data = 0<<USISIE | 0<<USIOIE |              // Interrupts disabled
         1<<USIWM1 | 0<<USIWM0 |              // Set USI in Two-wire mode.
         1<<USICS1 | 0<<USICS0 | 1<<USICLK |  // Software clock strobe as source.
         1<<USITC;                            // Toggle Clock Port.
  do {
    USICR = data;                       // Generate positive SCL edge.
    while (!(PINB & 1<<PIN_USI_SCL));   // Wait for SCL to go high.
    USICR = data;                       // Generate negative SCL edge.
  } while (!(USISR & 1<<USIOIF));       // Check for transfer complete.

  data = USIDR;                         // Read out data.
  USIDR = 0xFF;                         // Release SDA.
  DDRB |= (1<<PIN_USI_SDA);             // Enable SDA as output.

  return data;                          // Return the data from the USIDR
}

void I2Cinit () {
  PORTB |= 1<<PIN_USI_SDA;  // Enable pullup on SDA.
  PORTB |= 1<<PIN_USI_SCL;  // Enable pullup on SCL.

  DDRB |= 1<<PIN_USI_SCL;   // Enable SCL as output.
  DDRB |= 1<<PIN_USI_SDA;   // Enable SDA as output.

  USIDR = 0xFF;                               // Preload data register with "released level" data.
  USICR = 0<<USISIE | 0<<USIOIE |             // Disable Interrupts.
          1<<USIWM1 | 0<<USIWM0 |             // Set USI in Two-wire mode.
          1<<USICS1 | 0<<USICS0 | 1<<USICLK | // Software stobe as counter clock source
          0<<USITC;
  USISR = 1<<USISIF | 1<<USIOIF | 1<<USIPF | 1<<USIDC | // Clear flags,
          0x0<<USICNT0;                                 // and reset counter.
}

uint8_t I2Cread (void) {
  if ((I2Ccount != 0) && (I2Ccount != -1)) I2Ccount--;

  /* Read a byte */
  DDRB &= ~(1<<PIN_USI_SDA);               // Enable SDA as input.
  uint8_t data = I2Ctransfer(USISR_8bit);

  /* Prepare to generate ACK (or NACK in case of End Of Transmission) */
  if (I2Ccount == 0) USIDR = 0xFF; else USIDR = 0x00;
  I2Ctransfer(USISR_1bit);                 // Generate ACK/NACK.

  return data;                                // Read successfully completed
}

uint8_t I2CreadLast (void) {
  I2Ccount = 0;
  return I2Cread();
}

uint8_t I2Cwrite (uint8_t data) {
  /* Write a byte */
  PORTB &= ~(1<<PIN_USI_SCL);           // Pull SCL LOW.
  USIDR = data;                               // Setup data.
  I2Ctransfer(USISR_8bit);                 // Send 8 bits on bus.

  /* Clock and verify (N)ACK from slave */
  DDRB &= ~(1<<PIN_USI_SDA);               // Enable SDA as input.
  if (I2Ctransfer(USISR_1bit) & 1<<TWI_NACK_BIT) return 0;

  return 1;                                // Write successfully completed
}

// Start transmission by sending address
uint8_t I2Cstart (uint8_t address, int readWrite) {
  uint8_t addressRW = address<<1 | readWrite;

  /* Release SCL to ensure that (repeated) Start can be performed */
  PORTB |= 1<<PIN_USI_SCL;              // Release SCL.
  while (!(PINB & 1<<PIN_USI_SCL));     // Verify that SCL becomes high.

  /* Generate Start Condition */
  PORTB &= ~(1<<PIN_USI_SDA);      // Force SDA LOW.
  PORTB &= ~(1<<PIN_USI_SCL);      // Pull SCL LOW.
  PORTB |= 1<<PIN_USI_SDA;         // Release SDA.

  if (!(USISR & 1<<USISIF)) return 0;

  /*Write address */
  PORTB &= ~(1<<PIN_USI_SCL);      // Pull SCL LOW.
  USIDR = addressRW;               // Setup data.
  I2Ctransfer(USISR_8bit);         // Send 8 bits on bus.

  /* Clock and verify (N)ACK from slave */
  DDRB &= ~(1<<PIN_USI_SDA);       // Enable SDA as input.
  if (I2Ctransfer(USISR_1bit) & 1<<TWI_NACK_BIT) return 0; // No ACK

  return 1;  // Start successfully completed
}

uint8_t I2Crestart(uint8_t address, int readWrite) {
  return I2Cstart(address, readWrite);
}

void I2Cstop (void) {
  PORTB &= ~(1<<PIN_USI_SDA);              // Pull SDA low.
  PORTB |= 1<<PIN_USI_SCL;              // Release SCL.
  while (!(PINB & 1<<PIN_USI_SCL));     // Wait for SCL to go high.
  PORTB |= 1<<PIN_USI_SDA;                 // Release SDA.
}
