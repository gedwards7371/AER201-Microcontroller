#ifndef RTC_H
#define	RTC_H

unsigned char time[7]; //sec, min, hour, weekday, weekday(#), month, year
void initTime(unsigned char s, unsigned char m, unsigned char h,unsigned char w,
              unsigned char DD, unsigned char MM, unsigned char YY);
void loadRTC();
void getRTC(void);
void printRTC(void);
//int Timer0_1s_counter;

#endif	/* RTC_H */