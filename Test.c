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
void BothServos(void);
void PortTests(void);
void PortTestA5(void);
void EEPROMTest(void);
void ToggleTestA5(void);
void SpeedTest(void);
void PusherTest(void);
void BlockerTest(void);

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
            case 1: // Key 1
                algorithmTest();
                break;
            case 2: // Key 2
                sensorTest();
                break;
            case 3: // Key 3
                actuatorTest();
                break;
            case 5: // Key 4
                PortTestA5();
                break;
            case 6: // Key 5
                //ToggleTestA5();
                SpeedTest();
                break;
            case 7: // Key 6
                EEPROMTest();
                break;
            case 8: // Key B
                PusherTest();
                break;
            case 9: // Key 7
                PortTests();
                break;
            case 10: // Key 8
                BothServos();
                break;
            case 11: // Key 9
                BlockerTest();
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
    IR_EMITTER = 1;
    while(PORTBbits.RB1 == 0){
        readADC(0);
        int res = ADRESH<<8 | ADRESL;
        IR_signal = (res > THIR) ? 1 : 0;
        
        __lcd_clear();__lcd_home();
        printf("IR_signal: %d ", IR_signal);
        __lcd_newline();
        printf("%d", res);
        __delay_ms(100);
     }  
    IR_EMITTER = 0;
    
    
    // Magnetism sensor reading
    // Pass: Presence of soup can sets MAG_signal high at 6mm, and
    //       presence of pop can sets MAG_signal high at 3mm.
    // Justification: These were the ranges we determined experimentally.
    __lcd_clear();__lcd_home();
    printf("TST: MAGNETISM");
    __delay_ms(100);
    while(PORTBbits.RB1 == 0){
        readMAG();
       __lcd_home();
       __lcd_newline();
       printf("MAG_signal: %d ", MAG_signal);
       __delay_ms(100);
    }
    
    // Conductivity sensor reading
    // Pass: 
    // Justification: 
    __lcd_clear();__lcd_home();
    printf("TST: COND");
    __delay_ms(100);
    while(1){
        __lcd_clear();__lcd_home();
        printf("TST: COND");
        while(PORTBbits.RB1 == 0){ 
            // RB1 is the interrupt pin, so if there is no key pressed, RB1 will be 0
            // the PIC will wait and do nothing until a key press is signaled
            
            __lcd_home();__lcd_newline();
            printf("COND: %d", COND_SENSORS);
        }
        if(PORTB >> 4 == 0b1111){
            break;
        }
        else{
            SOL_COND_SENSORS = SOL_OUT;
            while(PORTBbits.RB1 == 1){
                // Wait until the key has been released
                __lcd_home();__lcd_newline();
                printf("COND: %d", COND_SENSORS);
            }
            SOL_COND_SENSORS = ~SOL_OUT;
        }
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
    updateServoPosition(POP_TILT_DROP, 3);
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
    SERVOCAM = 1;
    __delay_1s();__delay_1s();
    __lcd_home();__lcd_newline();
    printf("CAM DWN |RC0=0");
    
    /* PWM to make the CAM servo work with Twesh's circuit */
    for(i=0;i<10000;i++)
    {
        SERVOCAM = 1;
        __delay_us(10);
        SERVOCAM = 0;
        __delay_us(90);
    }    
    
    __delay_1s();__delay_1s();
    machine_state = Testing_state;
}

void BothServos(void){
    __lcd_clear();__lcd_home();
    printf("TST: BOTH SERVOS");
    TMR1ON =  1;
    TMR3ON = 1;
    machine_state = Sorting_state;
    was_low_1 = 0;
    was_low_3 = 0;
    
        // Combined control over both servos
    // Pass: (1) Pan servo vists R, RMID, MID, LMID, L
    //       (2) At each pan position, tilt servo tilts up then goes back down
    // Justification: tests the full range of motion we need for both servos
    initServos();
    __delay_ms(1500);
    
    updateServoPosition(PAN_R, 1);
    updateServoPosition(TILT_REST, 3);
    __delay_ms(750);
    updateServoPosition(POP_TILT_DROP, 3);
    __delay_ms(TILT_DROP_DELAY);
    updateServoPosition(TILT_REST, 3);
    __delay_ms(750);

    updateServoPosition(PAN_RMID, 1);
    updateServoPosition(TILT_REST, 3);
    __delay_ms(750);
    updateServoPosition(POP_TILT_DROP, 3);
    __delay_ms(TILT_DROP_DELAY);
    updateServoPosition(TILT_REST, 3);
    __delay_ms(750);

    updateServoPosition(PAN_MID, 1);
    updateServoPosition(TILT_REST, 3);
    __delay_ms(1500);
    
    updateServoPosition(PAN_LMID, 1);
    updateServoPosition(TILT_REST, 3);
    __delay_ms(750);
    updateServoPosition(SOUP_TILT_DROP, 3);
    __delay_ms(TILT_DROP_DELAY);
    updateServoPosition(TILT_REST, 3);
    __delay_ms(750);
    
    updateServoPosition(PAN_L, 1);
    updateServoPosition(TILT_REST, 3);
    __delay_ms(750);
    updateServoPosition(SOUP_TILT_DROP, 3);
    __delay_ms(TILT_DROP_DELAY);
    updateServoPosition(TILT_REST, 3);
    __delay_ms(750);
    
    TMR1ON = 0;
    TMR3ON = 0;
    di();
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
            LATAbits.LATA5 = 0;
        }
    }
}

void PusherTest(void){
    __lcd_clear();__lcd_home();
    printf("PUSHER TST");
    
    while(1){
         while(PORTBbits.RB1 == 0){ 
            // RB1 is the interrupt pin, so if there is no key pressed, RB1 will be 0
            // the PIC will wait and do nothing until a key press is signaled
        }
        if(PORTB >> 4 == 0b1111){
            break;
        }
        //int looptime = 0.1 / (uptime_us / 1000000);
        
        else if(PORTB >> 4 == 0b0101){
           for(int i = 0; i<3000; i++){
            SOL_PUSHER = 1; // activate solenoid pusher
            __delay_us(75);
            SOL_PUSHER = 0;
            __delay_us(25);
           }
        }
        else if (PORTB >> 4 == 0b110){
            for(int i = 0; i<3000; i++){
            SOL_PUSHER = 1; // activate solenoid pusher
            __delay_us(58);
            SOL_PUSHER = 0;
            __delay_us(42);
           }
        }

        while(PORTBbits.RB1 == 1) {continue;}
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
            continue;
        }
        if(PORTB >> 4 == 0b1111){
            break;
        }
        else{
            while(1){
                LATAbits.LATA5 = 1;
                __delay_ms(3000);
                LATAbits.LATA5 = 0;
                __delay_ms(1000);
            }  
        }
    }
}

void SpeedTest(void){
    // Turns the trommel at half speed, on keypress
    __lcd_clear();__lcd_home();
    printf("S: D WILL RETURN ");
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
            while(PORTBbits.RB1 == 1){
             LATAbits.LATA5 = 1;
             __delay_ms(5);
             LATAbits.LATA5 = 0;
             __delay_ms(5);
                
                // Wait until the key has been released
            }
            LATAbits.LATA5 = 0;
        }
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

void BlockerTest(void){
    __lcd_clear();__lcd_home();
    printf("D WILL RETURN    ");
    __lcd_newline();
    printf("OTHER TOGGLES CAM");
    int flag = 1; // 1 for up, 0 for down
    while(1){
        while(PORTBbits.RB1 == 0){ continue; }
        if(PORTB >> 4 == 0b1111){
            break;
        }
        else{
            if(flag){
               SERVOCAM = 1;
               flag = !flag;
            }
            else{
                for(int i=0;i<5000;i++)
                {
                    SERVOCAM = 1;
                    __delay_us(10);
                    SERVOCAM = 0;
                    __delay_us(90);
                }
                SERVOCAM = 0;
                flag = !flag;
            }
        }
        while(PORTBbits.RB1 == 1){ continue; /*If button still pressed*/}
    }
}