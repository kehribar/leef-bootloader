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
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include "./digital.h"

void (*funcptr)(void) = 0x0000;

/* SPM_PAGESIZE is 64 bytes for Attiny84 */
static uint8_t pageBuf[SPM_PAGESIZE];

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

static void boot_program_page(uint32_t page, uint8_t *buf)
{
    uint16_t i;

    cli();
    eeprom_busy_wait();

    for(i=0;i<SPM_PAGESIZE;i+=2)
    {
        uint16_t w = *buf++;
        w += (*buf++) << 8;

        boot_page_fill(page+i,w);
    }

    boot_page_write(page);
    boot_spm_busy_wait();  
}

int main(void) 
{    
    uint16_t i;
    uint8_t msg;
    uint32_t t32;
    uint8_t run = 1;
    uint32_t pageNum;
    uint32_t counter = 0;  

    wdt_reset();
    wdt_enable(WDTO_8S);  

    pinMode(A,0,INPUT);
    pinMode(A,1,OUTPUT);

    uint8_t x;

    uart_init();

    /* Wait until a message arrives or timeout */
    while((!newMessage())&&(run))
    {        
        if(counter++ > 0xFFFFF)
        {
            run = 0;
        }        
    }

    /* Was it correct message? */
    if(getByte() != 'a')
    {
        run = 0;
    }
    else
    {
        /* Send ACK */
        send_char('Y');
    }        

    while(run)
    {    
        /* kick the dog ... */
        wdt_reset();            
        
        while(!newMessage())
        {
            /* kick the dog ... */
            wdt_reset();
        }
        
        msg = getByte();
        
        switch(msg)
        {
            /* Ping */
            case 'a':
            {
                /* Send ACK */
                send_char('Y');
                break;
            }
            /* Fill the page buffer */
            case 'b':
            {
                /* Send ACK */
                send_char('Y');

                for(i=0;i<SPM_PAGESIZE;i++)
                {
                    while(!newMessage());
                    pageBuf[i] = getByte();
                }

                break;
            }
            /* Program the buffer */
            case 'c':
            {
                /* Send ACK */
                send_char('Y');

                while(!newMessage());
                pageNum = getByte();

                while(!newMessage());
                t32 = getByte();
                t32 = t32 << 8;
                pageNum += t32;

                while(!newMessage());
                t32 = getByte();
                t32 = t32 << 16;
                pageNum += t32;

                while(!newMessage());
                t32 = getByte();
                t32 = t32 << 24;
                pageNum += t32;

                boot_program_page(pageNum,pageBuf);

                /* Send ACK */
                send_char('Y');
                break;
            }
            /* Delete the pages */
            case 'd':
            {   
                for(t32=0;t32<BOOTSTART;t32+=SPM_PAGESIZE)
                {
                    boot_page_erase(t32);
                    boot_spm_busy_wait();               
                }                    

                /* Send ACK */
                send_char('Y');
                break;
            }
            /* Go to user app ... */
            case 'x':
            {                   
                MCUSR &= ~(1 << WDRF);
                wdt_disable();                 
                
                funcptr();                 
                break;
            }
        }     
    }
    
    /* Go to user app ... */
    MCUSR &= ~(1 << WDRF);
    wdt_disable();                 
    funcptr();

    return 0;
}

