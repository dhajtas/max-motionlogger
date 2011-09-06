#ifndef RTC_H_
#define RTC_H_

#define RTC_UPDATE		0x40

typedef struct TIME_struct
{
	uint8_t h;
	uint8_t m;
	uint8_t s;
} TIME;

typedef struct DATE_struct
{
	uint8_t d;
	uint8_t m;
	uint8_t y;
} DATE;


extern DATE Date __attribute__ ((section (".noinit")));
extern TIME Time __attribute__ ((section (".noinit")));

extern DATE ADate[14] __attribute__ ((section (".noinit")));
extern TIME ATime[14] __attribute__ ((section (".noinit")));

void RTC_Init(void);

void RTC_DateTime(void);

uint8_t RTC_CmpTime(TIME *time1, TIME *time2);

uint8_t RTC_CmpDate(DATE *time1, DATE *time2);

#endif	//RTC_H_
