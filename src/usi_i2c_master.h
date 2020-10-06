#include <stdio.h>
#include <stdint.h>
#include <avr/io.h>

// Defines
#define TWI_FAST_MODE



#define TWI_NACK_BIT 0 // Bit position for (N)ACK bit.
#define PIN_USI_SCL PB2
#define PIN_USI_SDA PB0

uint8_t I2CfindAddress();
void    I2Cinit(void);
uint8_t I2Cread(void);
uint8_t I2CreadLast(void);
uint8_t I2Cwrite(uint8_t data);
uint8_t I2Cstart(uint8_t address, int readWrite);
uint8_t I2Crestart(uint8_t address, int readWrite);
void    I2Cstop(void);
uint8_t I2Ctransfer(uint8_t data);
