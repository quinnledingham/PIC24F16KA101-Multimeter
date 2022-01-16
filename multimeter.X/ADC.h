/*
 * 
 * File:   ADC.h
 * Authors: Maram Elsayed, Quinn Ledingham, Jack Li
 *
 */

#ifndef ADC_H
#define	ADC_H

#include <xc.h>
#include <P24F16KA101.h>

void ADCinit(uint16_t port);

void ADCinitCTMU();

uint16_t do_ADC(void);

uint32_t measure(void);

void DispADC(void);

void __attribute__((interrupt, no_auto_psv)) _ADC1Interrupt(void);

#endif	/* ADC_H */