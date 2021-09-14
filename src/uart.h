#include<stdint.h>

#define TX_PIN PB4
#define RX_PIN PB3

#define BAUD 115200
#define OCR  8000000/BAUD


void UARTinit(uint32_t baud);
uint8_t UARTread();
void UARTwrite(char c);
void UARTsendstr(char* str);

