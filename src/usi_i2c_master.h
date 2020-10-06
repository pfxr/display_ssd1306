#include <stdio.h>
#include <stdint.h>
#include <avr/io.h>
#include <util/delay.h>

// Defines
#define TWI_FAST_MODE

#ifdef TWI_FAST_MODE                 // TWI FAST mode timing limits. SCL = 100-400kHz
#define DELAY_T2TWI (_delay_us(2))   // >1.3us
#define DELAY_T4TWI (_delay_us(1))   // >0.6us
#else                                // TWI STANDARD mode timing limits. SCL <= 100kHz
#define DELAY_T2TWI (_delay_us(5))   // >4.7us
#define DELAY_T4TWI (_delay_us(4))   // >4.0us
#endif

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
