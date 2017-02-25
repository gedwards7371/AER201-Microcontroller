
void readADC(char channel);
void readIR(void);
void readMAG(void);

int THIR = 255; // UNTESTED
int THMAG = 255; // drops from 1023 to 137, so 225 to be safe