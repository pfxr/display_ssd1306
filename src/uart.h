#include<stdint.h>

#define TX_PIN PB4
#define RX_PIN PB3

void    UARTinit();
uint8_t UARTread();
void UARTwrite(char c);
void UARTsendstr(char* str);

