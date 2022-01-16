/*
 * 
 * File:   main.c
 * Authors: Maram Elsayed, Quinn Ledingham, Jack Li
 *
 */

#include <P24F16KA101.h>

#include "config.h"
#include "TimeDelay.h"
#include "UART2.h"
#include "IOs.h"
#include "clock.h"

// MACROS for Idle, Sleep modes
#define Nop() {__asm__ volatile ("nop");}
#define ClrWdt() {__asm__ volatile ("clrwdt");}
#define Sleep() {__asm__ volatile ("pwrsav #0");} // Sleep() - put MCU in sleep mode - CPU and some peripherals
#define Idle() {__asm__ volatile ("pwrsav #1");} // Idle() - put MCU in idle mode - only CPU off
#define dsen() {__asm__ volatile ("BSET DSCON, #15");}

int main(int argc, char** argv) {

    IOinit(); // initializes io
    
    Disp2String("\e[?25l"); // Hides the cursor
    
    while(1)
    {
        Disp2String("\e[1;1H\e[2J"); // Clears the console
        IOcheck(); // checks if any buttons are pressed down
    }
}

