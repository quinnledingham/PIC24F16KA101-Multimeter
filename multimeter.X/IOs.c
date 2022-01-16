/*
 * 
 * File:   IOs.c
 * Authors: Maram Elsayed, Quinn Ledingham, Jack Li
 *
 */

// included for random num
#include <time.h>
#include <stdlib.h>
#include <math.h>

#include "Comparator.h"
#include "IOs.h"
#include "Clock.h"

int CN30flag = 0; // flag to tell if RA2 changed
int CN0flag = 0; // flag to tell if RA4 changed
int CN1flag = 0; // flag to tell if RB4 changed

int ioChange = 0; // flag to tell if io has changed
int ra2Pressed = 0;
int rb4Pressed = 0;
int ra4Pressed = 0; // flag to tell if ra4 did a complete press

int skipIdle = 0; // flag to skip idle

uint16_t risingEdges = 0;

void IOinit() {
    TRISAbits.TRISA2 = 1; // RA2 set as input (PB1)
    TRISAbits.TRISA4 = 1; // RA4 set as input (PB2)
    TRISBbits.TRISB4 = 1; // RB4 set as input (PB3)
    TRISBbits.TRISB8 = 0; // RB8 set as output

    CNPU2bits.CN30PUE = 1; // Pull CN30 - RA2 
    CNPU1bits.CN1PUE = 1; // Pull CN1 - RB4  
    CNPU1bits.CN0PUE = 1; // Pull CN0 - RA4  

    //TRISBbits.TRISB8 = 0; // RB8 set as output

    //Initialize CN Interrupt
    CNEN2bits.CN30IE = 1; //Enable CN30 interrupt
    CNEN1bits.CN1IE = 1; //Enable CN1 interrupt
    CNEN1bits.CN0IE = 1; //Enable CN0 interrupt

    IPC4bits.CNIP = 5; //set CN interrupt priority
    IFS1bits.CNIF = 0; //clears the CN interrupt flag
    IEC1bits.CNIE = 1; //enable the CN interrupt (general)

    TRISBbits.TRISB15 = 0; // RB15 set as output

    AD1PCFG = 0xFFFF; // make all pins digital

    srand(time(NULL)); // Initialize random num generator

    return;
}

float VDD = 3.5; // voltage

uint16_t ticks; // timer ticks for frequency

void IOcheck() {
    // RA2 (PB1)
    if (CN30flag == 1) {
        IEC1bits.CNIE = 0; // Disable CN interrupts to avoid debounce issues
        ADCinit(3); // initialize adc on pin ra3
        Disp2String("VOLTMETER Voltage = "); // display voltage
        
        float v; // voltage
        float m; // convert adc reading to voltage
        m = 1023 / VDD; // number to convert to voltage
        v = measure(); // get adc
        v = v / m; // get voltage

        uint16_t num = v; // num without decimals for voltage
        uint16_t deci = (v - num) * 100; // decimals for voltage

        Disp2Dec(num, 5); // display num voltage
        Disp2String("."); // decimal point
        Disp2Dec(deci, 2); // decimals
        Disp2String("V"); // units

        IEC1bits.CNIE = 1; // Enable CN interrupts

        doFrequency(); // get frequency value
    }
    // RA4 (PB2)
    if (CN0flag == 1) {
        IEC1bits.CNIE = 0; // Disable CN interrupts to avoid debounce issues
        ADCinit(13); // initialize adc on pin rb13
        Disp2String("OHMMETER Resistance = "); // display resistance

        float v; // voltage
        float m; // conversion
        m = VDD / 1023; // conversion number
        v = measure(); // get adc measurement
        v = v * m; // convert to voltage

        //float I = (VDD - v) / 1000;
        float R = (v * 1000) / (VDD - v); // get resistance.
                                          // 1000k resistor connected to VDD

        uint16_t num = R; // num
        uint16_t deci = (v - num) * 10; // decimals
        Disp2Dec(num, 5); // display nums
        Disp2String("."); // decimal point
        Disp2Dec(deci, 2); // display decimals
        Disp2String("ohms"); // units
        
        doFrequency(); // get frequency value
        IEC1bits.CNIE = 1; // Enable CN interrupts
    }
    // RB4 (PB3)
    if (CN1flag == 1) {
        IEC1bits.CNIE = 0; // Disable CN interrupts to avoid debounce issues

        LATBbits.LATB8 = 1; // turn light on

        IEC1bits.CNIE = 1; // Enable CN interrupts
        rb4Pressed = 1; // raPressed down. does capacitance once button is lifted
    } else if (rb4Pressed == 1) {
        IEC1bits.CNIE = 0; // Disable CN interrupts to avoid debounce issues
        ADCinit(12); // initalize adc on pin rb12
        float v; // voltage
        float m; // conversion
        uint16_t time; // time for charge to 3V

        m = 1023 / VDD; // init conversion
        initTimer2(); // initialize timer 2

        LATBbits.LATB14 = 0; // set output to 0 on b14 
        TRISBbits.TRISB14 = 0; // RB14 set as output

        StartStopwatch(); // starts timer2
        LATBbits.LATB14 = 1; // output 1
        // charge capacitor until volts > 3
        while (1) {
            v = do_ADC(); // do adc measurement
            v = v / m; // get voltage

            // volts > 3
            if (v > 3) {
                time = StopStopwatch(); // stop timer2 and gets time
                break; // leave loop
            }
        }

        LATBbits.LATB14 = 0; // output 0
        rb4Pressed = 0; // button not pressed down
        float s; // seconds
        s = (float) time / 16000;
        float tao; // tao. used to get to farads
        tao = s / (log(7)); // from capacitor formula
        float microfarads;
        microfarads = tao * 10; // gets mircofarads

        Disp2String("Capacitance = "); // display capacitance
        uint16_t num = microfarads; // num
        uint16_t deci = (microfarads - num) * 100; // decimals

        Disp2Dec(num, 5); // display num
        Disp2String("."); // display decimal point
        Disp2Dec(deci, 2); // display decimals
        Disp2String("uF"); // display units

        //Disp2Dec(microfarads, 5);
        IEC1bits.CNIE = 1; // Enable CN interrupts
        LATBbits.LATB8 = 0; // turn light off
        
        doFrequency(); // get frequency
        idleMode();
    }


    // No buttons pressed
    if (CN0flag == 0 && CN1flag == 0 && CN30flag == 0 && skipIdle == 0) {
        LATBbits.LATB8 = 0; // turn off light before going into idle
        idleMode();
    } else {
        skipIdle = 0; // don't skip next idle
    }
}

void idleMode() {
    NewClk(32); // set clock to 32kHz
    Idle();
    NewClk(8); // set clock to 8MHz
}

void doFrequency() {
    risingEdges = 0; // reset risingEdges flag

    NewClk(32); // set the clock to 32kHz

    REFOCONbits.RODIV = 0b1110; // Set what the base clock is divided by
    REFOCONbits.ROSEL = 0; // Use the system clock as the base clock

    //OC1RS = 20;
    TRISBbits.TRISB14 = 1; // RA6 set as output
    IC1CONbits.ICSIDL = 0; // continue in idle
    IC1CONbits.ICTMR = 0; // TMR3 contents captured on 
    IC1CONbits.ICI = 0b00; // Interrupt on every capture event
    IC1CONbits.ICM = 0b011; // capture mode, every rising edge
    IFS0bits.IC1IF = 0; //Clear IC1 interrupt flag

    initTimer2(); // initialize timer2
    IEC0bits.IC1IE = 1; // Turn IC1 interrupts on
    REFOCONbits.ROEN = 1; // Turn the reference oscillator on
    // wait for two rising edges to be seen. risingEdges = 1 when two have been counted.
    while (risingEdges == 0) {
    }
    REFOCONbits.ROEN = 0; // Turn the reference oscillator off
    IEC0bits.IC1IE = 0; // Turn IC1 interrupts off
    NewClk(8); // set the clock to 8MHz

    float period; // period for the input to ra6
    period = (float)ticks / 62.5;
    
    float frequency; // frequency for input to ra6
    frequency = 1 / period;
    
    Disp2String(" Frequency = "); // Print frequency = 
    uint16_t num = frequency; // get num with decimals
    uint16_t deci = (frequency - num) * 100; // get decimals

    Disp2Dec(num, 5); // Print frequency without decimals
    Disp2String("."); // Print decimal
    Disp2Dec(deci, 2); // Print decimals of frequency
    Disp2String("Hz"); // Print units
}

void __attribute__((interrupt, no_auto_psv)) _CNInterrupt(void) {
    // if interrupt status/flag bit is 1
    if (IFS1bits.CNIF == 1) {
        // If RA2 = 0 set flag = 1 else if RA2 = 1 set flag = 0
        if (PORTAbits.RA2 == 0) {
            CN30flag = 1; // user defined global variable used as flag
        } else if (PORTAbits.RA2 == 1) {
            CN30flag = 0; // user defined global variable used as flag
        }
        // If RB4 = 0 set flag = 1 else if RB4 = 1 set flag = 0
        if (PORTBbits.RB4 == 0) {
            CN1flag = 1; // user defined global variable used as flag
        } else if (PORTBbits.RB4 == 1) {
            CN1flag = 0; // user defined global variable used as flag
        }
        // If RA4 = 0 set flag = 1 else if RA4 = 1 set flag = 0
        if (PORTAbits.RA4 == 0) {
            CN0flag = 1; // user defined global variable used as flag
        } else if (PORTAbits.RA4 == 1) {
            CN0flag = 0; // user defined global variable used as flag
        }
    }

    IFS1bits.CNIF = 0; // clear IF flag
    Nop();

    ioChange = 1;
    return;
}

int frequencyTimerRun = 0; // flag to tell if it is the first or second rising edge

void __attribute__((interrupt, no_auto_psv)) _IC1Interrupt(void) {

    // first time interrupt is triggered (first rising edge)
    if (frequencyTimerRun == 0) {
        frequencyTimerRun = 1; // set flag that first rising edge happened
        StartStopwatch(); // start timer2
    } 
    // second time interrupt is triggered (second rising edge)
    else {
        ticks = StopStopwatch(); // stops timer2 and gets values
        frequencyTimerRun = 0; // reset flag
        risingEdges = 1; // measuring period is done
    }

    //risingEdges++;
    IFS0bits.IC1IF = 0; //Clear IC1 interrupt flag
}