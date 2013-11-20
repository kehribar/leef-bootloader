/*-----------------------------------------------------------------------------
/ Simple payload
/----------------------------------------------------------------------------*/
#include <stdio.h>
#include "digital.h"
#include <inttypes.h>
#include <util/delay.h>
#include <avr/interrupt.h>

/* Helpfull macros */
#define getByte() (UDR0)
#define newMessage() (UCSR0A & (1<<RXC0))

/* Bootloader start address */
void (*boot_funcptr)(void) = 0x7000;

/* Serial init function */
void init_serial()
{
    /* 38400 baud rate with 16 MHz F_OSC ... */
    const uint8_t ubrr = 25;

    pinMode(D,0,INPUT);
    pinMode(D,1,OUTPUT);

    /* Set baud rate */ 
    UBRR0H = (unsigned char)(ubrr>>8); 
    UBRR0L = (unsigned char)ubrr; 

    /* Enable receiver and transmitter */
    UCSR0B = (1<<RXEN0)|(1<<TXEN0);

    /* Set frame format: 8data, 1stop bit no parity */ 
    UCSR0C = (1<<UCSZ01)|(1<<UCSZ00);     
}

/* Main application */
int main(void) 
{         
    uint8_t ch;  
    init_serial();

    pinMode(B,5,OUTPUT);

    while(1)
    {       
        /* Fancy blink ... */
        togglePin(B,5);
        _delay_ms(125);
                
        togglePin(B,5);          
        _delay_ms(125);

        togglePin(B,5);
        _delay_ms(125);
                
        togglePin(B,5);         
        _delay_ms(1000); 

        /* Jump to the bootloader from userspace if needed */
        if(newMessage())
        {
            ch = getByte();
            
            /* Jump to the bootloader */
            if(ch == 'x')
            {
                boot_funcptr();
            }
        }
    }

    return 0;
}
