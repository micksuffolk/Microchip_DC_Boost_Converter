/*
 * File:   main.c
 * Author: Mike Suffolk
 */


// MPLAB X IDE will select the correct PIC16F887.h file if "xc.h" is called.
#include <xc.h>


// Configure the PIC device (Important Parameters).
#pragma config FOSC = 0x04  // Internal Oscillator
#pragma config WDTE = 0x00  // Wathdog disabled
#pragma config PWRTE = 0x00 // Power up timer enabled
#pragma config MCLRE = 0x00 // MCLR internally tied to Vdd
#pragma config CP = 0x01    // No program code protection
#pragma config CPD = 0x01   // No data memory protection
#pragma config BOREN = 0x03 // Brown out reset enabled
#pragma config IESO = 0x00  // Internal / External switchover disabled
#pragma config FCMEN = 0x01 // Fail safe clock monitor enabled
#pragma config LVP = 0x00   // Low Voltage programming not enabled
#pragma config DEBUG = 0x01 // In-circuit debugger disabled
#pragma config BOR4V = 0x01 // Brown out reset at 4.0V
#pragma config WRT = 0x03   // Write protection off
#define _XTAL_FREQ 8000000  // Oscillator freq. Hz for __delay_ms() function

// Program variables & definitions.
unsigned int PWM_init_complete;
unsigned int DUTY_Scaled;
unsigned int AI_raw;
unsigned int AI_filt;
unsigned int Delay_1;
unsigned int AI_Conversion_DN;
unsigned int Duty_Cycle_up_DN;
unsigned int Duty_Cycle_dw_DN;

void PWM_init(void);
void main(void)

{ 
// First line of code delays the processor.
// Allows power supplies to switch on and stabilise.
__delay_ms(100);

// Call to initialise and configure PIC.
PWM_init();

// Perform an endless program loop.
while(PWM_init_complete == 1)
{

// When ADC conversion complete, move result into AI_Control1 register.
// ADRESL & ADRESH contain the analogue result as a 10-bit value (0d to 1023d)
// i.e 0V measured = 0d, 5V measured = 1023d
if (ADCON0bits.nDONE == 0 && ADCON0bits.GO == 0 && AI_Conversion_DN == 0)
    {
        AI_raw = ((ADRESH<<8) + ADRESL);
        AI_Conversion_DN = 1;
    }   

// Increase the software setpoint by increments of one.
if ((AI_filt < AI_raw) && Duty_Cycle_up_DN == 0)
    {
        AI_filt = AI_filt + 1;
        Duty_Cycle_up_DN = 1;
    }

// Decrease the software setpoint by increments of one.
if ((AI_filt > AI_raw) && Duty_Cycle_dw_DN == 0)
    {
        AI_filt = AI_filt - 1;
        Duty_Cycle_dw_DN = 1;
    }   

// Restart the ADC conversion cycle & reset DONE status words.
if (Delay_1 == 40)
    {
        ADCON0bits.GO = 1;
        ADCON0bits.nDONE = 1;
        AI_Conversion_DN = 0;
        Duty_Cycle_up_DN = 0;
        Duty_Cycle_dw_DN = 0;

// (AI/1023) * (_XTAL_FREQ / (PWM Frequency * TMR2PRESCALE));
// Use 75% as test PWM duty cycle setpoint.
        DUTY_Scaled = ((float)768/1023)*(8000000/(200000*1));

// Use AI as PWM duty cycle setpoint.
//      DUTY_Scaled = ((float)AI_filt/1023)*(8000000/(200000*1));


        // Write duty cycle setpoint to the PWM control registers.        
        CCPR1L  =   DUTY_Scaled >> 2;           // Put MSB 8 Bits in CCPR1L
        CCP1CON &=  0xCF;                       // Make bit 4 and 5 zero
        CCP1CON |=  (0x30&(DUTY_Scaled << 4));  // Assign last 2 LSBs to CCP1CON

// Reset the delay register to zero ready to increment again.
        Delay_1 = 0;
    }

// Increment the Delay value by one.
        Delay_1 = Delay_1 + 1;


// Main bulk of logic here...

}
}

void PWM_init(void)
{
//  Set variables to zero initially
    PWM_init_complete = 0;
    DUTY_Scaled = 0;
    AI_raw = 0;
    AI_filt = 0;
    Delay_1 = 0;
    AI_Conversion_DN = 0;
    Duty_Cycle_up_DN = 0;
    Duty_Cycle_dw_DN = 0;
    
//  Internal Oscillator Frequency Select Bits (8MHz)
    OSCCONbits.IRCF = 0b111;
    OSCCONbits.SCS = 1;
    OSCCONbits.HTS = 1;
    OSCCONbits.LTS = 1;
    OSCCONbits.OSTS = 0;
    OSCTUNEbits.TUN = 0b00000;
    
//  Disable AI channels which share the pins used as digital inputs or outputs
    ANSELHbits.ANS13 = 0;
    ANSELHbits.ANS12 = 0;
    ANSELHbits.ANS11 = 0;
    ANSELHbits.ANS10 = 0;
    ANSELHbits.ANS9 = 0;
    ANSELHbits.ANS8 = 1;    // Enable AN8, used as analogue input (POT)
    ANSELbits.ANS7 = 0;
    ANSELbits.ANS6 = 0;
    ANSELbits.ANS5 = 0;
    ANSELbits.ANS4 = 0;
    ANSELbits.ANS3 = 0;
    ANSELbits.ANS2 = 0;
    ANSELbits.ANS1 = 0;
    ANSELbits.ANS0 = 0;

//  Setup ADC configuration
    ADCON0bits.ADCS0 = 1;
    ADCON0bits.ADCS1 = 0;
    ADCON1bits.VCFG0 = 0;
    ADCON1bits.VCFG1 = 0;
    ADCON0bits.CHS = 0b1000;
    ADCON1bits.ADFM = 1;
    ADCON0bits.ADON = 1;
    ADCON0bits.GO = 1;
    ADCON0bits.nDONE = 1;
     
//  Setup channels as digital outputs or inputs
    TRISAbits.TRISA7=0;
    TRISAbits.TRISA6=0;
    TRISAbits.TRISA5=0;
    TRISAbits.TRISA4=0;
    TRISAbits.TRISA3=0;
    TRISAbits.TRISA2=0;
    TRISAbits.TRISA1=0;
    TRISAbits.TRISA0=0;

    TRISBbits.TRISB7=0;
    TRISBbits.TRISB6=0;
    TRISBbits.TRISB5=0;
    TRISBbits.TRISB4=0;
    TRISBbits.TRISB3=0;
    TRISBbits.TRISB2=0;
    TRISBbits.TRISB1=0;
    TRISBbits.TRISB0=0;
 
    TRISCbits.TRISC7=0;    
    TRISCbits.TRISC6=0;    
    TRISCbits.TRISC5=0;
    TRISCbits.TRISC4=0;
    TRISCbits.TRISC3=0;
    TRISCbits.TRISC2=0;
    TRISCbits.TRISC1=0;
    TRISCbits.TRISC0=0;
    
    TRISDbits.TRISD7=0;
    TRISDbits.TRISD6=0;
    TRISDbits.TRISD5=0;
    TRISDbits.TRISD4=0;
    TRISDbits.TRISD3=0;
    TRISDbits.TRISD2=0;
    TRISDbits.TRISD1=0;
    TRISDbits.TRISD0=0;    

    TRISEbits.TRISE3=0;
    TRISEbits.TRISE2=0;
    TRISEbits.TRISE1=0;
    TRISEbits.TRISE0=0;
    
//  Setup digital outputs as ON/OFF initially    
    PORTAbits.RA0 = 0;
    PORTAbits.RA1 = 0;
    PORTAbits.RA2 = 0;
    PORTAbits.RA3 = 0;
    PORTAbits.RA4 = 0;
    PORTAbits.RA5 = 0;
    PORTAbits.RA6 = 0;
    PORTAbits.RA7 = 0;

    PORTBbits.RB0 = 0;
    PORTBbits.RB1 = 0;
    PORTBbits.RB2 = 0;
    PORTBbits.RB3 = 0;
    PORTBbits.RB4 = 0;
    PORTBbits.RB5 = 0;
    PORTBbits.RB6 = 0;
    PORTBbits.RB7 = 0;
    
    PORTCbits.RC0 = 0;
    PORTCbits.RC1 = 0;
    PORTCbits.RC2 = 0;
    PORTCbits.RC3 = 0;
    PORTCbits.RC4 = 0;
    PORTCbits.RC5 = 0;
    PORTCbits.RC6 = 0;
    PORTCbits.RC7 = 0;

    PORTDbits.RD0 = 0;
    PORTDbits.RD1 = 0;
    PORTDbits.RD2 = 0;
    PORTDbits.RD3 = 0;
    PORTDbits.RD4 = 0;
    PORTDbits.RD5 = 0;
    PORTDbits.RD6 = 0;
    PORTDbits.RD7 = 0;

    PORTEbits.RE0 = 0;
    PORTEbits.RE1 = 0;
    PORTEbits.RE2 = 0;
    PORTEbits.RE3 = 0;

//  Setup PWM mode
    CCPR1L = 0x00;              // Zero PWM duty cycle initially
    CCP1CONbits.DC1B0 = 0;      // Zero PWM duty cycle initially
    CCP1CONbits.DC1B1 = 0;      // Zero PWM duty cycle initially
    CCP1CONbits.CCP1M = 0b1100; // CCP1 and CCP2 are configured for PWM
    CCP2CONbits.CCP2M = 0b1100; // CCP1 and CCP2 are configured for PWM
    CCP1CONbits.P1M = 0b00;     // Single PWM output at P1A
    PR2 = 0x09; // PWM Period = (PR2 + 1)*4*(1/Fosc)*(TMR2 Pre-scale) [200kHz]
    T2CONbits.T2CKPS = 0b00;    // Timer2 Pre-scale is 1
    T2CONbits.TOUTPS = 0b0000;  // Timer2 Post-scaler is 1:1
    TMR2 = 0x00;                // Zero Timer2
    T2CONbits.TMR2ON = 0b1;     // Timer2 is ON

// Timer 0 Setup //
    OPTION_REGbits.PSA = 0;     // Prescaler assigned to Timer 0
    OPTION_REGbits.PS = 0b111;  // Set the prescaler to 1:256
    OPTION_REGbits.T0CS = 0;    // Use the instruction clock (Fcy/4) for timer

// Set tag TRUE to inform program that initialisation of the PIC is complete.
    PWM_init_complete = 1;

// Delay processor for 100ms after initialising parameters
// this allows for all parameters to implement correctly.
    __delay_ms(100);
}