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
#include "Test.h"

int debug;
int RTCflag;

void main(void) {
    machineConfig();
    initLCD();
    
    // Load RTC. Comment this out after it's been done the first time
    RTCflag = 0;
    if(RTCflag){
        initTime(0x42, 0x47, 0x17, 0x07, 0x08, 0x04, 0x17); //sec, min, hour, weekday (0x01 is Sunday), day, month (0x01 is Jan), year (0x17)
    }
    else{
        I2C_Master_Init(10000); //Initialize I2C Master with 100KHz clock
    }
    
    initUI();
    first = 1;
    initEEPROM();
    
    // machine_state = Testing_state; // comment this out if you're not testing.
                                   // This will make the machine enter a
                                   // function that tests/demonstrates the
                                   // functional units of this program.
    // debug = 1; // comment this out if you're not debugging. This will trigger
                  // print statements at various locations throughout the
                  // program.
    
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
                Test();
                break;
            case EmergencyStop_state :
                stopSignals(); // kill all outputs
                // I think we will actually just kill power using an external circuit though
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
    TMR1IE = 0;
    TMR2IE = 0;
    TMR3IE = 0;
}