void initEEPROM(void);
void EEPROM_write(unsigned short address, unsigned char data);
unsigned char EEPROM_read(unsigned short address);
void logRun(void);
void retrieveLog(int logNum);

// Log space selector bits
extern unsigned char sel;

// EEPROM global vars to be displayed on the logs in the UI
// The clusters of variables indicate which are stored/read in the same byte
extern int log_completion;
extern int log_count_total;

extern int log_count_pop_no_tab;
extern int log_count_pop_w_tab;

extern int log_count_can_w_lab;
extern int log_count_can_no_lab;

extern int log_startTime[7];

extern int log_total_time;