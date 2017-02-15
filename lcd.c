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

void lcdInst(char data) {
    RS = 0; // register select is 0 so that the LCD knows to interpret the data as commands, not characters
    lcdNibble(data);
}

void putch(char data){
    RS = 1; // register select is set to 1 so that the LCD is expecting character data
    lcdNibble(data);
}

void lcdNibble(char data){
    // Send of 4 most sig bits, then the 4 least sig bits (MSD,LSD)
    char temp = data & 0xF0; // mask 4 msb
    LATD = LATD & 0x0F; // clears LATD[4:7] by setting to 0; preserves state of LATD[0:3]
    LATD = temp | LATD; // Writes temp to LATD[4:7]]

    E = 1;
    __delay_us(LCD_DELAY);
    E = 0;    
    __delay_us(LCD_DELAY);
    
    data = data << 4; // left shift 4 bits
    
    temp = data & 0xF0;
    LATD = LATD & 0x0F;
    LATD = temp | LATD;

    E = 1;
    __delay_us(LCD_DELAY);
    E = 0;
    __delay_us(LCD_DELAY);
}

void initLCD(void) {
    __delay_ms(15);
    lcdInst(0b00110011); // first 2 sets of "0011" to get LCD into known 8-bit state
    lcdInst(0b00110010); // first 0011 -> LCD now expecting 8-bit instructions
                         // 0010 for... ?
    lcdInst(0b00101000); // 0010 to set 4-bit data length, 1 to set 2-lines, 0 to set 5x7 font, 00 don't care
    lcdInst(0b00001100); // 00001100 to hide cursor but keep it in underline mode
    lcdInst(0b00000110); // 1 for increment each byte written to the display, 0 for display shift off
    lcdInst(0b00000001); // clear display
    __delay_ms(15);
}