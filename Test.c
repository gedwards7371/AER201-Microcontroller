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

int i = 0; // Counting variable
int var; // Holds PORTB
void Test(void);
void algorithmTest(void);
void sensorTest(void);
void actuatorTest(void);
void PortTestA5(void);

void Test(void){
    // Test cases for the algorithm, sensors, and actuators
    while(1){
        __lcd_clear();__lcd_home();
        printf("1. ALG  2. SNSRS");
        __lcd_newline();
        printf("3. ACTUATORS");
        __delay_ms(100);
        while(PORTBbits.RB1 == 0) {continue;}
        var = PORTB >> 4;
        while(PORTBbits.RB1 == 1) {continue;}
        switch(var){
            case 0:
                algorithmTest();
                break;
            case 1:
                sensorTest();
                break;
            case 2: 
                actuatorTest();
                break;
            default:
                break;
        }
    }
}

void algorithmTest(void){
    /*****Algorithm test cases*****/
    __lcd_clear();__lcd_home();
    printf("ALGORITHM TEST");
    __lcd_newline();
    printf("PUSH TO START");
    __delay_ms(100);
    while(PORTBbits.RB1 == 0) {continue;}
    while(PORTBbits.RB1 == 1) {continue;}
    __lcd_clear();__lcd_home();
    
    // 9 cans of same type
    // Pass: After time has elapsed, logs show 9 POPNOTAB.
    // Justification: Use case with max number of 1 type of can.
    printf("ALG1: 9 POPNOTAB");
    
    
    // 12 cans total/Very fast sort
    // Pass: Sort terminates before MAX_SORT_TIME and logs show 12 total cans.
    // Justification: Use case where sort operation completes in less than 3 min
    //                due to the maximum number of cans being sorted.
    printf("ALG2: 12 CANS");
    
    // 1 can total/Runtime longer than MAX_SORT_TIME
    // Pass: Sort ends after MAX_SORT_TIME elapses and logs show 1 total can.
    // Justification: Use case where fewer than the maximum number of cans are
    //                sorted.
    // NB: We may want to change this behaviour so that the sort stops after,
    //     say, no cans have been detected for X seconds, where X is determined
    //     by a sufficient amount of experimentation (e.g. 95% confidence in it)
    printf("ALG3: 1 CAN");
    
    
    // 3 of each type of can
    // Pass: Logs show 3 of each type of can
    // Justification: "Average" use case
    printf("ALG4: ALL CANS");
    
    // No cans
    // Pass: In the current implementation, this would be stopping after MAX_SORT_TIME
    // Justification: Edge case
    printf("ALG5: NO CANS");
}

void sensorTest(void){
    /*****Sensor test cases*****/
    __lcd_clear();__lcd_home();
    printf("SENSORS");
    __lcd_newline();
    printf("PUSH TO START");
    __delay_ms(100);
    while(PORTBbits.RB1 == 0) {continue;}
    while(PORTBbits.RB1 == 1) {continue;}
    __lcd_clear();__lcd_home();
    
    // IR sensor pair reading
    // Pass: IR_signal is default low but beam break sets it high.
    // Justification: Since we haven't tested this, this is a placeholder.
    printf("TST: IR SNSR");
    __delay_ms(100);
    while(PORTBbits.RB1 == 0){
        readIR();
       __lcd_home();
       __lcd_newline();
       printf("IR_signal: %d ", IR_signal);
       __delay_ms(100);
    }
    __lcd_clear();__lcd_home();
    
    while(PORTBbits.RB1 == 1) {continue;}
    
    // Magnetism sensor reading
    // Pass: Presence of soup can sets MAG_signal high at 6mm, and
    //       presence of pop can sets MAG_signal high at 3mm.
    // Justification: These were the ranges we determined experimentally.
    printf("TST: MAGNETISM");
    __delay_ms(100);
    while(PORTBbits.RB1 == 0){
        readMAG();
       __lcd_home();
       __lcd_newline();
       printf("MAG_signal: %d ", MAG_signal);
       __delay_ms(100);
    }
}

void actuatorTest(void){    
    /*****Actuator test cases*****/
    ei();
    __lcd_clear();__lcd_home();
    printf("ACTUATORS");
    __lcd_newline();
    printf("PUSH TO START");
    __delay_ms(100);
    while(PORTBbits.RB1 == 0) {continue;}
    while(PORTBbits.RB1 == 1) {continue;}
    __lcd_clear();__lcd_home();

    // DC Motor activation
    // Pass: pin RA5 outputs a high voltage.
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
        SOL_COND_SENSORS = SOL_OUT;
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
        SOL_PUSHER = SOL_OUT;
        __delay_ms(TIME_SOLENOID_MOTION);
        SOL_PUSHER = ~SOL_OUT;
        __delay_ms(1000-TIME_SOLENOID_MOTION);
    }
    SOL_PUSHER = ~SOL_OUT;
    __lcd_clear();__lcd_home();
    
    // Initialize code for all servos
    machine_state = Sorting_state;
    was_low_1 = 0;
    was_low_1 = 0;
    
    // Pan servo control
    // Pass: (1) goes to middle for 2 s, (2) goes to extreme right for 2 s,
    //       (3) goes to extreme left for 2 s, (4) goes mid-right for 2 s,
    //       (5) goes mid-left for 2 s, (6) returns to middle.
    // Justification: Tests the full range of motion, extreme cases, and Timer1.
    printf("TST: PAN SERVO");
    __lcd_newline();
    printf("MIDDLE -- 2s    ");
    updateServoPosition(PAN_MID, 1);
    TMR1ON = 1;
    __delay_1s();__delay_1s();
    
    __lcd_home();__lcd_newline();
    printf("RIGHT -- 2s     ");
    updateServoPosition(PAN_R, 1);
    __delay_1s();__delay_1s();
    
    __lcd_home();__lcd_newline();
    printf("LEFT -- 2s      ");
    updateServoPosition(PAN_L, 1);
    __delay_1s();__delay_1s();
    
    __lcd_home();__lcd_newline();
    printf("MID-LEFT -- 2s  ");
    updateServoPosition(PAN_LMID, 1);
    __delay_1s();__delay_1s();
    
    __lcd_home();__lcd_newline();
    printf("MID-RIGHT -- 2s ");
    updateServoPosition(PAN_RMID, 1);
    __delay_1s();__delay_1s();
    
    __lcd_home();__lcd_newline();
    printf("MID -- 2s       ");
    updateServoPosition(PAN_MID, 1);
    __delay_1s();__delay_1s();
    TMR1ON = 0;
    
    // Tilt servo control
    // Pass: (1) tilts down (i.e. level with horizontal plane) for 2 s, 
    //       (2) tilts up to middle position (i.e. 45 degrees) for 2 s,
    //       (3) returns to level position.
    // Justification: tests the full range of motion we need and Timer3.
    printf("TST: TILT SERVO");
    __lcd_newline();
    printf("LEVEL -- 2s     ");
    updateServoPosition(TILT_REST, 3);
    TMR3ON = 1;
    __delay_1s();__delay_1s();
    
    __lcd_home();__lcd_newline();
    printf("DROP -- 2s      ");
    updateServoPosition(TILT_DROP, 3);
    __delay_1s();__delay_1s();
    
    __lcd_home();__lcd_newline();
    printf("LEVEL -- 2s     ");
    updateServoPosition(TILT_REST, 3);
    __delay_1s();__delay_1s();
    
    TMR3ON = 0;
    di();
    
    // Cam servo control
    // Pass: RC0 goes from low to high to lower block
    // Justification: Tests what's needed to activate the cam servo
    __lcd_clear();__lcd_home();
    printf("TST: CAM SERVO");
    __lcd_newline();
    printf("CAM UP  |RC0=1");
    SERVOCAM = 0;
    __delay_1s();__delay_1s();
    __lcd_home();__lcd_newline();
    SERVOCAM = 1;
    printf("CAM DWN |RC0=0");
    __delay_1s();__delay_1s();
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