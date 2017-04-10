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

// Interrupt handler
void interrupt handler(void) {
    //** Interrupt handler for key presses **
    if(INT1IF){
        INT1IF = 0; // Clear flag bit
        input = keys[(PORTB & 0xF0) >> 4];
        if(input == '*'){
            switch(machine_state){
                case UI_state:
                    machine_state = Testing_state;
                    break;
                case Testing_state:
                    machine_state = UI_state;
                    change_state_to_menu_start();
                    break;
                case Sorting_state:
                    machine_state = DoneSorting_state;
                    break;
            }
        }
        else if(machine_state == UI_state){ // "If we're supposed to be in the UI..."
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
    
    //** Timer for pan servo PWM **
    if(TMR1IF){
        TMR1IF = 0; // Clear interrupt flag
        TMR1ON = 0;
        if(machine_state == Sorting_state){
            if(was_low_1){
                SERVOPAN = 1;
                was_low_1 = 0;
                TMR1H = servoTimes[0];
                TMR1L = servoTimes[1];
            }
            else{
                SERVOPAN = 0;
                was_low_1 = 1;
                TMR1H = 20000 - servoTimes[0];
                TMR1L = 20000 - servoTimes[1];
            }
            TMR1ON = 1;
            servo_timer_counter += 10;
            //servo_timer_counter += (float)((65535-((servoTimes[0] << 8) || (servoTimes[1] & 0xFF)))/1000);
            updateServoStates();
        }
    }
    
    //** Timer for arm servo PWM **
    if(TMR2IF){
        // TMR2IF triggers when TMR2 == PR2
        // PR2 = 0xFF by default --> set to some hexadecimal value for custom delays
        TMR2IF = 0; // Clear interrupt flag
        TMR2ON = 0;
        timer2_counter++;
        if(machine_state == Sorting_state){
            if(was_low_2 && (timer2_counter == 2)){
                // 2 * ~8 ms = ~16 ms 
                SERVOARM = 1;
                was_low_2 = 0;
                timer2_counter = 0;
            }
            else if(!was_low_2){
                SERVOARM = 0;
                was_low_2 = 1;
                timer2_counter = 0;
            }
            updateArmState();
            TMR2ON = 1;
        }
    }
    
    //** Timer for tilt servo PWM **
    if(TMR3IF){
        TMR3IF = 0; // Clear interrupt flag
        TMR3ON = 0;
        if(machine_state == Sorting_state){
            if(was_low_3){
                SERVOTILT = 1;
                was_low_3 = 0;
                TMR3H = servoTimes[2];
                TMR3L = servoTimes[3];
            }
            else{
                SERVOTILT = 0;
                was_low_3 = 1;
                TMR3H = 20000 - servoTimes[2];
                TMR3L = 20000 - servoTimes[3];
            }
            TMR3ON = 1;
        }
    }
}