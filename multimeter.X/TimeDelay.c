/*
 * 
 * File:   TimeDelay.c
 * Authors: Maram Elsayed, Quinn Ledingham, Jack Li
 *
 */

#include "TimeDelay.h"
#include "IOs.h"

int stopwatchTimer = 0;

void initTimer2() {
    //T2CON config
    T2CONbits.TSIDL = 0; //operate in idle mode
    T2CONbits.T32 = 0; // operate timer 2 as 16 bit timer
    T2CONbits.TCS = 0; // use internal clock
    // T2CONbits.TGATE = 0;

    //T2CON prescaler select. 11 = 1:256
    T2CONbits.TCKPS0 = 1;
    T2CONbits.TCKPS1 = 1;

    // Timer 2 interrupt config
    IPC1bits.T2IP = 3; // Set priority to 3. 7 is highest and 1 is lowest priority
    IEC0bits.T2IE = 1; // enable timer interrupt
}

void startTimer2(uint16_t time_ms) {
    IFS0bits.T2IF = 0; // Clear timer 2 flag

    uint16_t tmr_ticks = time_ms << 4; // Multiple the time by 16 to get the timer
    // ticks for the delay.  1s / (256 * 0.25us) = 15625 which is close to 16000.
    // 2s * 16000 = 2000ms * 16 = 32000 timer ticks.

    PR2 = tmr_ticks; //PR2 stores the target to trigger T2 interrupt
    TMR2 = 0; //zero TMR2 register to start
    T2CONbits.TON = 1; //start timer 2
    
    return;
}

uint16_t stopTimer2() {
    T2CONbits.TON = 0; //Stop timer
    uint16_t time = TMR2; // timer ticks counted
    TMR2 = 0; //zero TMR2 register on exit
    return time; // return timer ticks counted
}

void __attribute__((interrupt, no_auto_psv)) _T2Interrupt(void) {
    IFS0bits.T2IF = 0; //Clear timer 2 interrupt flag
    return;
}

void initTimer3() {
    //T2CON config
    T3CONbits.TSIDL = 0; //operate in idle mode
    T2CONbits.T32 = 0; // operate timer 2 as 16 bit timer
    T3CONbits.TCS = 0; // use internal clock
    // T2CONbits.TGATE = 0;

    //T2CON prescaler select. 11 = 1:256
    T3CONbits.TCKPS0 = 1;
    T3CONbits.TCKPS1 = 1;

    // Timer 2 interrupt config
    IPC1bits.T2IP = 3; // Set priority to 3. 7 is highest and 1 is lowest priority
    IEC0bits.T3IE = 1; // enable timer interrupt
}

void startTimer3(uint16_t time_ms) {
    IFS0bits.T3IF = 0; // Clear timer 3 flag

    uint16_t tmr_ticks = time_ms << 4; // Multiple the time by 16 to get the timer
    // ticks for the delay.  1s / (256 * 0.25us) = 15625 which is close to 16000.
    // 2s * 16000 = 2000ms * 16 = 32000 timer ticks.

    PR3 = tmr_ticks; //PR2 stores the target to trigger T2 interrupt
    TMR3 = 0; //zero TMR2 register to start
    T3CONbits.TON = 1; //start timer 2

    //Idle(); //Enter idle state
}

void stopTimer3() {
    // if not the stopwatchTimer
    if (stopwatchTimer == 0) {
        T3CONbits.TON = 0; //Stop timer
        TMR3 = 0; //zero TMR2 register on exit
    }
    return;
}


void __attribute__((interrupt, no_auto_psv)) _T3Interrupt(void) {
    IFS0bits.T3IF = 0; //Clear timer 2 interrupt flag
    return;
}

void Delay_ms(uint16_t time_ms) {
    startTimer2(time_ms); // start timer to interrupt in time_ms

    Idle(); //Enter idle state

    stopTimer2(); // stop timer
    return;
}

void StartStopwatch() {
    stopwatchTimer = 1; // stopwatch flag
    startTimer2(5000); // start timer2
    Idle(); // go into idle
}

uint16_t StopStopwatch() {
    uint16_t time = stopTimer2(); // stop timer2
    stopwatchTimer = 0; // clear stopwatch flag
    return time; // returns ticks counted
}