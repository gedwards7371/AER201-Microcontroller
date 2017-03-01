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

void main(void) {
    machineConfig();
    initLCD();
    initTime(0x42, 0x44, 0x19, 0x06, 0x24, 0x02, 0x17); //sec, min, hour, weekday (0x01 is Sunday), day, month (0x01 is Jan), year (0x17)
    initUI();
    first = 1;
    
    //machine_state = Testing_state; // comment me out if you're not testing
    while(1){
        switch(machine_state){
            case UI_state :
                UI();
                break;
            case Sorting_state :
                sort();
                break;
            case DoneSorting_state :
                stopSignals(); // kill all outputs
                logRun();
                first = 1; // Permit the sort operation to be re-initialized
                UI(); // have a check for this state in the UI to display a special screen
                break;
            case Testing_state :
                PortTestA5();
                break;
            case EmergencyStop_state :
                stopSignals(); // kill all outputs
                // I think we will actually just kill MCLR using an external circuit though
                while(1){}
            default :
                break;
        }
    }
}

void stopSignals(void){
    LATA = 0x00; // output low
    LATB = 0x00; // output low
    LATC = 0x00; // output low
    LATD = 0x00; // output low
    LATE = 0x00; // output low
}