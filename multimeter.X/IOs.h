/*
 * 
 * File:   IOs.h
 * Authors: Maram Elsayed, Quinn Ledingham, Jack Li
 *
 */

#ifndef IOS_H
#define	IOS_H

#include <xc.h>
#include <P24F16KA101.h>

#include "UART2.h"
#include "ADC.h"
#include "TimeDelay.h"

void IOinit(); // Setups up the IO ports that are going to be used.

void IOcheck(); // Checks if the LED should be turned on.

void idleMode();

void doFrequency(); // gets frequency on ra6

// Called when there is a change of the input to the io pins
void __attribute__ ((interrupt, no_auto_psv)) _CNInterrupt (void);

// Called when there is a rising edge on ra6
void __attribute__((interrupt, no_auto_psv)) _IC1Interrupt(void);

#endif	/* IOS_H */