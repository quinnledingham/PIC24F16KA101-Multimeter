/*
 * 
 * File:   CTMU.c
 * Authors: Maram Elsayed, Quinn Ledingham, Jack Li
 *
 */

#include <xc.h>
#include <P24F16KA101.h>
#include "CTMU.h"
#include "ADC.h"
#include "TimeDelay.h"
#include "Clock.h"

void CTMUinit() {
    CTMUCON = 0x0000; // make sure CTMU is disabled
    CTMUCONbits.CTMUEN = 0; // disable the CTMU
    CTMUCONbits.TGEN = 1;
    CTMUCONbits.EDG1POL = 0;
    CTMUCONbits.EDG2POL = 0;

    // CTMU continues to run when emulator is stopped, CTMU continues to run in Idle
    // mode, Time Generation mode enabled, Edges are blocked. No edge sequence order,
    // Analog current source not grounded, trigger output disabled, 
    // Edge2 polarity = positive level, Edge2 source = source, Edge1 polarity = positive level,
    // Edge1 source = source 0, Set Edge status bits to zero
    // CTMUICON - CTMU Current Control Register
    CTMUICON = 0x0000;

    CTMUICONbits.IRNG0 = 0; // .55uA, Nominal - No Adjustment
    CTMUICONbits.IRNG1 = 0; // .55uA, Nominal - No Adjustment
    
       // Set up AD Converter
    AD1CON1bits.ADON = 0;     // Turn off ADC, turn on before sampling in do_ADC()
    AD1CON1bits.ADSIDL = 0;   // Continue ADC operation in idle mode
    AD1CON1bits.FORM = 0b00;  // 0b00: Int, 01: Signed Int, 10:Fractional, 11:Signed Fract
    AD1CON1bits.SSRC = 0b000; // Start ADC conversion Time set by SAMC has passed
    AD1CON1bits.ASAM = 0;     // Next Sampling begins when SAMP bit is set
    AD1CON1bits.SAMP = 0;     // Set this bit in ADC routine to start sampling
    
    // Sampling time and ADC Clk select
    AD1CON2bits.VCFG = 0b000;  // Vref+=AVDD   Vref-=AVSS
    AD1CON2bits.CSCNA = 0;     // Do not scan inputs; use channel selected by CH0SA bits
    AD1CON2bits.SMPI = 0b0000; // Any ADC interrrups occur at completion of each sample/convert seq.
    AD1CON2bits.BUFM = 0;      // Buffer confugured as 1 16 word buffer
    AD1CON2bits.ALTS = 0;      // Always use MUXA input MUX settings
    AD1CON3bits.ADRC = 0;      // 0: Use System clk; 1: ADC uses internal RC clock for idle or sleep mode
    AD1CON3bits.SAMC = 0;
    AD1CON3bits.ADCS = 0; // ADCS conversion clock select bits. Ignored if using internal AD RC clock.

    // MUX settings
    AD1CHSbits.CH0NB = 0;      // Ch0 MUXB -ve Input is VR-
    AD1CHSbits.CH0SB = 0b0011; // YOUR ADC CHANNEL HERE AN3
    AD1CHSbits.CH0NA = 0;      // Ch0 -ve Input is VR-
    AD1CHSbits.CH0SA = 0b0011; // YOUR ADC CHANNEL HERE AN3

    // IO port selection
    TRISBbits.TRISB1 = 1;  // SET YOUR PIN AS AN INPUT RB1
    AD1PCFGbits.PCFG3 = 0; // SET YOUR PIN AS A VOLTAGE AN3/RB1/PIN5
    AD1CON1bits.ADON = 1;

    CTMUCONbits.CTMUEN = 1; // Enable the CTMU
}

#define COUNT 500
#define ETIME COUNT*2.5     //time in uS
#define DELAY for(i=0; i<COUNT; i++)
#define RCAL .027         // R value is 4200000 (4.2M)

uint16_t capacitanceMeasurement() {

    int i;
    int j = 0;
    
    unsigned int Vread=0;
    unsigned int CTMUISrc, CTMUCap, Vavg, VTot = 0, Vcal;
    initTimer2();
    CTMUinit();
    NewClk(8);
    for (j = 0; j < 1; j++) {

        AD1CON1bits.SAMP = 1; // Manual sampling start
        CTMUCONbits.IDISSEN = 1; // drain charge on the circuit
        
        Delay_ms(10);  // wait 62.5 us
        
        CTMUCONbits.IDISSEN = 0; // end drain of circuit
        
        CTMUCONbits.EDG1STAT = 1; // Begin charging the circuit
        // using CTMU current source
        
        Delay_ms(10);  // wait 62.5 us
        
        CTMUCONbits.EDG1STAT = 0; // Stop charging the circuit
        
        IFS0bits.AD1IF = 0; // make sure A/D Int not set
        AD1CON1bits.SAMP = 0; // and begin A/D conv.
        while(!IFS0bits.AD1IF) {}  // Wait for A/D convert complete
        //AD1CON1bits.DONE = 0;
        Vread = ADC1BUF0; // Get the value from the A/D
        IFS0bits.AD1IF = 0; // Clear A/D interrupt flag
        VTot += Vread; // Add the reading to the total
    }

    Vavg = (VTot / 1); // 1 Measurement
    //Vcal = (Vavg / 1023 * 3.5);
    //CTMUISrc = Vcal / RCAL;
    //CTMUCap = (CTMUISrc * 1 / Vcal);
    //Vcal= (Vavg/1023*3.5);
   // CTMUISrc= Vcal/0.027;
    //CTMUCap= (CTMUISrc*ETIME/Vcal)/100;
    //Disp2Dec(Vavg);
    //XmitUART2('\n', 1);
    //mitUART2('\r', 1);
    VTot = 0;
    NewClk(8);
    return (uint16_t)Vavg;
}