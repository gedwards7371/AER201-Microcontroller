
#ifndef CONSTANTS_H
#define	CONSTANTS_H //Prevent multiple inclusion 

//Conductivity sensors
#define COND_SENSORS  PORTAbits.RA2 // conductivity sensors

//IR emitter
// #define IR_EMITTER LATAbits.LATA6
#define IR_EMITTER LATCbits.LATC5

//Solenoids (1 cm)
#define SOL_COND_SENSORS       LATAbits.LATA3 // solenoids for moving conductivity sensors
#define SOL_PUSHER      LATAbits.LATA4 // solenoid for pushing cans off trommel
#define SOL_OUT         1 // signal to move solenoids out

//DC motors
#define DC_RAMP     45
#define DC          LATAbits.LATA5 // DC motors

//Servo motors, times in microseconds
#define SERVOCAM    LATCbits.LATC0 // cam servo for blocker
#define SERVOPAN    LATCbits.LATC1 // pan servo (sides)
#define SERVOTILT   LATCbits.LATC2 // tilt servo (up/down)
#define PAN_R       500  
#define PAN_RMID    850
#define PAN_MID     1200 // 1500
#define PAN_LMID    1670
#define PAN_L       2200
#define TILT_REST   1150
#define POP_TILT_DROP  1260
#define SOUP_TILT_DROP 1300

// Characteristic travel times in milliseconds
#define TIME_OUT_OF_TROMMEL     1500
#define TIME_LOADING_TO_ID      1500
#define TIME_ID_TO_DISTRIBUTION 1500
#define TIME_SOLENOID_MOTION    300
#define TIME_SERVO_MOTION       750
#define TILT_DROP_DELAY         2500
#define TIME_CONDUCTIVITY       600

// Maximum operation allowances before forced termination
#define MAX_SORT_TIME           179
#define MAX_CANS                12

// IR sensor thresholds
#define THIR    1000
#define THMAG   150

//LCD Control Registers
#define RS          LATDbits.LATD2          
#define E           LATDbits.LATD3
#define	LCD_PORT    LATD   //On LATD[4,7] to be specific
#define LCD_DELAY   25

#endif	/* CONSTANTS_H */