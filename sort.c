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
int time_recent_agitator;

// Sensors
int IR_signal;
int IR_res;
int MAG_signal;
int COND_signal;

// DC motor control
int motor_toggle_count;
int agitator_toggle_count;

// Servo control
unsigned int servoTimes[4];
volatile int was_low_1;
volatile int was_low_2;
volatile int was_low_3;
volatile int servo_timer_counter;
volatile int servo_timer_target;
volatile int pan_servo_state;
volatile int tilt_servo_state;
volatile int f_panning_to_bin;
volatile int timer2_counter;
volatile int f_arm_position;

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
        TMR1IE = 1;
        TMR2IE = 1;
        TMR3IE = 1;
        initGlobalVars();
        __lcd_clear();
        initSortTimer();
        IR_EMITTER = 1;
        IR_EMITTER_LABEL = 1;
        
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
            getIR(0);
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
            f_most_recent_sort_time = 1;
            f_loadingNewCan = 0; // clear the new can flag after it's gone to ID
            __delay_ms(TIME_OUT_OF_TROMMEL); 
            DC  =  0; // Stop trommel to minimize can-within-can chances and meet our
                     // design requirement of "simplicity" by not having unnecessary actions
                    
            // Read magnetism to distinguish pop cans and soup cans (start of IDing)
            f_arm_position = 1;
            
            delay_ms(TIME_ARM_SWING_IN); // so that arm doesn't interfere with solenoid
            getMAG(); // Get analog input from magnetism sensor. Sets MAG_signal
            sensor_outputs[0] = MAG_signal;
            
            TMR2IE = 0; // disable PWM to arm for now so that it doesn't spaz...
            if(sensor_outputs[0]){
                for(int i = 0; i<3000; i++){
                    SOL_PUSHER = 1; // activate solenoid pusher @ 9 V
                    __delay_us(75);
                    SOL_PUSHER = 0;
                    __delay_us(25);
                }
            }
            else{
                for(int i = 0; i<3000; i++){
                    SOL_PUSHER = 1; // activate solenoid pusher @ 6.96 V
                    __delay_us(58);
                    SOL_PUSHER = 0;
                    __delay_us(42);
                }
            }
            TMR2IE = 1;
            __delay_ms(500);  // 350
            // Check if can is stuck. If so, hit it again.
            readIR(0);
            if(IR_signal==1){
                __delay_ms(100);
                readIR(0);
                if(IR_signal==1){
                    getMAG();
                    sensor_outputs[0] = sensor_outputs[0] || MAG_signal;
                    
                    if(sensor_outputs[0]){
                        for(int i = 0; i<3000; i++){
                            SOL_PUSHER = 1; // activate solenoid pusher @ 9 V
                            __delay_us(75);
                            SOL_PUSHER = 0;
                            __delay_us(25);
                        }
                    }
                    else{
                        for(int i = 0; i<3000; i++){
                            SOL_PUSHER = 1; // activate solenoid pusher @ 6.96 V
                            __delay_us(58);
                            SOL_PUSHER = 0;
                            __delay_us(42);
                        }
                    }
                }
                
                __delay_ms(400); // 350
                // Check if can is still stuck. If so, hit it with all we've got!
                int j = 0;
                while(IR_signal == 1){
                    readIR(0);
                    if(j == 3 || j == 4){
                        f_arm_position = 0;
                    }
                    else if(j == 5 || j == 6){
                        DC = 1;
                    }
                    else if(j == 7 || j == 8 || j == 9 || j == 10){
                        f_arm_position = 1;
                        __delay_ms(200);
                    }
                    else if(j % 2 == 0){
                        DC = !DC;
                        f_arm_position = !f_arm_position;
                        __delay_ms(200);
                    }
                    
                    // Pushing code
                    if(IR_signal==1){
                        __delay_ms(350);
                        readIR(0);
                        if(IR_signal==1){
                            if(sensor_outputs[0]){
                                SOL_PUSHER = 1;
                                __delay_ms(300);
                                SOL_PUSHER = 0;
                            }
                            else{
                                TMR2IE = 0;
                                for(int i = 0; i<3000; i++){
                                    switch(j){
                                        case 0:
                                            SOL_PUSHER = 1;
                                            __delay_us(75);
                                            SOL_PUSHER = 0;
                                            __delay_us(25);
                                            break;
                                        case 1:
                                            SOL_PUSHER = 1;
                                            __delay_us(80);
                                            SOL_PUSHER = 0;
                                            __delay_us(20);
                                            break;
                                        case 2:
                                            SOL_PUSHER = 1;
                                            __delay_us(85);
                                            SOL_PUSHER = 0;
                                            __delay_us(15);
                                            break;
                                        case 3:
                                            SOL_PUSHER = 1;
                                            __delay_us(90);
                                            SOL_PUSHER = 0;
                                            __delay_us(10);
                                            break;
                                        case 4:
                                            SOL_PUSHER = 1;
                                            __delay_us(95);
                                            SOL_PUSHER = 0;
                                            __delay_us(50);
                                            break;
                                        default:
                                            SOL_PUSHER = 1;
                                            __delay_us(100);
                                            break;
                                    }  
                                }
                                TMR2IE = 1;
                            }
                        }
                        SOL_PUSHER = 0;
                        j++;
                    }
                    // Recognition of "almost got unstuck" case
                    if(!IR_signal){
                        __delay_ms(750); // 500
                        readIR(0);
                        if(IR_signal==1){
                            continue;
                        }
                        else{
                            break;
                        }
                    }
                }
                DC = 0;
            }
            f_can_coming_to_ID = 1;
        }
    }
}
void ID(void){
    if(f_can_coming_to_ID){
        // Characteristic delay based on time it takes can to be transported here
        __delay_ms(TIME_LOADING_TO_ID);
        
        f_arm_position = 0; // ensures that any cans in the trommel are held back until they can no longer interfere with the current can
        
        if(sensor_outputs[0]){
            // Read side of can reflectivity (calibrated for soup cans to detect label)
            readIR(3);
            sensor_outputs[1] = IR_signal;
            __delay_ms(10);
            readIR(3);
            sensor_outputs[1] = sensor_outputs[1] || IR_signal;
        }
        int reflectivity1 = IR_res;
        
        SOL_COND_SENSORS = 1; // Activate solenoids for top/bottom conductivity sensors
        
        // Sample conductivity reading and later take the average to determine can ID
        const int n = 10;
        // If a pop can...
        if(!sensor_outputs[0]){
            const unsigned char time = (TIME_CONDUCTIVITY_POP / n);
            int res1 = 0; 
            int res2 = 0;
            for(int i = 0; i<n; i++){
                delay_ms(time);
                readCOND();
                res1 += COND_signal;
            }
            SOL_COND_SENSORS = 0;
            __delay_ms(200);
            SOL_COND_SENSORS = 1;
            for(int i = 0; i<n; i++){
                delay_ms(time);
                readCOND();
                res2 += COND_signal;
            }
            SOL_COND_SENSORS = 0;
            res1 = ((res1 / n) > 0.3) ? 1 : 0;
            res2 = ((res2 / n) > 0.3) ? 1 : 0;

            // Average value
            sensor_outputs[1] = (res1 || res2);
        }
        else{
            const unsigned char time = (TIME_CONDUCTIVITY_SOUP / n);
            int res[NUM_SOUP_TESTS];
            
            for(int j = 0; j < NUM_SOUP_TESTS; j++){
                SOL_COND_SENSORS = 1;
                for(int i = 0; i < n; i++){
                    delay_ms(time);
                    readCOND();
                    res[j] += COND_signal;
                }
                SOL_COND_SENSORS = 0;
                
                __delay_ms(200); // For solenoid retraction
                
                // Logical OR of all samples
                res[j] = ((res[j] / n) > 0.3) ? 1 : 0;
                if(j==0){
                    sensor_outputs[1] = res[j];
                }
                else{
                    sensor_outputs[1] = sensor_outputs[1] || res[j];
                }
            }
        }
        int cond1 = sensor_outputs[1];
        
        
        if(sensor_outputs[0]){
            // Read side of can reflectivity (calibrated for soup cans to detect label)
            readIR(3);
            sensor_outputs[1] = sensor_outputs[1] || IR_signal;
            __delay_ms(10);
            readIR(3);
            sensor_outputs[1] = sensor_outputs[1] || IR_signal;
        }
        int reflectivity2 = IR_res;
        
        //__lcd_clear();__lcd_home();
        //printf("r1: %d|r2: %d", reflectivity1, reflectivity2);
        //__lcd_newline();
        // printf("cond: %d", cond1);
        
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
        
        while(!f_can_distributed){continue;}
        
        for(int i=0;i<10000;i++)
        {
            SERVOCAM = 1;
            __delay_us(10);
            SERVOCAM = 0;
            __delay_us(90);
        }

        SERVOCAM = 0;   
        
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
        f_can_distributed = 0;
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
        servo_timer_target = TIME_SERVO_MOTION;
        f_panning_to_bin = 1;
        f_can_coming_to_distribution = 0;
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
    f_can_distributed = 1;
    f_most_recent_sort_time = 0;
    
    // Count variables
    count_total = 0;
    count_pop_no_tab = 0;
    count_pop_w_tab = 0;
    count_can_w_lab = 0;
    count_can_no_lab = 0;
    
    // Time variables
    most_recent_sort_time  = 999;
    int time_recent_agitator = 0;
    
    // DC motor control
    motor_toggle_count = 0;
    agitator_toggle_count = 0;
    
    // Servo variables
    servo_timer_counter = 0;
    servo_timer_target = 9999;
    timer2_counter = 0;
    f_arm_position = 0;
    pan_servo_state = -1;
    tilt_servo_state = -1;
    f_panning_to_bin = 0;
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
        // Pan and tilt servo initialization to middle
        updateServoPosition(PAN_MID, 1);
        updateServoPosition(TILT_REST, 3);
        TMR1ON = 1;
        was_low_1 = 0;
        
        __delay_ms(5); // So that these servo ISRs don't race
        
        TMR3ON = 1;
        was_low_3 = 0;
                
        // Arm servo initialization to "out"
        __delay_ms(1); // To decrease chances of ISRs racing
        SERVOARM = 0;
        TMR2ON = 1;
        was_low_2 = 0;
        
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
    
    // Every 20 seconds, turns off drum for 2 seconds.
    if(total_time >= TIME_INTERMITTENT_DRUM_STOP){
        if(DC == 0){
            motor_toggle_count++;
            if(motor_toggle_count == 2){
                motor_toggle_count = 0;
                //for(int i=0; i<46; i++){
                //    DC = !DC;
                //    delay_ms(45-i);
                //}
                DC = 1;
            }
        }
        else if(total_time % TIME_INTERMITTENT_DRUM_STOP == 0){
            DC = !DC;
        }
    }
    
    // After 6 cans have been sorted, turn on agitator for 2 seconds every 30 seconds. Otherwise,
    // turn on agitator every 30 sec after 1 min
    if(total_time >= TIME_INTERMITTENT_AGITATOR){
        if(total_time % TIME_INTERMITTENT_AGITATOR == 0){
            AGITATOR = 1;
            __delay_ms(200);
            AGITATOR = 0;
        }
    }
    
    if(total_time - most_recent_sort_time == MAX_NO_CANS){
        if((count_pop_no_tab > 0) && (count_pop_w_tab > 0) && (count_can_w_lab > 0) && (count_can_no_lab > 0)){
            machine_state = DoneSorting_state;
             // STOP EXECUTION (switch to DoneSorting_state and make sure loop executing will see this)
        }
    }
    else if (total_time == MAX_SORT_TIME){
         machine_state = DoneSorting_state;
        // STOP EXECUTION (switch to DoneSorting_state and make sure loop executing will see this)
    }
    
    int min = (timeDiff % 3600) / 60;
    int sec = (timeDiff % 3600) % 60;
    
    __lcd_home();
    printf("SORTING...     ");
    __lcd_newline();
    printf("TIME %d:%02d   ", min, sec);
}

void getIR(int port){
    readIR(port);
    for(int i = 0; i < 150; i++){
        __delay_us(500);
        readIR(port);
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
    if(MAG_signal){
        __delay_ms(100);
        readMAG();
    }
}

void updateServoPosition(int time_us, int timer){
    unsigned int my_time = 65535 - time_us;
    switch(timer){
        case 1:
            servoTimes[0] = my_time >> 8;
            servoTimes[1] = my_time & 0xFF;
            
            pan_servo_state = time_us;
        case 3:
            servoTimes[2] = my_time >> 8;
            servoTimes[3] = my_time & 0xFF;
            
            tilt_servo_state = time_us;
    }
}
void updateServoStates(void){
    //...only change servo state after ensuring travel times have been met or exceeded
    if(servo_timer_counter >= servo_timer_target){
        // Takes care of current location, not how long duration @ location is
        if(f_panning_to_bin){
            switch(pan_servo_state){
                // Changes tilt state based on where the pan is
                case PAN_R:
                    updateServoPosition(POP_TILT_DROP, 3);
                    servo_timer_counter = 0;
                    servo_timer_target = TILT_DROP_DELAY;
                    break;
                case PAN_RMID:
                    updateServoPosition(POP_TILT_DROP, 3);
                    servo_timer_counter = 0;
              
                    servo_timer_target = TILT_DROP_DELAY;
                    break;
                case PAN_LMID:
                    updateServoPosition(SOUP_TILT_DROP, 3);
                    servo_timer_counter = 0;
                    servo_timer_target = TILT_DROP_DELAY;
                    break;
                case PAN_L:
                    updateServoPosition(SOUP_TILT_DROP, 3);
                    servo_timer_counter = 0;
                    servo_timer_target = TILT_DROP_DELAY;
                    break;
                default:
                    break;
            }
            f_panning_to_bin = 0;
        }
        else if(pan_servo_state == PAN_MID){
            // Don't do anything if waiting for a can...
            servo_timer_counter = 0;
        }
        else{
            switch(tilt_servo_state){
                // Reset the distribution stage
                case POP_TILT_DROP:
                    updateServoPosition(TILT_REST, 3);
                    servo_timer_counter = 0;
                    servo_timer_target = TIME_SERVO_MOTION;
                    break;
                case SOUP_TILT_DROP:
                    updateServoPosition(TILT_REST, 3);
                    servo_timer_counter = 0;
                    servo_timer_target = TIME_SERVO_MOTION;
                    break;

                case TILT_REST:
                    updateServoPosition(PAN_MID, 1);
                    updateServoPosition(TILT_REST, 3);
                    servo_timer_counter = 0;
                    servo_timer_target = 9999;

                    f_can_distributed = 1;
                    if(f_lastCan == 1){
                        machine_state = DoneSorting_state;
                    }
                    break;

                default:
                    break;
            }
        }
    }
}
void updateArmState(void){
    // PR2=0x11 -> 500 us
    // PR2=0xA0 -> 5000 us
    // PR2=0xFF -> 7968.75 us
    // 1000 us -> "out" position | 1000 us -> PR2 = 0x20
    // 2000 us -> "in" position | 2000 us -> PR2 = 0x40
    // 20000 us downtime is fine for both positions; we will instead try using 16000 us
    if(!was_low_2){
        // Here we set the timer conditions for the high signal
        switch(f_arm_position){
            case 0:
                // "out" position
                PR2 = 0x20;
                break;
            case 1:
                // "in" position
                PR2 = 0x40;
                break;
        }
    }
    else{
        // Here we set the timer conditions for the low signal
        PR2 = 0xFF; // ~8 ms
    }
}

void delay_ms(unsigned char milliseconds){
   while(milliseconds > 0)
   {
      milliseconds--;
       __delay_us(990);
   }
}