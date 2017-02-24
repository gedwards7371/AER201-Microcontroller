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

void PortTestD0(void){
        while(PORTBbits.RB1 == 0){ 
            // RB1 is the interrupt pin, so if there is no key pressed, RB1 will be 0
            // the PIC will wait and do nothing until a key press is signaled
        }
        LATDbits.LATD0 = 1;
        while(PORTBbits.RB1 == 1){
            // Wait until the key has been released
        }
        Nop();  //Apply breakpoint here because of compiler optimizations
        Nop();
        LATDbits.LATD0 = 0;
}

void PortTestA5(void){
        while(PORTBbits.RB1 == 0){ 
            // RB1 is the interrupt pin, so if there is no key pressed, RB1 will be 0
            // the PIC will wait and do nothing until a key press is signaled
        }
        LATAbits.LATA5 = 1;
        while(PORTBbits.RB1 == 1){
            // Wait until the key has been released
        }
        Nop();  //Apply breakpoint here because of compiler optimizations
        Nop();
        LATAbits.LATA5 = 0;
}