
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
//define DC          LATAbits.LATA5 // DC motors
#define DC          LATBbits.LATB2 // DC motors

//Servo motors, times in microseconds. Each must be unique
#define SERVOCAM    LATCbits.LATC0 // cam servo for blocker
#define SERVOPAN    LATCbits.LATC1 // pan servo (sides)
#define SERVOTILT   LATCbits.LATC2 // tilt servo (up/down)
#define PAN_R       500  
#define PAN_RMID    850
#define PAN_MID     1200 // 1500
#define PAN_LMID    1670
#define PAN_L       2400 // 2200
#define TILT_REST   1050 // 1150
#define POP_TILT_DROP  1260
#define SOUP_TILT_DROP 1390
#define SERVOARM    LATBbits.LATB3

// Characteristic travel times in milliseconds unless otherwise noted
#define TIME_OUT_OF_TROMMEL     400
#define TIME_ARM_SWING_IN       750
#define TIME_LOADING_TO_ID      1000
#define TIME_ID_TO_DISTRIBUTION 750
#define TIME_SOLENOID_MOTION    300
#define TIME_SERVO_MOTION       750
#define TILT_DROP_DELAY         700 // 2500
#define TIME_CONDUCTIVITY       600
#define TIME_INTERMITTENT_DRUM_STOP 20 // Second

// Maximum operation allowances before forced termination
#define MAX_SORT_TIME           179
#define MAX_CANS                12
#define MAX_NO_CANS             42

// IR sensor thresholds
#define THIR    800
#define THMAG   150
#define THCOND  300

//LCD Control Registers
#define RS          LATDbits.LATD2          
#define E           LATDbits.LATD3
#define	LCD_PORT    LATD   //On LATD[4,7] to be specific
#define LCD_DELAY   25

#endif	/* CONSTANTS_H */