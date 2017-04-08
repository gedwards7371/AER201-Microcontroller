// Initializers
void initGlobalVars(void);
void initSortTimer(void);
void initServos(void);

// Primary routines
void sort(void);
void Loading(void);
void ID(void);
void Distribution(void);

// LOADING STAGE FLAGS
extern int first;//flag for first entry
extern int f_loadingNewCan; // Flag for a new can coming out of trommel
extern int f_lastCan; // Flag for 12th can
extern int f_ID_receive; // flag if ID stage is ready to receive
extern int f_can_coming_to_ID; //flag to tell ID stage that a can is coming (response flag to the one above))

// ID STAGE FLAGS
extern int f_can_coming_to_distribution;

// DISTRIBUTION STAGE FLAGS
extern int f_can_distributed; // flag to say a can has been put in its appropriate bin
extern int f_most_recent_sort_time; // flag to update comparison time in printSortTimer

// Time tracking
void printSortTimer(void);
extern int startTime[7];
extern int total_time;
extern int most_recent_sort_time;

// DC motor control
void delay_ms(unsigned char milliseconds);

// Sensor inputs
void getIR(void);
void getMAG(void);
extern int IR_signal;
extern int MAG_signal;

// Servo control
void updateServoPosition(int time_us, int timer); // pan & tilt
void updateServoStates(void);
void updateArmState(void);
extern unsigned int servoTimes[4]; // [0]=TMR1H, [1]=TMR1L, [2]=TMR3H, [3]=TMR3L
volatile extern int was_low_1; // flag to indicate whether pan servo is refreshing (20ms low) or not refreshing (sending high signal)
volatile extern int was_low_2; // same as above
volatile extern int was_low_3; // same as above
volatile extern int servo_timer_counter;
volatile extern int servo_timer_target;
volatile extern int pan_servo_state;
volatile extern int tilt_servo_state;
volatile extern int f_panning_to_bin;
volatile extern int timer2_counter;
volatile extern int f_arm_position; // controls the time of the high signal to the arm. 0 for "out", 1 for "in".

// Can type trackers
extern int sensor_outputs[2];
extern int count_total;
extern int count_pop_no_tab;
extern int count_pop_w_tab;
extern int count_can_w_lab;
extern int count_can_no_lab;
extern int cur_can; // 0 = pop_no_tab, 1 = pop_w_tab, 2 = can_w_lab, 3 = can_no_lab