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

int i;
int var; // Holds PORTB
void Test(void);
void algorithmTest(void);
void sensorTest(void);
void actuatorTest(void);
void PortTests(void);
void PortTestA5(void);
void EEPROMTest(void);
void ToggleTestA5();

void Test(void){
    // Test cases for the algorithm, sensors, and actuators
    while(1){
        __lcd_clear();__lcd_home();
        printf("1.ALG|2.SNR|3.AC");
        __lcd_newline();
        printf("4.HI|5.TOG|6.EEP");
        __delay_ms(100);
        while(PORTBbits.RB1 == 0) {continue;}
        var = PORTB >> 4;
        var++;
        while(PORTBbits.RB1 == 1) {continue;}
        switch(var){
            case 1:
                algorithmTest();
                break;
            case 2:
                sensorTest();
                break;
            case 3: 
                actuatorTest();
                break;
            case 5:
                PortTestA5();
                break;
            case 6:
                ToggleTestA5();
                break;
            case 7:
                EEPROMTest();
                break;
            case 8:
                PortTests();
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
    int i = 0;
    
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
    machine_state = Testing_state;
}

void PortTests(void){
    // Same as PortTestA5 but with multiple keys
    while(1){
        while(PORTBbits.RB1 == 0) {continue;}
        var = PORTB >> 4;
        switch(var){
            case 0:
                LATEbits.LATE0 = 1;
                break;
            case 1:
                LATEbits.LATE1 = 1;
                break;
            case 2:
                LATDbits.LATD0 = 1;
                break;
            case 3:
                LATAbits.LATA3 = 1;
                break;
            case 4:
                LATAbits.LATA4 = 1;
                break;
            case 5:
                LATAbits.LATA5 = 1;
                break;
            default:
                break;
        }
        while(PORTBbits.RB1 == 1) {continue;}
        Nop();  //Apply breakpoint here because of compiler optimizations
        Nop();
        LATA = 0x00;
        LATE = 0x00;
        LATD = 0x00;   }
}

void PortTestA5(void){
    __lcd_clear();__lcd_home();
    printf("D WILL RETURN    ");
    __lcd_newline();
    printf("OTHER SETS RA5   ");
    while(1){
        while(PORTBbits.RB1 == 0){ 
            // RB1 is the interrupt pin, so if there is no key pressed, RB1 will be 0
            // the PIC will wait and do nothing until a key press is signaled
        }
        if(PORTB >> 4 == 0b1111){
            break;
        }
        else{
            LATAbits.LATA5 = 1;
            while(PORTBbits.RB1 == 1){
                // Wait until the key has been released
            }
            
            /* PWM to make the CAM servo work with Twesh's circuit
            for(i=0;i<10000;i++)
            {
                LATAbits.LATA5 = 1;
                __delay_us(10);
                LATAbits.LATA5 = 0;
                __delay_us(90);
            }    
            */
            
            LATAbits.LATA5 = 0;
        }
    }
}

void ToggleTestA5(void){
    __lcd_clear();__lcd_home();
    printf("D WILL RETURN   ");
    __lcd_newline();
    printf("ELSE TOGGLES RA5");
    int on = 0;
    while(1){
        while(PORTBbits.RB1 == 0){ 
            // RB1 is the interrupt pin, so if there is no key pressed, RB1 will be 0
            // the PIC will wait and do nothing until a key press is signaled
        }
        if(PORTB >> 4 == 0b1111){
            break;
        }
        else{
            if(on == 0){
                on = 1;
                LATAbits.LATA5 = 1;
            }
            else{
                on = 0;
                LATAbits.LATA5  = 0;
            }
        }
        while(PORTBbits.RB1 == 1) {continue;}
    }
}

void EEPROMTest(void){
    // Tests EEPROM_write(), EEPROM_read(), and logging cases
    
    // EEPROM_write tests
    // Pass: The print messages displayed to the screen match what you're writing
    // Justification: 
    
    // EEPROM_read tests
    // Pass: 
    // Justification: 
}