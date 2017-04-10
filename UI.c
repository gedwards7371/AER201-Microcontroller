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

/* This file contains all the UI functionality outside the main loop
 * To add a new state:
 1. Define the state in the STATE TABLE (give it a numerical code and description)
 2. Create a new state change handler
 3. Add a case for the new state in the switch statement in updateMenu
 */

// <editor-fold defaultstate="collapsed" desc="VARIABLE DEFINITIONS">
int up; int down; int enter; int back; // correspond to certain inputs to make logic more transparent
int cur_state;
int firstboot; // flag to tell input handler to handle events for the first boot
int logstate; // flag to tell input handler if the UI is in a log (for pause/back purposes)
int log; // index for log of interest
volatile char input;
// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="STATE TABLE">
/* Define a menu to be a collection of text displayed (excludes cursor)
 * The state codes below use the following convention:
 *      For 2-digit codes: (main navigation menu state)
 *          1st digit = 1 => 1st and 2nd options displayed
 *          1st digit = 2 => 2nd and 3rd options displayed
 *      For 4-digit codes: (logs menu)
 *          3rd digit = 1 => 1st and 2nd options displayed
 *          3rd digit = 2 => 2nd and 3rd options displayed
 *          ...
 *          So on.
 *      For all menu state codes, the last digit specifies the menu option that
 *      the cursor is on. Each cursor location requires a unique state. The
 *      alternative is to write a routine that erases and rewrites the cursor.
 * 
 * 0 - start menu
 * 11 - 1st menu: first and second menu options with cursor on 1st option
 * 12 - 1st menu: first and second menu options with cursor on 2nd option
 * 22 - 2nd menu: second and third menu options with cursor on 2nd option
 * 23 - 2nd menu: second and third menu options with cursor on 3rd option
 * 1011 - 1st logs menu, cursor on 1st option
 * 1012 - 1st logs menu, cursor on 2nd option
 * 1022 - 2nd logs menu, cursor on 2nd option
 * 1023 - 2nd logs menu, cursor on 3rd option
 * 1033 - 3rd logs menu, cursor on 3rd option
 * 1034 - 3rd logs menu, cursor on 4th option
 */

// </editor-fold>

// Initialization routine for UI
void initUI(void){ 
    ei(); // enable interrupts
    machine_state = UI_state;
    logstate = 0;
    
    if(RTCflag){
        loadRTC(); // Load RTC with the date and time from main
    }  
    
    change_state_to_menu_start(); // Initialization  state
    firstboot = 1;
}

// Main routine for UI
void UI(void){
    if(machine_state == DoneSorting_state){
        __lcd_clear();
        __lcd_home();
        printf("RESULTS IN LOG %d", sel+1);
        __lcd_newline();
        printf("TIME: %d:%02d", (total_time % 3600) / 60, (total_time % 3600) % 60);
        while(PORTBbits.RB1 == 1) {continue;}
        while(PORTBbits.RB1 == 0){
            // Wait for user to press a key
        }
        if(cur_state == 12){
            change_state_to_menu_12();
        }
        else{
            change_state_to_menu_22();
        }
        while(PORTBbits.RB1 == 1){ continue; } // Wait until key is released
        __delay_ms(100);
        machine_state = UI_state; // Return to UI state
    }
    else if(cur_state == 0){
        __delay_1s();
        if(cur_state == 0){
            printRTC();
        }
    }
    while(logstate){
        // <editor-fold defaultstate="collapsed" desc="PRINT STATEMENTS FOR LOGS">
        if(!log_completion){
            __lcd_clear();
            __lcd_home();
            printf("-- LOG %2d HERE--", log);
            __lcd_newline();
            printf("DIDN'T COMPLETE");
            while(1) {__delay_1s();if(!logstate){break;}}
            break;
        }
        else{
            __lcd_clear();
            __lcd_home();
            printf("-- LOG %2d HERE--", log);
            __lcd_newline();
            printf("PAUSE < | BACK >");

            __delay_1s();if(!logstate){break;}
            __delay_1s();if(!logstate){break;}

            __lcd_clear();
            __lcd_home();
            printf("START:");
            __lcd_newline();
            char month[] = "MMM";
            switch(log_startTime[5]){
                case 1 :
                    strcpy(month, "JAN");
                    break;
                case 2:
                    strcpy(month, "FEB");
                    break;
                case 3:
                    strcpy(month, "MAR");
                    break;
                case 4:
                    strcpy(month, "APR");
                    break;
                case 5:
                    strcpy(month, "MAY");
                    break;
                case 6:
                    strcpy(month, "JUN");
                    break;    
                case 7:
                    strcpy(month, "JUL");
                    break;  
                case 8:
                    strcpy(month, "AUG");
                    break;    
                case 9:
                    strcpy(month, "SEP");
                    break;    
                case 10:
                    strcpy(month, "OCT");
                    break;    
                case 11:
                    strcpy(month, "NOV");
                    break;    
                case 12:
                    strcpy(month, "DEC");
                    break;
                default:
                    strcpy(month, "ERR");
                    break;
            }
            printf("%02d%s | %02d:%02d:%02d", log_startTime[4],month,log_startTime[2],log_startTime[1],log_startTime[0]);

            __delay_1s();if(!logstate){break;}
            __delay_1s();if(!logstate){break;}

            __lcd_clear();
            __lcd_home();
            printf("DURATION:");
            __lcd_newline();
            printf("%d MIN %d SEC", (log_total_time % 3600) / 60, (log_total_time % 3600) % 60);

            __delay_1s();if(!logstate){break;}
            __delay_1s();if(!logstate){break;}

            __lcd_clear();
            __lcd_home();
            printf(" -- POP  CAN -- ");
            __lcd_newline();
            printf("NTAB: %d|TAB: %d", log_count_pop_no_tab, log_count_pop_w_tab);

            __delay_1s();if(!logstate){break;}
            __delay_1s();if(!logstate){break;}

            __lcd_clear();
            __lcd_home();
            printf(" -- SOUP CAN -- ");
            __lcd_newline();
            printf("NLAB: %d|LAB: %d", log_count_can_no_lab, log_count_can_w_lab);

            __delay_1s();if(!logstate){break;}
            __delay_1s();if(!logstate){break;}

            __lcd_clear();
            __lcd_home();
            printf("-- TOTAL CANS --");
            __lcd_newline();
            printf("%d", log_count_total);

            __delay_1s();if(!logstate){break;}
            __delay_1s();if(!logstate){break;}
        }
        // </editor-fold>
    }
}

// Menu update routine
void updateMenu(void){
    up = 0; down = 0; enter = 0; back = 0; // Reset these
    di(); // disable interrupts while writing to LCD 
    
    if (inputHandler()
            // directly calls menu print functions if a number is pressed on the keypad. Returns 0 in this case
            // Else, sets up navigation flags and returns 1 so this if block is entered
            ){
        switch(cur_state){
            case 0 :
                // Any key press will change state from state 0 to state 11
                change_state_to_menu_11();
                break;

            case 11 :
                if(up){
                    change_state_to_menu_23();
                }
                else if(down){
                    change_state_to_menu_12();
                }
                else if(enter){
                    change_state_to_menu_start();
                }
                break;

            case 12 :
                if(up){
                    change_state_to_menu_11();
                }
                else if(down){
                    change_state_to_menu_23();
                }
                else if(enter){
                    machine_state = Sorting_state;
                }
                break;

            case 22 :
                if(up){
                    change_state_to_menu_11();
                }
                else if(down){
                    change_state_to_menu_23();
                }
                else if(enter){
                     machine_state = Sorting_state;
                }
                break;
                
            case 23 :
                if(up){
                    change_state_to_menu_22();
                }
                else if(down){
                    change_state_to_menu_11();
                }
                else if(enter){
                    change_state_to_logs_11();
                }
                break;
                
            case 1011:
                if(up){
                    change_state_to_logs_34();
                }
                else if(down){
                    change_state_to_logs_12();
                }
                else if(enter){
                    dispLogs(1);
                }
                else if(back){
                    change_state_to_menu_23();
                }
                break;
                
            case 1012:
                if(up){
                    change_state_to_logs_11();
                }
                else if(down){
                    change_state_to_logs_23();
                }
                else if(enter){
                    dispLogs(2);
                }
                else if(back){
                    change_state_to_menu_23();
                }
                break;
                
            case 1022 :
                if(up){
                    change_state_to_logs_11();
                }
                else if(down){
                    change_state_to_logs_23();
                }
                else if(enter){
                    dispLogs(2);
                }
                else if(back){
                    change_state_to_menu_23();
                }
                break;
                
            case 1023 :
                if(up){
                    change_state_to_logs_22();
                }
                else if(down){
                    change_state_to_logs_34();
                }
                else if(enter){
                    dispLogs(3);
                }
                else if(back){
                    change_state_to_menu_23();
                }
                break;
                
            case 1033 :
                if(up){
                    change_state_to_logs_22();
                }
                else if(down){
                    change_state_to_logs_34();
                }
                else if(enter){
                    dispLogs(3);
                }
                else if(back){
                    change_state_to_menu_23();
                }
                break;
                
            case 1034 :
                if(up){
                    change_state_to_logs_33();
                }
                else if(down){
                    change_state_to_logs_11();
                }
                else if(enter){
                    dispLogs(4);
                }
                else if(back){
                    change_state_to_menu_23();
                }
                break;

            default:
                break;
        }
    }
    ei(); // enable keypad interrupts after write
}

int inputHandler(void){
    // Uses current inputs to set next state where possible
    // and the navigation request otherwise
    // return 0 tells updateMenu that things have already been taken care of
    // return 1 tells updateMenu that navigation (up/down/enter/back) remains to be taken care of
    if(firstboot){
        change_state_to_menu_11();
        firstboot = 0;
        return 0;
    }
    if (logstate){
        if(input == 'C' && PORTBbits.RB1 == 1){
            // Pause
            while(PORTBbits.RB1 == 1){
                __delay_ms(10);
            }
            return 0;
        }
        else if (input == 'D'){
            // Back
            logstate = 0;
            switch(cur_state){
                case 1011 :
                    change_state_to_logs_11();
                    break;
                case 1012 :
                    change_state_to_logs_12();
                    break;
                case 1022 :
                    change_state_to_logs_22();
                    break;
                case 1023 :
                    change_state_to_logs_23();
                    break;
                case 1033:
                    change_state_to_logs_33();
                    break;
                case 1034 :
                    change_state_to_logs_34();
                    break;
            }
            return 0;
        }
        else{
            return 0;
        }
    }
    switch(input){
        // Tackles cases where next state can be set right away
        case '1' :
            if(cur_state == 11){
                return 0;
            }
            else{ 
            change_state_to_menu_11();
            }
            return 0;
        case '2' :
            if(cur_state == 22 || cur_state == 12){
                return 0;
            }
            else if(cur_state == 23){
                change_state_to_menu_22();
            }
            else{
                change_state_to_menu_12();
            }
            return 0;
        case '3' :
            if(cur_state == 23){
                return 0;
            }
            else{
                change_state_to_menu_23();
            }
            return 0;
        //Tackle navigation cases
        case 'A' :
            up = 1;
            return 1;
        case 'B' :
            down = 1;
            return 1;
        case 'C' :
            enter = 1;
            return 1;
        case 'D' :
            back = 1;
            return 1;
        default :
            // On any other key press, do nothing
            return 1;
    }
}

void dispLogs(int myLog){
    logstate = 1;
    log = myLog;
    retrieveLog(myLog - 1); // minus 1 because the logs are zero-indexed in memory
}

/* STATE CHANGE HANDLERS */
void change_state_to_menu_start(void){
    cur_state = 0;
    // Start menu
    __lcd_home();
    printRTC();
    __lcd_newline();
    printf("PUSH TO CONTINUE");
}
void change_state_to_menu_11(void){
    cur_state = 11;
    // 1st menu: first and second menu options with cursor on 1st option
    __lcd_home();
    printf("1. DATE/TIME   <");
    __lcd_newline();
    printf("2. SORT         ");
}
void change_state_to_menu_12(void){
    cur_state = 12;
    // 1st menu: first and second menu options with cursor on 2nd option
    __lcd_home();
    printf("1. DATE/TIME    ");
    __lcd_newline();
    printf("2. SORT        <");
}
void change_state_to_menu_22(void){
    cur_state = 22;
    // 2nd menu: second and third menu options with cursor on 2nd option
    __lcd_home();
    printf("2. SORT        <");
    __lcd_newline();
    printf("3. LOGS         ");
}
void change_state_to_menu_23(void){
    cur_state = 23;
    // 2nd menu: second and third menu options with cursor on 3rd option
    __lcd_home();
    printf("2. SORT         ");
    __lcd_newline();
    printf("3. LOGS        <");
}
void change_state_to_logs_11(void){
    cur_state = 1011;
    __lcd_home();
    printf("LOG 1          <");
    __lcd_newline();
    printf("LOG 2           ");
}
void change_state_to_logs_12(void){
    cur_state = 1012;
    __lcd_home();
    printf("LOG 1           ");
    __lcd_newline();
    printf("LOG 2          <");
}
void change_state_to_logs_22(void){
    cur_state = 1022;
    __lcd_home();
    printf("LOG 2          <");
    __lcd_newline();
    printf("LOG 3           ");
}
void change_state_to_logs_23(void){
    cur_state = 1023;
    __lcd_home();
    printf("LOG 2           ");
    __lcd_newline();
    printf("LOG 3          <");
}
void change_state_to_logs_33(void){
    cur_state = 1033;
    __lcd_home();
    printf("LOG 3          <");
    __lcd_newline();
    printf("LOG 4           ");
}
void change_state_to_logs_34(void){
    cur_state = 1034;
    __lcd_home();
    printf("LOG 3           ");
    __lcd_newline();
    printf("LOG 4          <");
}