#include<avr/io.h>
#include<avr/interrupt.h>
#include<util/delay.h>
#include<stdint.h>
#include"uart.h"

//Variable declaration
volatile uint8_t  sending   =      0;
volatile uint8_t  receiving =      0;
volatile uint16_t tx_data   = 0x0000; 
volatile uint8_t  rx_data   =   0x00;
volatile uint8_t  rx_cnt    =      0;


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


// Interrupt handling
// Timer0
ISR (TIM0_COMPA_vect){

  if(tx_data){                     //means that there is something to send (stop_bit alone is enough)
     PORTB = (tx_data & 0x01) ? 
             PORTB | (1<<TX_PIN) : //set   TX_PIN 
             PORTB &~(1<<TX_PIN) ; //unset TX_PIN
    tx_data = tx_data >> 1;
  } else {
    if(!receiving){                //not receiving or sending
      disable_timer0();
      sending = 0;
    } else {                       //receiving
      if(rx_cnt == 8){
        if(PINB & RX_PIN){         //Stop bit
          receiving = 0;
          disable_timer0();
          enable_pcint();
          UARTwrite(rx_data);
        } 
        rx_cnt = 0;
      } else {                    //LSB 1st
        rx_data  = rx_data >> 1;
        rx_data |= (PINB & (1<<RX_PIN)) ? 0x80 : 0;
        rx_cnt++;
      }
    }
  } 

}

// PCINT interrupt
ISR(PCINT0_vect){
  //Start bit 
  if((PINB & (1<<RX_PIN))==0){
    receiving = 1;
    disable_pcint();
    enable_timer0();
  }
}

void UARTsendstr(char *str){
  while(*str != '\0'){
    UARTwrite(*str);
    while(sending);
    str++;
  }
  UARTwrite('\n');
}

void UARTwrite(char c){
  sending = 1;
  tx_data = (1<<9) | (c<<1); //stop_bit | c | start_bit
  enable_timer0();
}

uint8_t UARTread(uint8_t *data){

}

void UARTinit(){
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
  PCMSK  = (1<<RX_PIN);
  GIMSK  = (1<<PCIE);
}


