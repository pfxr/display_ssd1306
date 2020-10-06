#include<stdint.h>


void displaySendCommand(uint8_t *command, uint8_t count);
void displayShowImage();
void displaySetY(uint8_t y);
void displaySetX(uint8_t x);
void displaySetXY(uint8_t x, uint8_t y);
void displayInit(uint8_t address);
void displayWriteStr(char *str);
void displayClear();
void displayWrite(uint8_t value);
