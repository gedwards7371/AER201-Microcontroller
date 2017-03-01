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

// <editor-fold defaultstate="collapsed" desc="VARIABLE DECLARATIONS">
int first;

// Flags
int f_loadingNewCan;
int f_lastCan;
int f_ID_receive;
int f_can_coming_to_ID;
int f_can_coming_to_distribution;
int f_can_distributed;

// Count variables
int count_total;
int count_pop_no_tab;
int count_pop_w_tab;
int count_can_w_lab;
int count_can_no_lab;

// Time trackers
int startTime[7];
int total_time;

// Sensors
int IR_signal;
int MAG_signal;

// Servo control
unsigned int servoTimes[4];
volatile int was_low_1;
volatile int was_low_3;

// Can type trackers
int count_total;
int count_pop_no_tab;
int count_pop_w_tab;
int count_can_w_lab;
int count_can_no_lab;
int cur_can;
// </editor-fold>

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
        initGlobalVars();
        __lcd_clear();
        initSortTimer();
        
        // Write to EEPROM that the sort did not complete. This will be changed
        // after a successful run, but until then we must assume the run will
        // not complete.
        sel = EEPROM_read(0);
        unsigned short addr = 1 + 11 * sel;
        unsigned char byte1 = 0b01111111;
        EEPROM_write(addr, byte1);
        
        //Write to RA5 for DC motors
        DC = 1;
        
        // Start sending pulses to servos
        initServos();       
    }
    else{
        // If a can is not already waiting to go to the ID stage, we want the
        // code to be able to enter straight down to check if the ID stage is ready
        if(!f_loadingNewCan){
            // Call getIR to update f_loadingNewCan flag
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
            __delay_ms(TIME_SOLENOID_MOTION); 
            SOL_PUSHER = 0; // deactivate solenoid pusher (could do this in ID since we have a delay there anyway)
        }
    }
}
void ID(void){
    if(f_can_coming_to_ID){
        // Characteristic delay based on time it takes can to be transported here
        __delay_ms(TIME_LOADING_TO_ID);
        
        // Create array to identify can type.
        // [0] = side conductivity, [1] = magnetism, [2] = top/bottom conductivity
        // Note that this is a local variable because we don't want to use an old result, ever!
        int sensor_outputs[3];
        
        // Read conductivity sensor circuit. Because the solenoids are pushed in
        // right now, this will tell us side of can conductivity
        sensor_outputs[0] = COND_SENSORS; 
        
        if(!sensor_outputs[0]){
            readMAG(); // Get analog input from magnetism sensor. Sets MAG_signal
            sensor_outputs[1] = MAG_signal;
            if(!sensor_outputs[1]){
                SOL_COND_SENSORS = 1; //activate solenoids for top/bottom conductivity sensors
                // characteristic delay for time it takes solenoids move out
                __delay_ms(TIME_SOLENOID_MOTION); // IDK
                sensor_outputs[2] = COND_SENSORS;
                SOL_COND_SENSORS = 0;
            }
        }
        
        // Identify can type
        if(!sensor_outputs[0]){
            if(!sensor_outputs[1]){
                if(!sensor_outputs[2]){
                    count_pop_no_tab++;
                    cur_can = 0;
                }
                else{
                    count_pop_w_tab++;
                    cur_can = 1;
                }
            }
            else{
                count_can_w_lab++;
                cur_can = 2;
            }
        }
        else{
            count_can_no_lab++;
            cur_can = 3;
        }
        
        SERVOCAM = 1; // Lower block
        f_can_coming_to_distribution = 1;
        __delay_ms(TIME_ID_TO_DISTRIBUTION);
        SERVOCAM = 0; // Raise block
        
        f_can_coming_to_ID = 0; // clear ID flag to allow another can to come
    }
}
void Distribution(void){
    if(f_can_coming_to_distribution){
        
        // Set pan servo position
        switch(cur_can){
            case 0:
                updateServoPosition(PAN_R, 1);
                break;
            case 1:
                updateServoPosition(PAN_RMID, 1);
                break;
            case 2:
                updateServoPosition(PAN_LMID, 1);
                break;
            case 3:
                updateServoPosition(PAN_L, 1);
                break;
            default:
                break;
        }
        __delay_ms(TIME_SERVO_MOTION); // Give servo time to move
        
        // Tilt to drop can into bin
        updateServoPosition(TILT_DOWN, 3);
        __delay_ms(TIME_SERVO_MOTION); // Give servo time to move
        
        // Reset the distribution stage
        updateServoPosition(PAN_MID, 1);
        updateServoPosition(TILT_UP, 3);
        f_can_coming_to_distribution = 0;
        f_can_distributed = 1;
    }
}

void initGlobalVars(void){
    // Clear flag for first entry
    first = 0;
    
    // Flags
    f_loadingNewCan = 0;
    f_lastCan = 0;
    f_ID_receive = 1;
    f_can_coming_to_ID = 0;
    f_can_coming_to_distribution = 0;
    f_can_distributed = 0;
    
    // Count variables
    count_total = 0;
    count_pop_no_tab = 0;
    count_pop_w_tab = 0;
    count_can_w_lab = 0;
    count_can_no_lab = 0;
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
        updateServoPosition(PAN_L, 1);
        updateServoPosition(TILT_UP, 3);
        TMR1ON = 1;
        __delay_ms(5); // So that servo ISRs don't race
        TMR3ON = 1;
        SERVOCAM = 0; // Raise block between ID and distribution stages
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
    printf("SORTING...");
    __lcd_newline();
    printf("TIME %d:%02d", min, sec);
}

void getIR(void){
    readIR();
    if(IR_signal==1){
        __delay_ms(100);
        if(IR_signal==1){
            f_loadingNewCan = 1;
        }
        else{
            f_loadingNewCan = 0;
        }
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