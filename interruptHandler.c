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
            if(was_low){
                SERVOPAN = 1;
                was_low = 0;
                TMR1H = servoTimes[0];
                TMR1L = servoTimes[1];
            }
            else{
                SERVOPAN = 0;
                was_low = 1;
                TMR1H = 20000 - servoTimes[0];
                TMR1L = 20000 - servoTimes[1];
            }
            TMR1ON = 1;
        }
    }
    
    if(TMR3IF){
        TMR3IF = 0; // Clear interrupt flag
        TMR3ON = 0;
        if(machine_state == Sorting_state){
            if(was_low){
                SERVOTILT = 1;
                was_low = 0;
                TMR3H = servoTimes[2];
                TMR3L = servoTimes[3];
            }
            else{
                SERVOTILT = 0;
                was_low = 1;
                TMR3H = 20000 - servoTimes[2];
                TMR3L = 20000 - servoTimes[3];
            }
            TMR3ON = 1;
        }
    }
    
    /*
    //** Timer for IR sensor beam break
    if(TMR3IF){
        TMR3IF = 0;
        TMR3ON = 0;
        TMR3counter++;
        if(TMR3counter==10){
            TMR3counter=0;
            TMR3CF = 1;
        }
        else{
            TMR3H = 0b00111100;
            TMR3L = 0b10110000;
            TMR3ON = 1;
        }
    }
    */
}