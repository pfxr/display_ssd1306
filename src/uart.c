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
ISR (TIM0_COMPA_vect) {
  if(!receiving && !sending){ 
    disable_timer0();
  } else {

    // TX process
    if(sending) {
      if(tx_data) { //means that there is something to send (stop_bit alone is enough)
        PORTB = (tx_data & 0x01) ? 
               PORTB | (1<<TX_PIN) : //set   TX_PIN 
               PORTB &~(1<<TX_PIN) ; //unset TX_PIN
        tx_data = tx_data >> 1;
      } else { // Nothing else to send
        sending = 0;
      }
    }

    // RX Process
    if(receiving) {                      
      if(rx_cnt == 8) {
        // Check for stop bit
        if(PINB & RX_PIN) { 
          receiving = 0;
          enable_pcint();
          rx_cnt = 0;
        } 
        // Receive Data bits (LSB first)
      } else {    
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
    str++;
  }
  UARTwrite('\n');
}

void UARTwrite(char c){
  sending = 1;
  tx_data = (1<<9) | (c<<1); //stop_bit | c | start_bit
  enable_timer0();
  while(sending);
}

uint8_t UARTread(uint8_t *data){
  if (receiving) {
    return 0;
  } else {
    if (rx_data == 0x00) {
      return 0;
    } else {
      *data = rx_data;
      rx_data = 0x00;
      return 1;
    }
  }
}

void UARTinit(uint32_t baud){
  //Timer0 initialization
  //fck/8 prescaler = 1MHz
  TCCR0B |= (1<<CS01); //Enable timer0
  
  //Enable interrupt(CTC)
  TCCR0A = (1<<WGM01);
  TIMSK |= (1<<OCIE0A);
  
  // Tclk = 125ns
  // Baud rate bps
  // T115200 = 8.6us
  OCR0A = 104; 

  /*IO configuration*/
  //RX = PCINT3 interrupt enable
  //TX = PB4 output
  DDRB  |= (1<<TX_PIN);
  PORTB |= (1<<TX_PIN);
  PCMSK  = (1<<RX_PIN);
  GIMSK  = (1<<PCIE);
}


