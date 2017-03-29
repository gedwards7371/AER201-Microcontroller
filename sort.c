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
int f_most_recent_sort_time;

// Count variables
int count_total;
int count_pop_no_tab;
int count_pop_w_tab;
int count_can_w_lab;
int count_can_no_lab;

// Time trackers
int startTime[7];
int total_time;
int most_recent_sort_time;

// Sensors
int IR_signal;
int MAG_signal;

// Servo control
unsigned int servoTimes[4];
volatile int was_low_1;
volatile int was_low_3;

// Can type trackers
int sensor_outputs[2]; // Create array to identify can type.[0] = magnetism, [1] = conductivity
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
        IR_EMITTER = 1;
        
        // Write to EEPROM that the sort did not complete. This will be changed
        // after a successful run, but until then we must assume the run will
        // not complete.
        sel = EEPROM_read(0);
        unsigned short addr = 1 + 11 * sel;
        unsigned char byte1 = 0b01111111;
        EEPROM_write(addr, byte1);
        
        //Write to RA5 for DC motors, ramp up
        DC = 0;
        
        for(int i=0; i<46; i++){
            DC = !DC;
            delay_ms(45-i);
        }
        DC = 1;
        
        // Start sending pulses to servos
        initServos();
    }
    else{
        // If a can is not already waiting to go to the ID stage, we want the
        // code to be able to enter straight down to check if the ID stage is ready
        if(!f_loadingNewCan){
            // Call getIR to update f_loadingNewCan flag
            getIR();
            // "If no new can is being loaded..."
            if(!f_loadingNewCan){
               return; // get out of sort function (i.e. restart it)
            }
            // "If a new can is being loaded"
            else{ 
                count_total++;
                if(count_total == MAX_CANS){
                    f_lastCan = 1;
                }
            }
        }
        // "If a new can has already been loaded but the IR stage was not ready
        // the last time we executed this chunk...and if ID stage is ready..."
        else if(f_ID_receive){
            f_loadingNewCan = 0; // clear the new can flag after it's gone to ID
            __delay_ms(TIME_OUT_OF_TROMMEL); 
            DC  =  0; // Stop trommel to minimize can-within-can chances and meet our
                     // design requirement of "simplicity" by not having unnecessary actions
                    
            // Read magnetism to distinguish pop cans and soup cans (start of IDing)
            getMAG(); // Get analog input from magnetism sensor. Sets MAG_signal
            sensor_outputs[0] = MAG_signal;
            
            if(sensor_outputs[0]){
                for(int i = 0; i<2500; i++){
                    SOL_PUSHER = 1; // activate solenoid pusher @ 9 V
                    __delay_us(75);
                    SOL_PUSHER = 0;
                    __delay_us(25);
                }
            }
            else{
                for(int i = 0; i<2500; i++){
                    SOL_PUSHER = 1; // activate solenoid pusher @ 6.96 V
                    __delay_us(58);
                    SOL_PUSHER = 0;
                    __delay_us(42);
                }
            }
            
            __delay_ms(200);
            // Check if can is stuck. If so, hit it again.
            readIR();
            if(IR_signal==1){
                __delay_ms(100);
                readIR();
                if(IR_signal==1){
                    if(sensor_outputs[0]){
                        for(int i = 0; i<2500; i++){
                            SOL_PUSHER = 1; // activate solenoid pusher @ 9 V
                            __delay_us(75);
                            SOL_PUSHER = 0;
                            __delay_us(25);
                            }
                    }
                    else{
                        for(int i = 0; i<2500; i++){
                            SOL_PUSHER = 1; // activate solenoid pusher @ 6.96 V
                            __delay_us(58);
                            SOL_PUSHER = 0;
                            __delay_us(42);
                        }
                    }
                }
                
                __delay_ms(200);
                // Check if can is still stuck. If so, hit it with all we've got!
                int i = 1;
                while(IR_signal == 1){
                    readIR();
                    if(IR_signal==1){
                        __delay_ms(200);
                        readIR();
                        if(IR_signal==1){
                            SOL_PUSHER = 1;
                            __delay_ms(250);
                            SOL_PUSHER =  0;
                        }
                    }
                    if(i % 10 == 0){
                        DC = 1;
                        __delay_ms(5000);
                        DC = 0;
                    }
                    i++;
                }
            }
            f_can_coming_to_ID = 1;
        }
    }
}
void ID(void){
    if(f_can_coming_to_ID){
        // Characteristic delay based on time it takes can to be transported here
        __delay_ms(TIME_LOADING_TO_ID);
        
        SOL_COND_SENSORS = 1; // Activate solenoids for top/bottom conductivity sensors
        __delay_ms(TIME_CONDUCTIVITY); // Characteristic delay for time it takes solenoids move out
        sensor_outputs[1] = COND_SENSORS;
        
        // Identify can type
        // cur_can:
        //  0 - pop can no tab
        //  1 - pop can with tab
        //  2 - soup can with label
        //  3 - soup can no label  
        if(!sensor_outputs[0]){
            if(!sensor_outputs[1]){
                    count_pop_no_tab++;
                    cur_can = 0;
            }
            else{
                count_pop_w_tab++;
                cur_can = 1;
            }
        }
        if(sensor_outputs[0]){
            if(!sensor_outputs[1]){  
                count_can_w_lab++;
                cur_can = 2;
            }
            else{
                count_can_no_lab++;
                cur_can = 3;
            }
        }
        SOL_COND_SENSORS = 0;
        __delay_ms(200);
        SOL_COND_SENSORS = 1;
        // Lower block
        /* PWM to make the CAM servo work with Twesh's circuit */
        for(int i=0;i<10000;i++)
        {
            SERVOCAM = 1;
            __delay_us(10);
            SERVOCAM = 0;
            __delay_us(90);
        }  
        SERVOCAM = 0;
        
        SOL_COND_SENSORS = 0; // Retract solenoids. Note that this needs to happen after the cam is down,
                              // because otherwise cans will sandwich the cam and it won't be able to
                              // retract.
        
        f_can_coming_to_distribution = 1;
        __delay_ms(TIME_ID_TO_DISTRIBUTION);
        SERVOCAM = 1; // Raise block
        
        // Turn on DC motors again to start mixing remaining cans
        if(!f_lastCan){
            for(int i=0; i<46; i++){
                DC = !DC;
                delay_ms(45-i);
            }
            DC = 1;
        }
        
        f_can_coming_to_ID = 0; // clear ID flag to allow another can to come
    }
}
void Distribution(void){
    if(f_can_coming_to_distribution){
        
        // Set pan servo position
        // cur_can:
        //  0 - pop can no tab
        //  1 - pop can with tab
        //  2 - soup can with label
        //  3 - soup can no label 
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
        updateServoPosition(TILT_REST, 3);
        __delay_ms(TIME_SERVO_MOTION); // Give servo time to move
        
        // Tilt to drop can into bin
        // cur_can:
        //  0 - pop can no tab
        //  1 - pop can with tab
        //  2 - soup can with label
        //  3 - soup can no label 
        switch(cur_can){
            case 0:
                updateServoPosition(POP_TILT_DROP, 3);
                break;
            case 1:
                updateServoPosition(POP_TILT_DROP, 3);
                break;
            case 2:
                updateServoPosition(SOUP_TILT_DROP, 3);
                break;
            case 3:
                updateServoPosition(SOUP_TILT_DROP, 3);
                break;
            default:
                break;
        }
        __delay_ms(TILT_DROP_DELAY); // Give servo time to move
        
        // Reset the distribution stage
        updateServoPosition(TILT_REST, 3);
        __delay_ms(750);
        updateServoPosition(PAN_MID, 1);
        updateServoPosition(TILT_REST, 3);
        
        f_can_coming_to_distribution = 0;
        f_can_distributed = 1;
        f_most_recent_sort_time = 1;
        if(f_lastCan == 1){
            machine_state = DoneSorting_state;
        }
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
    f_most_recent_sort_time = 0;
    
    // Count variables
    count_total = 0;
    count_pop_no_tab = 0;
    count_pop_w_tab = 0;
    count_can_w_lab = 0;
    count_can_no_lab = 0;
    
    // Time variables
    most_recent_sort_time  = 999;
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
        updateServoPosition(TILT_REST, 3);
        TMR1ON = 1;
        was_low_1 = 0;
        __delay_ms(5); // So that servo ISRs don't race
        TMR3ON = 1;
        was_low_3 = 0;
        
        SERVOCAM = 1; // Raise block between ID and distribution stages
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
    
    if(f_most_recent_sort_time){
        most_recent_sort_time = total_time;
        f_most_recent_sort_time = 0;
    }
     
    
    if((total_time - most_recent_sort_time == MAX_NO_CANS) | (total_time == MAX_SORT_TIME)){
        machine_state = DoneSorting_state;
        // STOP EXECUTION (switch to DoneSorting_state and make sure loop executing will see this)
    }
    
    int min = (timeDiff % 3600) / 60;
    int sec = (timeDiff % 3600) % 60;
    
    __lcd_home();
    printf("SORTING...");
    __lcd_newline();
    printf("TIME %d:%02d", min, sec);
}

void getIR(void){
    readIR();
    for(int i = 0; i < 150; i++){
        __delay_us(500);
        readIR();
        if(!IR_signal){
            break;
        }
    }
    if(IR_signal==1){
        f_loadingNewCan = 1;
    }
    else{
        f_loadingNewCan = 0;
    }
}
void getMAG(void){
    readMAG();
    
    if(MAG_signal==1){
        __delay_ms(500);
        readMAG();
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

void delay_ms(unsigned char milliseconds){
   while(milliseconds > 0)
   {
      milliseconds--;
       __delay_us(990);
   }
}