/*-----------------------------------------------------------------------------
/ Simple payload
/----------------------------------------------------------------------------*/
#include <stdio.h>
#include "digital.h"
#include <inttypes.h>
#include <avr/boot.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

int main(void) 
{           
    pinMode(B,5,OUTPUT);

    while(1)
    {        
        _delay_ms(125);
        togglePin(B,5);
        _delay_ms(125);
        togglePin(B,5);          
        _delay_ms(1000); 
    }

    return 0;
}
