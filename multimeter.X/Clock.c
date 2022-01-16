/* 
 * File:   clock.c
 * Authors: Maram Elsayed, Quinn Ledingham, Jack Li
 *
 */

// clkval= 8 8MHZ
// clkval= 500 500 kHz
// clkval= 32 32 kHz
#include "Clock.h"

void NewClk(unsigned int clkval) 
{
    char COSCNOSC;
    if (clkval == 8)    //8 MHZ
    {
        COSCNOSC = 0x00; 
    }
    else if (clkval == 500)    //500 kHZ
    {
        COSCNOSC = 0x66; 
    }
    else if (clkval == 32)    //32 kHZ
    {
        COSCNOSC = 0x55; 
    }
    else                      //default 32 kHZ
    {
        COSCNOSC = 0x55;
    }

    SRbits.IPL= 7;           //disable interrupts 
    CLKDIVbits.RCDIV = 0;     //turn clock division to 0
    __builtin_write_OSCCONH(COSCNOSC);  //write COSCNOSC to the high side of OSCON
    __builtin_write_OSCCONL(0x01);
    OSCCONbits.OSWEN= 1; 
    while (OSCCONbits.OSWEN == 1)
    {}
    SRbits.IPL= 0;           //enable interrupts
}