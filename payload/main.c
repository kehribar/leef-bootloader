/*-----------------------------------------------------------------------------
/
/
/
/
/
/
/
/
/----------------------------------------------------------------------------*/
#include <stdio.h>
#include <inttypes.h>
#include <avr/boot.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include "./digital.h"

#define CPU_CLOCK_FREQ   8000000UL 
#define SAMPLES_PER_BIT  8 
#define BAUD_RATE        115200 

/* http://www.avrfreaks.net/index.php?name=PNphpBB2&file=viewtopic&p=921386&sid=da7c0ec9973a3245d52f5261b46b3f9c#921386 */
void uart_init()
{
    /* Set samples per bit and UART baud */ 
    LINBTR = (1 << LDISR) | SAMPLES_PER_BIT; 
    LINBRR = ((CPU_CLOCK_FREQ / SAMPLES_PER_BIT) / BAUD_RATE) - 1; 

    /* Configure LIN UART in UART mode */ 
    LINCR  = (1 << LENA) | (1 << LCMD0) | (1 << LCMD1) | (1 << LCMD2); 
}

void send_char(uint8_t ch)
{
    while ((LINSIR & (1 << LBUSY))); 
    
    LINDAT = ch;

    while ((LINSIR & (1 << LTXOK))); 
}

#define newMessage() (LINSIR & (1<<LRXOK))
#define getByte() (LINDAT)

void (*funcptr)(void) = 0x3C00;

int main(void) 
{
    uint8_t run = 1;
    uint32_t counter = 0;

    pinMode(A,0,INPUT);
    pinMode(A,1,OUTPUT);

    uart_init();

    /* Check for a uart command */
    while((!newMessage())&&(run))
    {        
        if(counter++ > 0xFFFFF)
        {
            run = 0;
        }        
    }

    /* Was it correct message? */
    if(getByte() == 'a')
    {
        /* Send ACK */
        send_char('Y');
    
        /* Go to bootloader */
        funcptr();
    } 
    
    while(1)
    {
        send_char('h');
        send_char('i');
        send_char('\r');
        send_char('\n');
        _delay_ms(150);
    }

}