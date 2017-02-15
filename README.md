# AER201-Microcontroller

Program for an autonomous can sorting machine. Microcontroller used: PIC18F4620

#.c files
###main.c
- Calls initialization functions and runs the main loop

###sort.c
- The can-sorting algorithm and actuator/sensor helper functions

###lcd.c
- Functions to interface with the LCD (send characters and machine instructions)

###UI.c
- Switch statements and helper functions to control the scrolling UI

###RTC.c
- Sets a desired time into the real-time clock (RTC) and contains functions to fetch and print current time

###PortHigh.c
- Will set certain pins high on keypress (for rapid prototyping purposes)

###PWM.c
- Functions to configure and use built-in PWM (suitable for large frequencies)

###MachineConfig.c
- Configures PIC ports, interrupts, timers, and clock speed

###I2C.c
- Communication over I2C ports to talk with RTC

###ADCFunctionality.c
- Analog-to-digital conversion functions


#.h files
###macros.h
- Defines macros for programming convenience

###configBits.h
- Setting of PIC configuration bits

###constants.h
- Defines frequently used constants
