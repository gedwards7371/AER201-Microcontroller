
#ifndef CONSTANTS_H
#define	CONSTANTS_H //Prevent multiple inclusion 

//Conductivity sensor
#define CONDUCTIVE  LATAbits.LATA2 // conductivity sensors

//Solenoids (1 cm)
#define SOL_COND_SENSORS       LATAbits.LATA3 // solenoids for moving conductivity sensors

//Solenoid (0.5 cm))
#define SOL_PUSHER      LATAbits.LATA4 // solenoid for pushing cans off trommel

//DC motors
#define DC          LATAbits.LATA5 // DC motors

//Servo motors
#define SERVOCAM    LATCbits.LATC0 // cam servo for blocker
#define SERVOPAN    LATCbits.LATC1 // pan servo (sides)
#define SERVOTILT   LATCbits.LATC2 // tilt servo (up/down)
#define PAN_R       1000
#define PAN_RMID    1333
#define PAN_MID     1500
#define PAN_LMID    1666
#define PAN_L       2000
#define TILT_UP     1000
#define TILT_DOWN   1500

//LCD Control Registers
#define RS          LATDbits.LATD2          
#define E           LATDbits.LATD3
#define	LCD_PORT    LATD   //On LATD[4,7] to be specific
#define LCD_DELAY   25

#endif	/* CONSTANTS_H */