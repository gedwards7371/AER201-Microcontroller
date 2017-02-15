
#ifndef CONSTANTS_H
#define	CONSTANTS_H //Prevent multiple inclusion 

//IR sensor
#define IRIN        LATAbits.LATA0 // input to IR sensor
#define IROUT       LATAbits.LATA6 // output to IR sensor

//Magnetism sensor
#define MAGNETIC    LATAbits.LATA1 // magnetism sensor

//Conductivity sensor
#define CONDUCTIVE  LATAbits.LATA2 // conductivity sensors

//Solenoids (1 cm)
#define SOL_1       LATAbits.LATA3 // 1 cm solenoids

//Solenoid (0.5 cm))
#define SOL_05      LATAbits.LATA4 // 0.5 cm solenoid

//DC motors
#define DC          LATAbits.LATA5 // DC motors

//Servo motors
#define SERVOCAM    LATCbits.LATC0 // cam servo for blocker
#define SERVOPAN    LATCbits.LATC1 // pan servo (sides)
#define SERVOTILT   LATCbits.LATC3 // tilt servo (up/down)

//LCD Control Registers
#define RS          LATDbits.LATD2          
#define E           LATDbits.LATD3
#define	LCD_PORT    LATD   //On LATD[4,7] to be specific
#define LCD_DELAY   25

#endif	/* CONSTANTS_H */