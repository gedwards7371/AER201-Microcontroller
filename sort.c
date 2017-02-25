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

void sort(void){
    if(machine_state == Sorting_state){
        Loading();
    }
    if(machine_state == Sorting_state){
        ID();
    }
    if(machine_state == Sorting_state){
        Distribution();
    }
}

void Loading(void){
    if(first){
        first = 0;
        initFlags();
        __lcd_clear();
        initSortTimer();
        
        //Write to RA5 for DC motors
        DC = 1;
        
        // Start sending pulses to servos
        initServos();
        __delay_1s();__delay_1s();
        updateServoPosition(PAN_R, 1);
        __delay_1s();__delay_1s();
        updateServoPosition(TILT_DOWN, 3);
        __delay_1s();__delay_1s();
        updateServoPosition(TILT_UP, 3);
        updateServoPosition(PAN_MID, 1);
        __delay_1s();__delay_1s();
        
        // moveServoBlock(Raise);
    }
    else{
        // If a can is not already waiting to go to the ID stage, we want the
        // code to be able to enter straight down to check if the ID stage is ready
        if(!f_loadingNewCan){
            // update f_loadingNewCan flag
            //getIR(); 
            // "If no new can is being loaded..."
            if(!f_loadingNewCan){
               return; // get out of sort function (i.e. restart it)
            }
            // "If a new can is being loaded"
            else{ 
                count_total++;
                if(count_total == 12){
                    f_lastCan = 1;
                }
            }
        }
        // "If a new can has already been loaded but the IR stage was not ready
        // the last time we executed this chunk...and if ID stage is ready..."
        else if(f_ID_receive){
            f_loadingNewCan = 0; // clear the new can flag after it's gone to ID
            SOL_PUSHER = 1; // activate solenoid pusher
            f_can_coming_to_ID = 1;
            
            //DELAY TIMER SET FOR ABOUT 500 MS DURATION (see? I need a dedicated duration-setting function!)
            SOL_PUSHER = 0; // deactivate solenoid pusher
        }
    }
}
void ID(void){
    if(f_can_coming_to_ID){
        //**characteristic delay based on time it takes can to be transported here
        __delay_ms(500); // IDK...& we could use Timer2 for this
        // Read conductivity sensor circuit. Because the solenoids are pushed in right now, this will tell us side of can conductivity
        int side_conductivity = 0;
        int magnetic = 0;
        int top_bottom_conductivity = 0;
        
        side_conductivity = PORTAbits.RA2; // note that this is a local variable because we don't want to use an old result, ever!
        if(!side_conductivity){
            readMAG();
            magnetic = MAG_signal;
            if(!magnetic){
                SOL_COND_SENSORS = 1; //activate solenoids for top/bottom conductivity sensors
                // characteristic delay for time it takes for solenoids move out
                __delay_ms(100); // IDK
                top_bottom_conductivity = PORTAbits.RA2;
                SOL_COND_SENSORS = 0;
            }
        }
        
        // use the local variables that we just initialized from the sensors to determine the can type
        //####moveServoBlock(Lower);
        // characteristic delay
        f_can_coming_to_distribution = 1;
        //####moveServoBlock(Raise);
        f_can_coming_to_ID = 0; // clear ID flag to allow another can to come
    }
}
void Distribution(void){
    if(f_can_coming_to_distribution){
        // delay characteristic of the time it takes a can to roll to the distribution cup from the ID stage
        //#####moveServoCup(canType);
        // delay characteristic of the time it takes a can to roll to the distribution cup from the ID stage
        //####moveServoCup(Home);
        f_can_coming_to_distribution = 0;
        f_can_distributed = 1;
    }
}

void initFlags(void){
    f_loadingNewCan = 0;
    f_lastCan = 0;
    f_ID_receive = 1;
    f_can_coming_to_ID = 0;
    f_can_coming_to_distribution = 0;
    f_can_distributed = 0;
}
void initSortTimer(void){
    // 1 second timer to generate interrupts
    getRTC();
    for(int i = 0; i < 7; i++){
        startTime[i] = __bcd_to_num(time[i]); // store the start time of the sort operation
    }
    // Configure 16-bit timer with 1:256 prescaler
    T0CON = 0b00010111;
    // Load timer with value such that only 1 interrupt needs to occur to get to 1s
    // 32000000 CPU cycles per second
    // * 0.25 instructions per CPU cycle
    // * (1/256) timer ticks per instruction
    // = 31250 timer ticks per second
    // Now, we have a total of 65536 (2^16) bits in the timer. We need to overflow it.
    // Therefore, load the timer with 65526-31250 = 34286
    // d'34286 = 0b1000010111101110
    TMR0H = 0b10000101;
    TMR0L = 0b11101110;
    TMR0ON = 1;
}
void initServos(void){
        updateServoPosition(PAN_MID, 1);
        updateServoPosition(TILT_UP, 3);
        TMR1ON = 1;
        TMR3ON = 1;
}
void printSortTimer(void){ 
    getRTC();
    int curTime[7];
    for(int i = 0; i < 7; i++){
        curTime[i] = __bcd_to_num(time[i]);
    }
    unsigned int start_sec = startTime[0] + startTime[1]*60 + startTime[2]*3600;
    unsigned int cur_sec = curTime[0] + curTime[1]*60 + curTime[2]*3600;
    unsigned int timeDiff = cur_sec - start_sec;
    
    total_time = timeDiff;
    
    if(timeDiff >= 10){
        machine_state = DoneSorting_state;
        // STOP EXECUTION (switch to DoneSorting_state and make sure loop executing will see this)
    }
    
    int hour = timeDiff / 3600;
    int min = (timeDiff % 3600) / 60;
    int sec = (timeDiff % 3600) % 60;
    
    __lcd_home();
    printf("Sorting...");
    __lcd_newline();
    printf("Time: %02d:%02d:%02d", hour, min, sec);
}

void getIR(void){
    readIR();
    if(IR_signal==1){
        // load Timer3 with 50 ms delay
        T3CON = 0b10110000;
        TMR3H = 0b00111100;
        TMR3L = 0b10110000;
        TMR3ON = 1;
    }
    while(IR_signal && !TMR3CF){
        // Check IR signal for the duration Timer3 was set, as a maximum
        readIR();
    }
    // If IR beam was broken for longer than Timer3 elapsed
    TMR3CF = 0;
    if(TMR3IF==1){
        f_loadingNewCan = 1;
    }
    else{
        f_loadingNewCan = 0;
    }
}

void updateServoPosition(int time_us, int timer){
    unsigned int my_time = 65535 - time_us;
    switch(timer){
        case 1:
            servoTimes[0] = my_time >> 8;
            servoTimes[1] = my_time & 0xFF;
        case 3:
            servoTimes[2] = my_time >> 8;
            servoTimes[3] = my_time & 0xFF;
    }
}