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
#include "./elmChan_serial/suart.h"
#include "./elmChan_serial/xitoa.h"

#define getByte() rcvr()
#define send_char(x) xmit(x)

void (*funcptr)(void) = 0x1C00;

int main(void) 
{    
    pinMode(A,1,INPUT);
    pinMode(A,0,OUTPUT);
    digitalWrite(A,0,HIGH);

    /* Was it correct message? */
    if(getByte() != 'a')
    {
        
    }
    else
    {
        /* Send ACK */
        send_char('Y');
        funcptr();
    } 
    
    while(1)
    {
        send_char('h');
        send_char('i');
        send_char('\r');
        send_char('\n');
        _delay_ms(50);
    }

}