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
    
    int i = 0; // counting variable
    /*****Algorithm test cases*****/
    

    /*****Sensor test cases*****/
    
    __lcd_clear();__lcd_home();
    
    /*****Actuator test cases*****/
    // DC Motor activation
    // Pass: pin RA5 outputs a high voltage
    // Justification: obligatory.
    printf("TST: DC MOTORS");
    __lcd_newline();
    printf("PIN RA5 HIGH.");
    DC = 1;
    for(i=0;i<5;i++){__delay_1s();}
    DC = 0;
    __lcd_clear();__lcd_home();
    
    // Top/bottom conductivity sensor solenoid activation
    // Pass: pin RA3 outputs a SOL_LOW voltage for TIME_SOLENOID_MOTION ms,
    //       followed by a ~SOL_LOW voltage for 1000-TIME_SOLENOID_MOTION ms.
    // Justification: obligatory.
    printf("TST: COND SOLN'D");
    __lcd_newline();
    printf("PIN RA3...");
    for(i=0;i<5;i++){
        __delay_ms(TIME_SOLENOID_MOTION);
        SOL_COND_SENSORS = ~SOL_OUT;
        __delay_ms(1000-TIME_SOLENOID_MOTION);
    }
    SOL_COND_SENSORS = ~SOL_OUT;
    __lcd_clear();__lcd_home();
    
    // Pusher solenoid activation
    // Pass: pin RA4 outputs a SOL_LOW voltage for TIME_SOLENOID_MOTION ms,
    //       followed by a ~SOL_LOW voltage for 1000-TIME_SOLENOID_MOTION ms.
    // Justification: obligatory.
    printf("TST: PUSH SOLN'D");
    __lcd_newline();
    printf("PIN RA4...");
    for(i=0;i<5;i++){
        __delay_ms(TIME_SOLENOID_MOTION);
        SOL_COND_SENSORS = ~SOL_OUT;
        __delay_ms(1000-TIME_SOLENOID_MOTION);
    }
    SOL_PUSHER = ~SOL_OUT;
    __lcd_clear();__lcd_home();
    
    // Pan servo control
    // Pass: (1) goes to middle for 2 s, (2) goes to extreme right for 2 s,
    //       (3) goes to extreme left for 2 s, (4) goes mid-right for 2 s,
    //       (5) goes mid-left for 2 s, (6) returns to middle
    // Justification: Tests the full range of motion, extreme cases, and Timer1.
    printf("TST: PAN SERVO");
    __lcd_newline();
    printf("MIDDLE -- 2s");
    updateServoPosition(PAN_MID, 1);
    TMR1ON = 1;
    __delay_1s();__delay_1s();
    
    __lcd_home();__lcd_newline();
    printf("RIGHT -- 2s");
    updateServoPosition(PAN_R, 1);
    __delay_1s();__delay_1s();
    
    __lcd_home();__lcd_newline();
    printf("LEFT -- 2s");
    updateServoPosition(PAN_L, 1);
    __delay_1s();__delay_1s();
    
    __lcd_home();__lcd_newline();
    printf("MID-LEFT -- 2s");
    updateServoPosition(PAN_LMID, 1);
    __delay_1s();__delay_1s();
    
    __lcd_home();__lcd_newline();
    printf("MID-RIGHT -- 2s");
    updateServoPosition(PAN_RMID, 1);
    __delay_1s();__delay_1s();
    
    __lcd_home();__lcd_newline();
    printf("MID -- 2s");
    updateServoPosition(PAN_MID, 1);
    __delay_1s();__delay_1s();
    
    TMR1ON = 0;
    
    // Tilt servo control
    // Pass: (1) tilts down (i.e. level with horizontal plane) for 2 s, 
    //       (2) tilts up to middle position (i.e. 45 degrees) for 2 s,
    //       (3) returns to level position
    // Justification: tests the full range of motion we need and Timer3.
    printf("TST: TILT SERVO");
    __lcd_newline();
    printf("LEVEL -- 2s");
    updateServoPosition(TILT_REST, 3);
    TMR3ON = 1;
    __delay_1s();__delay_1s();
    
    __lcd_home();__lcd_newline();
    printf("DROP -- 2s");
    updateServoPosition(TILT_DROP, 3);
    __delay_1s();__delay_1s();
    
    __lcd_home();__lcd_newline();
    printf("LEVEL -- 2s");
    updateServoPosition(TILT_REST, 3);
    __delay_1s();__delay_1s();
    
    TMR3ON = 0;
}