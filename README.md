# AER201-Microcontroller

Program for an autonomous can sorting machine. Microcontroller used: PIC18F4620. Noteworthy files will be described below

# .c files
### main.c
- Calls initialization functions and runs the main operation loop

### sort.c
- The can-sorting algorithm and actuator/sensor helper functions

### lcd.c
- Functions to interface with the LCD (send characters and machine instructions)

### UI.c
- Switch statements and helper functions to control the scrolling UI

### RTC.c
- Sets a desired time into the real-time clock (RTC) and contains functions to fetch and print current time

### MachineConfig.c
- Configures PIC ports, interrupts, timers, and clock speed

### I2C.c
- Communication over I2C ports to talk with RTC

### ADCFunctionality.c
- Analog-to-digital conversion functions

### interruptHandler.c
- Handles timer and keypress interrupts

### EEPROM.c
- Handles storage and retrieval of permenant data

### Test.c
- Contains debugging code that can be accessed from the main UI by pressing * on the keypad. This debugging code can read and display all sensor inputs, and drive each actuator in several convenient ways


# .h files (noteworthy ones)
### macros.h
- Defines macros for programming convenience

### configBits.h
- Setting of PIC configuration bits

### constants.h
- Defines frequently used constants
