#ifndef UI_H
#define	UI_H

void initUI(void);
void UI(void);
void interrupt handler(void);
void updateMenu(void);
extern int cur_state;
extern int log;
int inputHandler(void);
void dispLogs(int);

volatile extern char input;
const char keys[] = "123A456B789C*0#D"; // possible inputs from keypad

/* MENU STATE CHANGE FUNCTIONS */
void change_state_to_menu_start(void);
void change_state_to_menu_11(void);
void change_state_to_menu_12(void);
void change_state_to_menu_22(void);
void change_state_to_menu_23(void);
void change_state_to_logs_11(void);
void change_state_to_logs_12(void);
void change_state_to_logs_22(void);
void change_state_to_logs_23(void);
void change_state_to_logs_33(void);
void change_state_to_logs_34(void);
/* MENU PRINT FUNCTIONS */
void print_menu_start(void);
void print_menu_11(void);
void print_menu_12(void);
void print_menu_22(void);
void print_menu_23(void);
void print_menu_logs_11(void);
void print_menu_logs_12(void);
void print_menu_logs_22(void);
void print_menu_logs_23(void);
void print_menu_logs_33(void);
void print_menu_logs_34(void);

#endif	/* UI_H */