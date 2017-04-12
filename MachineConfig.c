#include <xc.h>
#include <stdio.h>
#include "configBits.h"
#include "constants.h"
#include "lcd.h"
#include "I2C.h"
#include "macros.h"
#include "UI.h"
#include "RTC.h"
#include "main.h"
#include "sort.h"
#include "ADCFunctionality.h"
#include "EEPROM.h"

void machineConfig(void) {
    //**OSCILLATOR**//
    OSCCON = 0xF0; // Force internal oscillator operation at 8 MHz (pg. 7-39)
    OSCTUNEbits.PLLEN = 1; // set  OSCTUNE<6> to enable PLL (4x  phase-lock-loop) thereby setting internal oscillator frequency to 32 MHz
    
    //**PIN I/O**//
    //TRIS SETS DATA DIRECTION: 0  = output; 1 = input. Default is  0
    TRISA = 0b00001111; // RA0, RA1, RA2, RA3 as inputs
                        // RA0 and RA1 are analog input, for the IR sensor and magnetism sensor respectively
    TRISB = 0b11110010; // RB1, RB4, RB5, RB6, RB7 as inputs
                        // RB1 is for the keypad interrupt
                        // RB4-RB7 are  for keypad  data
    TRISC = 0x00;       // RC0-RC2 are servo outputs. RC1 and RC2 have the PWM modules
                        // RC3 and RC4 are for the RTC
                        // RC5 is for IR sensor modulation
                        // RC6 and RC7 are for EUSART (PC Interface)
    TRISD = 0x00;       // RD0 and RD1 are unused
                        // RD2 is the LCD RS, RD3 is the LCD enable (E) bit
                        // RD4-RD7 are LCD data outputs!
    TRISE = 0x00;       // All of RE0-RE3 are unused
    //LATx SETS THE LEVEL OF THE OUTPUT LATCHES (written to as output). PORTx ARE READ AS INPUTS
    //HERE, THEY ARE ALL BEING CLEARED (SET LOW) TO ENSURE A CONTROLLED STARTUP STATE
    LATA = 0x00; // output low
    LATB = 0x00; // output low
    LATC = 0x00; // output low
    LATD = 0x00; // output low
    LATE = 0x00; // output low
    
    nRBPU = 0; // Enable PORTB's pull-up resistors
    
     //**A/D Converter Module**//
    ADCON0 = 0x00;  //Disable ADC
    ADCON1 = 0b00001011;  // bits 0-3 set pins with ADC capabilities as either analog  or digital. Here, we set AN0, AN1, AN2, and AN3 as analog inputs and the rest as digital
                    // bits 4-5 determine whether to use external voltage references or VDD and VSS
                    // Reference: pg. 7- 158
                    // 2017-04-10: changed from 0b00001101 to 0b00001100 for COND sensors.
                    // 2017-04-11: changed from 0b00001100 to 0b00001011 for new IR detector
    CVRCON = 0x00; // Disable comparator voltage reference (pg. 239 of datasheet)
    CMCONbits.CIS = 0; // Connect C1 Vin and C2 Vin to RA0/AN0 and RA1/AN1 (pg. 233 datasheet)
    ADCON2 = 0b10110001; // Right justify A/D result, 16TAD, FOSC/8 clock
    
    //**INTERRUPTS**//
    INT1IE = 1; // Enable external interrupts on INT1/RB1 (for keypad!)
    
    TMR0IE = 1; // Enable Timer0 interrupts
    TMR1IE = 1; // Enable Timer1 interrupts
    TMR2IE = 1; // Enable Timer2 interrupts
    TMR3IE = 1; // Enable Timer3 interrupts
    
    T1CON = 0b10110000;
    T2CON = 0b01111011; // 1:16 postscale, 16x prescale
    PR2 = 0x22; // TMR2 comparison register
    T3CON = 0b10110000;
    PEIE = 1; // Enable peripheral interrupts
    ADIE = 0; // Disable A/D conversion interrupts (important for readADC())
    di(); // Disable all interrupts for now
}
