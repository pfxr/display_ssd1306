#include<avr/io.h>
#include<avr/interrupt.h>
#include<util/delay.h>
#include<stdint.h>
#include"uart.h"

//Variable declaration
volatile uint8_t send_flag = 0;


static void enable_timer0(){
  TCNT0   =        0; //Reset timer counter 0 
  TCCR0B |= (1<<CS01); //Enable timer0
}

static void disable_timer0(){
  TCCR0B &= ~(1<<CS01);
  TCNT0   =        0; //Reset timer counter 0 
}

static void enable_pcint(){
  GIMSK = (1<<PCIE);
} 

static void disable_pcint(){
  GIMSK &= ~(1<<PCIE);
} 

volatile uint32_t cnt_led = 0;
// Interrupt handling
// Timer0
ISR (TIM0_COMPA_vect){
  send_flag = 1;
  cnt_led++;
  if(cnt_led == 9600){
    cnt_led=0;
  }
}

// PCINT interrupt
ISR(PCINT0_vect){
  //Start bit 
  if((PINB & (1<<RX_PIN))==0){
    disable_pcint();
  }
}

void UARTsendstr(char *str){
  while(*str != '\0'){
    UARTwrite(*str);
    str++;
  }
}

void UARTwrite(char c){
  send_flag = 0;
  PORTB &= ~(1<<TX_PIN); //start bit
  enable_timer0();
  while(!send_flag);
  for(uint8_t i=0; i<8; i++){
    if(c & 0x01){
      PORTB |= (1<<TX_PIN);
    } else {
      PORTB &= ~(1<<TX_PIN);
    }
    c = c>>1;
    send_flag = 0;
    while(!send_flag);
  }
  PORTB |= (1<<TX_PIN);
  send_flag = 0;
  while(!send_flag);
  disable_timer0();
}

uint8_t UARTread(uint8_t *data){

}

void UARTinit(){
  DDRB |= (1<<PB1);

  //Timer0 initialization
  //fck/8 prescaler = 1MHz
  TCCR0B |= (1<<CS01); //Enable timer0
  
  //Enable interrupt(CTC)
  TCCR0A = (1<<WGM01);
  TIMSK |= (1<<OCIE0A);
  
  // Tclk = 1us
  // Baud rate 9600bps
  // T9600 = 104.16us
  OCR0A = 103;

  /*IO configuration*/
  //RX = PCINT3 interrupt enable
  //TX = PB4 output
  DDRB  |= (1<<TX_PIN);
  PORTB |= (1<<TX_PIN);
  PCMSK = (1<<RX_PIN);
  GIMSK = (1<<PCIE);
}


