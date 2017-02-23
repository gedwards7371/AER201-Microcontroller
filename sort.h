
enum motorPositions { Home, popCanNoTab, popCanWithTab, soupCanNoLabel, soupCanWithLabel } canType;
enum blockPositions { Raise, Lower } block;

// Primary routines
void sort(void);
void Loading(void);
void ID(void);
void Distribution(void);

// Helper functions
void initSortTimer();
void printSortTimer(void);
void getIR(void);
int MAGNETISM_in(void);
void moveServoBlock(enum blockPositions block);
void moveServoCup(enum motorPositions canType);

int first=1;//flag for first entry
int sortTimerCounter;
int was_low; // flag to indicate whether servo is refreshing (20ms low) or not refreshing (sending high signal)
int startTime[7];
int total_time;

int timer1highbits;
int timer1lowbits;
int timer1_20ms_high = 0b10110001;
int timer1_20ms_low = 0b11100000;
int timer3highbits;
int timer3lowbits;

int count_total;
int count_pop_no_tab;
int count_pop_w_tab;
int count_can_no_lab;
int count_can_w_lab;