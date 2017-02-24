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

// <editor-fold defaultstate="collapsed" desc="VARIABLE & FLAG DEFINITIONS">

// LOADING STAGE FLAGS
int f_loadingNewCan = 0; // Flag for a new can coming out of trommel
int f_lastCan = 0; // Flag for 12th can
int f_ID_receive = 1; // flag if ID stage is ready to receive
int f_can_coming_to_ID = 0; //flag to tell ID stage that a can is coming (response flag to the one above))

// ID STAGE FLAGS
int f_can_coming_to_distribution = 0;

// DISTRIBUTION STAGE FLAGS
int f_can_distributed = 0; // flag to say a can has been put in its appropriate bin
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
        first = 0;
        __lcd_clear();
        initSortTimer();
        
        //Write to RA5 for DC motors
        DC = 1;
        
        // Start sending pulses to servos
        //moveServoCup(Home);
        // moveServoBlock(Raise);
    }
    else{
        // If a can is not already waiting to go to the ID stage,
        // we want the code to be able to enter straight down
        // to check if the ID stage is ready, each time it loops
        if(!f_loadingNewCan){
            //getIR(); // updates f_loadingNewCan flag
            
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
        //characteristic delay based on time it takes can to be transported here
        
        // Read conductivity sensor circuit. Because the solenoids are pushed in
        // right now, this will tell us side of can conductivity
        int side_conductivity = 0;
        int magnetic = 0;
        int top_bottom_conductivity = 0;
        
        side_conductivity = PORTAbits.RA2; // note that this is a local variable because we don't want to use an old result ever
        if(!side_conductivity){
            magnetic = MAGNETISM_in();
            if(!magnetic){
                SOL_COND_SENSORS = 1; //activate solenoids for top/bottom conductivity sensors
                // characteristic delay of time it takes for solenoids move out
                top_bottom_conductivity = PORTAbits.RA2;
                SOL_COND_SENSORS = 0;
            }
        }
        
        // use the local variables that we just initialized from the sensors to determine the can type
        moveServoBlock(Lower);
        // characteristic delay
        f_can_coming_to_distribution = 1;
        moveServoBlock(Raise);
        f_can_coming_to_ID = 0; // clear ID flag to allow another can to come
    }
}
void Distribution(void){
    if(f_can_coming_to_distribution){
        // delay characteristic of the time it takes a can to roll to the distribution cup from the ID stage
        moveServoCup(canType);
        // delay characteristic of the time it takes a can to roll to the distribution cup from the ID stage
        moveServoCup(Home);
        f_can_coming_to_distribution = 0;
        f_can_distributed = 1;
    }
}

void initSortTimer(){
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
    // store the start time of the beam break in timeBroken
    getRTC();
    int timeBroken[7];
    for(int i = 0; i < 7; i++){
        timeBroken[i] = __bcd_to_num(time[i]);
    }      
    while(!IRIN){
        // "if beam is broken for > 500 ms..."
            // f_loadingNewCan = 1;
            // break;
    }
}
int MAGNETISM_in(void){
    // gets analog reading from magnetism sensor    
    return 0;
}

void moveServoBlock(enum blockPositions myPosition){
    // lower or raise the block
    T3CON = 0b10110000;
    switch(myPosition){
        case Raise:
            // 2 ms pulses
            timer3highbits = 0b11111000;
            timer3lowbits = 0b00110000;
            break;
        case Lower:
            // 1 ms pulses
            timer3highbits = 0b11111100;
            timer3lowbits = 0b00011000;
            break;
        default:
            break;
    }
    // Start TMR1
    TMR3H = timer3highbits;
    TMR3L = timer3lowbits;
    TMR3ON = 1;
    was_low = 0;
}
void moveServoCup(enum motorPositions myPosition){
    // Hit servo with pulses characteristic to each position
    T1CON = 0b10110000; 
    switch(myPosition){
        case Home:
            // 1.5 ms pulses
            timer1highbits = 0b11111010;
            timer1lowbits = 0b00100100;
            break;
        case popCanNoTab:
            // 1 ms pulses
            timer1highbits = 0b11111100;
            timer1lowbits = 0b00011000;
            break;
        case popCanWithTab:
            //1.33 ms pulses
            timer1highbits = 0b11111010;
            timer1lowbits = 0b11001011;
            break;
        case soupCanNoLabel:
            // 1.66 ms pulses
            timer1highbits = 0b11111001;
            timer1lowbits = 0b01111101;
            break;
        case soupCanWithLabel:
            // 2 ms pulses
            timer1highbits = 0b11111000;
            timer1lowbits = 0b00110000;
            break;
        default:
            break;
    }
    // Start TMR1
    TMR1H = timer1highbits;
    TMR1L = timer1lowbits;
    TMR1ON = 1;
    was_low = 0;
}