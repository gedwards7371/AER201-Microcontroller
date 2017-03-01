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

void Test(void){
    // Test cases for the algorithm, sensors, and actuators
    
    /*****Algorithm test cases*****/
    

    /*****Sensor test cases*****/
    
    
    /*****Actuator test cases*****/
    
}