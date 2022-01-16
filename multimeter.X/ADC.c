/*
 * 
 * File:   ADC.c
 * Authors: Maram Elsayed, Quinn Ledingham, Jack Li
 *
 */

#include "ADC.h"
#include "UART2.h"

// Global Variables
uint16_t ADC_1 = 0;
uint16_t ADC_2 = 0;
uint16_t ADC_3 = 0;
uint16_t ADC_4 = 0;
uint16_t ADC_5 = 0;
uint16_t ADC_6 = 0;
uint16_t ADC_7 = 0;
uint16_t ADC_8 = 0;
uint16_t ADC_9 = 0;
uint16_t ADC_10 = 0;
uint16_t ADC_11 = 0;
uint16_t ADC_12 = 0;
uint16_t ADC_13 = 0;
uint16_t ADC_14 = 0;
uint16_t ADC_15 = 0;
uint16_t ADC_16 = 0;

uint16_t ADC_ARRAY[500];

// ADC initialization: Call before using do_ADC() and do_ADCseq()
// Sets up ADC on pin8/RA3/AN5

void ADCinit(uint16_t port) {
    /*-----------ADC INITIALIZATION-----------*/
    AD1CON1bits.ADON = 0; // Turn off ADC, turn on before sampling in do_ADC()
    AD1CON1bits.ADSIDL = 0; // Continue ADC operation in idle mode
    AD1CON1bits.FORM = 0b00; // 0b00: Int, 01: Signed Int, 10:Fractional, 11: Signed Fract
    AD1CON1bits.SSRC = 0b111; // Start ADC conversion Time set by SAMC has passed
    AD1CON1bits.ASAM = 0; // Next Sampling begins when SAMP bit is set
    AD1CON1bits.SAMP = 0; // Set this bit in ADC routine to start sampling

    // Sampling time and ADC Clk select
    AD1CON2bits.VCFG = 0b000; // Vref+=AVDD Vref-=AVSS
    AD1CON2bits.CSCNA = 0; // Do not scan inputs; use channel by CHOSA bits
    AD1CON2bits.SMPI = 0b0111; // Any ADC interrupts occur at completion of each
    // sample/convert seq.
    //AD1CON2bits.BUFM = 0; // Buffer configured as 1 16 word buffer
    AD1CON2bits.BUFM = 1; // Buffer configured as 1 8 word buffer
    AD1CON2bits.ALTS = 0; // Always use MUXA input MUX settings
    AD1CON3bits.ADRC = 0; // 0: Use System clk; 1: ADC uses internal RC clock
    // for idle or sleep mode
    AD1CON3bits.SAMC = 0b11111; // Sample time = 31*TAD = 31*1/125e3 = 0.248ms
    AD1CON3bits.ADCS = 0b111111; // ADCS conversion clock select bits 64 Tcy.
    // Ignored if using internal AD RC clock.
    // TAD = (2/8Mhz) * (64) = 0.016 ms
    // SAMC = 31 * 0.016 ms = 0.496 ms    
    // 1 conversion in 12 TADs: conv = 0.192ms
    // full conversion - ~0.6ms
    // 250 Hz = 4 ms period

    // IO port selection
    // RA3
    if (port == 3) {
        // MUX settings
        AD1CHSbits.CH0NB = 0; // Ch0 MUXB -ve Input is VR-
        AD1CHSbits.CH0SB = 0b0101; // Positive input is AN5/RA3/pin8 for MUXB
        AD1CHSbits.CH0NA = 0; // Ch0 -ve Input is VR-
        AD1CHSbits.CH0SA = 0b0101; // Positive input is AN5/RA3/pin8 for MUXA

        TRISAbits.TRISA3 = 1; // Set pin8/RA3/AN5 as input
        AD1PCFGbits.PCFG5 = 0; // Set only pin8/AN5/RA3 as Analog input for ADC
    }
    // RB13
    if (port == 13) {
        // MUX settings
        AD1CHSbits.CH0NB = 0; // Ch0 MUXB -ve Input is VR-
        AD1CHSbits.CH0SB = 0b1011; // Positive input is AN5/RA3/pin8 for MUXB
        AD1CHSbits.CH0NA = 0; // Ch0 -ve Input is VR-
        AD1CHSbits.CH0SA = 0b1011; // Positive input is AN5/RA3/pin8 for MUXA

        TRISBbits.TRISB13 = 1;
        AD1PCFGbits.PCFG11 = 0;
    }
    // RB12
    if (port == 12) {
        // MUX settings
        AD1CHSbits.CH0NB = 0; // Ch0 MUXB -ve Input is VR-
        AD1CHSbits.CH0SB = 0b1100; // Positive input is AN5/RA3/pin8 for MUXB
        AD1CHSbits.CH0NA = 0; // Ch0 -ve Input is VR-
        AD1CHSbits.CH0SA = 0b1100; // Positive input is AN5/RA3/pin8 for MUXA

        TRISBbits.TRISB12 = 1;
        AD1PCFGbits.PCFG12 = 0;
    }

    AD1PCFG = 0xFFFF; // Set all bits as digital   
    AD1CSSL = 0; // Input Scan disabled, 0x0000 is default state.

    // INT bit setup
    IPC3bits.AD1IP = 5;

    AD1CON1bits.ADON = 1; // Turn on ADC module

    return;
}

void ADCinitCTMU() {
    AD1CON1bits.ADON = 0; // Turn off ADC, turn on before sampling in do_ADC()
    AD1CON1bits.ADSIDL = 0; // Continue ADC operation in idle mode
    AD1CON1bits.FORM = 0b00; // 0b00: Int, 01: Signed Int, 10:Fractional, 11: Signed Fract
    AD1CON1bits.SSRC = 0b000; // Start ADC conversion Time set by SAMC has passed
    AD1CON1bits.ASAM = 0; // Next Sampling begins when SAMP bit is set
    AD1CON1bits.SAMP = 0; // Set this bit in ADC routine to start sampling

    // Sampling time and ADC Clk select
    AD1CON2bits.VCFG = 0b000; // Vref+=AVDD Vref-=AVSS
    AD1CON2bits.CSCNA = 0; // Do not scan inputs; use channel by CHOSA bits
    AD1CON2bits.SMPI = 0b0000; // Any ADC interrupts occur at completion of each
    // sample/convert seq.
    //AD1CON2bits.BUFM = 0; // Buffer configured as 1 16 word buffer
    AD1CON2bits.BUFM = 0; // Buffer configured as 1 8 word buffer
    AD1CON2bits.ALTS = 0; // Always use MUXA input MUX settings
    AD1CON3bits.ADRC = 0; // 0: Use System clk; 1: ADC uses internal RC clock
    // for idle or sleep mode
    AD1CON3bits.SAMC = 0; // Sample time = 31*TAD = 31*1/125e3 = 0.248ms
    AD1CON3bits.ADCS = 0; // ADCS conversion clock select bits 64 Tcy.

    AD1CHSbits.CH0NB = 0; // Ch0 MUXB -ve Input is VR-
    AD1CHSbits.CH0SB = 0b0101; // Positive input is AN5/RA3/pin8 for MUXB
    AD1CHSbits.CH0NA = 0; // Ch0 -ve Input is VR-
    AD1CHSbits.CH0SA = 0b0101; // Positive input is AN5/RA3/pin8 for MUXA

    TRISAbits.TRISA3 = 1; // Set pin8/RA3/AN5 as input
    AD1PCFGbits.PCFG5 = 0; // Set only pin8/AN5/RA3 as Analog input for ADC
    AD1CON1bits.ADON=1;

}

// ADC conversion subroutine - Returns Single conversion 10bit ADC value (ADCvalue)
// in unsigned int form

uint16_t do_ADC(void) {
    uint16_t ADCvalue = 0; // 16 bit register used to hold ADC converted digital
    // AD1CON1bits.SAMP = 1; // Start Sampling, Conversion starts automatically after
    // SSRC and SAMC settings

    IFS0bits.AD1IF = 0; // clear ADC interrupt flag
    AD1CON1bits.ASAM = 1; // auto start sampling for 31Tad
    // then go to conversion
    while (!IFS0bits.AD1IF) {
    }; // conversion done?
    AD1CON1bits.ASAM = 0; // yes then stop sample/convert

    // 16 values. used in measure function.
    if (AD1CON2bits.BUFS == 1) {
        ADC_1 = ADC1BUF0;
        ADC_2 = ADC1BUF1;
        ADC_3 = ADC1BUF2;
        ADC_4 = ADC1BUF3;
        ADC_5 = ADC1BUF4;
        ADC_6 = ADC1BUF5;
        ADC_7 = ADC1BUF6;
        ADC_8 = ADC1BUF7;
    } else {
        ADC_9 = ADC1BUF8;
        ADC_10 = ADC1BUF9;
        ADC_11 = ADC1BUFA;
        ADC_12 = ADC1BUFB;
        ADC_13 = ADC1BUFC;
        ADC_14 = ADC1BUFD;
        ADC_15 = ADC1BUFE;
        ADC_16 = ADC1BUFF;
    }
    
    // function
    ADCvalue = ADC_1; // get one value
    return (ADCvalue); // returns 1 value
}

uint32_t measure(void) {
    unsigned int ctr = 0; // measurement counter
    uint32_t total = 0; // total of adc values

    int i = 0;
    // do 500 measurements twice to get 1000
    while (i < 2) {
        while (ctr < 500) {
            do_ADC(); // Does ADC conversion on AN5/IO8
            if (AD1CON2bits.BUFS == 1) {
                ADC_ARRAY[ctr] = ADC_1;
                ctr++;
                ADC_ARRAY[ctr] = ADC_2;
                ctr++;
                ADC_ARRAY[ctr] = ADC_3;
                ctr++;
                ADC_ARRAY[ctr] = ADC_4;
                ctr++;
                ADC_ARRAY[ctr] = ADC_5;
                ctr++;
                ADC_ARRAY[ctr] = ADC_6;
                ctr++;
                ADC_ARRAY[ctr] = ADC_7;
                ctr++;
                ADC_ARRAY[ctr] = ADC_8;
                ctr++;
            } else {
                ADC_ARRAY[ctr] = ADC_9;
                ctr++;
                ADC_ARRAY[ctr] = ADC_10;
                ctr++;
                ADC_ARRAY[ctr] = ADC_11;
                ctr++;
                ADC_ARRAY[ctr] = ADC_12;
                ctr++;
                ADC_ARRAY[ctr] = ADC_13;
                ctr++;
                ADC_ARRAY[ctr] = ADC_14;
                ctr++;
                ADC_ARRAY[ctr] = ADC_15;
                ctr++;
                ADC_ARRAY[ctr] = ADC_16;
                ctr++;
            }
        }
        unsigned int ctr = 0;
        // add up all measurements
        while (ctr < 500) {
            total = total + ADC_ARRAY[ctr]; // add values
            ctr++; // next value
        }
        i++; // next 500
    }

    total = total / 1000; // get average value

    return total; // return average
}

// Does ADC on AN5 and displays ADCBUF0 value and proportional number of markers
// on Terminal

void DispADC(void) {
    /*
    uint16_t adcbuf; // variable to store ADCBUF0 value
    adcbuf = do_ADC(); // Does ADC converstion on AN5/IO8
    
    Disp2String("\e[?25l");
    //XmitUART2('\n', 1); // new line sent
    XmitUART2('\r', 1); // write over line
    Disp2Hex(adcbuf); // Displays ADCBUF0 value at end of markers
     */
    unsigned int ctr = 0;
    uint32_t total = 0;
    while (ctr < 500) {

        //Disp2Dec(ADC_ARRAY[ctr]); // Displays ADCBUF0 value at end of markers
        //XmitUART2('\n', 1); // write over line
        //ctr++;

        total = total + ADC_ARRAY[ctr];
        //Disp2Dec(total);
        ctr++;
    }
    total = total / 500;
    Disp2Dec(total, 5); // Displays ADCBUF0 value at end of markers
    XmitUART2('\n', 1); // write over line
    XmitUART2('\r', 1); // write over line
    return;
}

// ADC interrupt subroutine

void __attribute__((interrupt, no_auto_psv)) _ADC1Interrupt(void) {
    IFS0bits.AD1IF = 0; // Clear the ADC1 Interrupt Flag
}
