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

void readADC(char channel){
    // Select A2D channel to read
    ADCON0 = channel << 2; // Shift signal into appropriate data registers
    ADCON0bits.ADON = 1;
    ADCON0bits.GO = 1;
    while(ADCON0bits.GO_NOT_DONE){__delay_ms(5);}
}

void readIR(void){
    readADC(0);
    int res = ADRESH<<8 | ADRESL;
    IR_signal = (res > THIR) ? 1 : 0;
}

void readMAG(void){
    readADC(1);
    int res = ADRESH<<8 | ADRESL;
    MAG_signal = (res < THMAG) ? 1 : 0;
}

void readCOND(void){
    readADC(2);
    int res = ADRESH<<8 | ADRESL;
    COND_signal = (res > THCOND) ? 1 : 0;
}