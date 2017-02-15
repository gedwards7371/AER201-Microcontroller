#ifndef UI_H
#define	UI_H

void initUI(void);
void interrupt keypressed(void);
int UIenabled;
int cur_state;
int inputHandler(void);
void dispLogs(int);

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

