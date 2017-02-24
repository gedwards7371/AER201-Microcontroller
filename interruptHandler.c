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

// Interrupt handler
void interrupt handler(void) {
    //** Interrupt handler for key presses: updates the menu state **
    if(INT1IF){
        INT1IF = 0;     //Clear flag bit
        if(machine_state == UI_state) { // "If we're supposed to be in the UI..."
            input = keys[(PORTB & 0xF0) >> 4];
            updateMenu();
        }
    }
    
    //** 1 SECOND TIMER THAT CALLS printSortTimer() **
    if(TMR0IF){
        TMR0IF = 0;
        if(machine_state == Sorting_state){
            printSortTimer();
            
            // Initialize timer again!
            T0CON = 0b00010111;
            TMR0H = 0b10000101;
            TMR0L = 0b11101110;
            TMR0ON = 1;
        }
    }
    
    //** Timer for servo delays **
    if(TMR1IF){
        TMR1IF = 0; // Clear interrupt flag
        TMR1ON = 0;
        if(machine_state == Sorting_state){
            T1CON = 0b10110000;
            if(was_low){
                //SERVOPAN = 1;
                SERVOTILT = 1;
                was_low = 0;              
                // Set Timer1 to interrupt after appropriate pulse time
                TMR1H = timer1highbits;
                TMR1L = timer1lowbits;
            }
            else{
                //SERVOPAN = 0;
                SERVOTILT = 0;
                was_low = 1;
                // Set Timer1 to interrupt on 20 ms
                TMR1H = timer1_20ms_high;
                TMR1L = timer1_20ms_low;
            }
            TMR1ON = 1;
        }
    }
    
    //** Timer for IR sensor beam break
    if(TMR2IF){
        TMR2IF = 0;
        TMR2ON = 0;
    }
}