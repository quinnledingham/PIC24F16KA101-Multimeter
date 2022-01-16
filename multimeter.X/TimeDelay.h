/*
 * 
 * File:   TimeDelay.h
 * Authors: Maram Elsayed, Quinn Ledingham, Jack Li
 *
 */

#ifndef TIMEDELAY_H
#define	TIMEDELAY_H

#include <xc.h>
#include <P24F16KA101.h>

void initTimer2(); // clears and initializes timer2

void startTimer2(uint16_t time_ms); // starts timer2 to interrupt every time_ms

uint16_t stopTimer2(); // stops timer2 and returns timer ticks

void __attribute__((interrupt, no_auto_psv)) _T2Interrupt(void); // Timer 2 interrupt
                                                                 // service routine

void initTimer3(); // clears and initializes timer3

void startTimer3(uint16_t time_ms); // start timer3 to interrupt every time_ms

void stopTimer3(); // stops timer3

void __attribute__((interrupt, no_auto_psv)) _T3Interrupt(void); // Timer 3 interrupt
                                                                 // service routine

void Delay_ms(uint16_t time_ms); // Delays for time_ms

void StartStopwatch(); // starts stopwatch. counts timer ticks.

uint16_t StopStopwatch(); // stops stopwatch and returns timer ticks

#endif	/* TIMEDELAY_H */