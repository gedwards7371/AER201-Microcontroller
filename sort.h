
enum motorPositions { Home, popCanNoTab, popCanWithTab, soupCanNoLabel, soupCanWithLabel } canType;
enum blockPositions { Raise, Lower } block;

// Primary routines
void sort(void);
void Loading(void);
void ID(void);
void Distribution(void);

// Helper functions
void initFlags(void);
void initSortTimer(void);
void printSortTimer(void);

void getIR(void);
int IR_signal;
int TMR3counter=0;
int TMR3CF=0;
int MAG_signal;

void moveServoBlock(enum blockPositions block);
void moveServoCup(enum motorPositions canType);

int first=1;//flag for first entry
int was_low; // flag to indicate whether servo is refreshing (20ms low) or not refreshing (sending high signal)
int startTime[7];
int total_time;

void set_timer1(int time_us);
int ServoTimes[2];
int timer3highbits;
int timer3lowbits;

int count_total;
int count_pop_no_tab;
int count_pop_w_tab;
int count_can_no_lab;
int count_can_w_lab;

// LOADING STAGE FLAGS
int f_loadingNewCan = 0; // Flag for a new can coming out of trommel
int f_lastCan = 0; // Flag for 12th can
int f_ID_receive = 1; // flag if ID stage is ready to receive
int f_can_coming_to_ID = 0; //flag to tell ID stage that a can is coming (response flag to the one above))

// ID STAGE FLAGS
int f_can_coming_to_distribution = 0;

// DISTRIBUTION STAGE FLAGS
int f_can_distributed = 0; // flag to say a can has been put in its appropriate bin