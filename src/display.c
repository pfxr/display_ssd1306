#include<stdint.h>
#include <avr/pgmspace.h>
#include"display.h"
#include"usi_i2c_master.h"
#include "image.h"

#define HRES 128
#define VRES   8

//variable declaration
//Commands
static uint8_t SetContrast[]      = {0x81, 0x3f};
static uint8_t SetCOMOutput[]     = {0xc8};     //c0 to invert screen vertically 
static uint8_t SetAddressingMode[]= {0x20,0x00};
static uint8_t SetSegmentRemap[]  = {0xa1};
static uint8_t EnableChargePump[] = {0x00,0x8d,0x14,0xaf};
static uint8_t displayAddress     =  0x00;


//Function delclaration
void displaySendCommand(uint8_t *command, uint8_t count){
  I2Cstart(displayAddress,0);
  I2Cwrite(0x00);
  for(uint8_t i=0; i<count; i++){
    I2Cwrite(*command);
    command++;
  }
  I2Cstop();
}

void displaySetXY(uint8_t x, uint8_t y){
  displaySetX(x);
  displaySetY(y);
}

void displaySetY(uint8_t y){
  uint8_t yValue = 0xb0 | y;
  displaySendCommand(&yValue,1);
}

void displaySetX(uint8_t x){
  uint8_t highX, lowX;
  lowX  = 0x00 | (x  & 0xf);
  highX = 0x10 |((x >>  4 ) & 0xf);
  displaySendCommand(&lowX ,1); // // lower col addr
  displaySendCommand(&highX,1); // upper col addr
}

void displayInit(uint8_t address){
  displayAddress = address;
  displaySendCommand(SetContrast      ,2);
  displaySendCommand(SetCOMOutput     ,1);
  displaySendCommand(SetAddressingMode,2);
  displaySendCommand(SetSegmentRemap  ,1);
  displaySendCommand(EnableChargePump ,4);
}


void displayWriteStr(char *str){
  uint8_t i = 0;
  I2Cstart(displayAddress,0);
  I2Cwrite(0x40);
  while(*str != '\0'){
    I2Cwrite(str[i]);
    str++;
  }
  I2Cstop();
}

void displayClear(){
  I2Cstart(displayAddress,0);
  I2Cwrite(0x40);
  for (uint8_t i=0;i<8;i++){
    for (uint8_t j=0; j<128; j++){
      I2Cwrite(0x00);
    }
  }
  I2Cstop();
}

void displayWrite(uint8_t value){
  I2Cstart(displayAddress,0);
  I2Cwrite(0x40);
  I2Cwrite(value);
  I2Cstop();
}

void displayShowImage(){
  I2Cstart(displayAddress,0);
  I2Cwrite(0x40);
  I2Cstart(displayAddress,0);
  I2Cwrite(0x40);
  for(uint8_t y=0; y<VRES; y++){
    for(uint8_t x=0; x<HRES; x++){
      I2Cwrite(pgm_read_byte(&mem[y][x]));
    }
  }
  I2Cstop();
}

