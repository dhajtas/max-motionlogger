//	RTC routines bazed on hw.h //

#include <avr/interrupt.h>
//test
#include <avr/io.h>
#include "spi.h"
//endtest
#include <inttypes.h>
#include <stdio.h>

#include "hw.h"
#include "rtc.h"

DATE ADate[14] __attribute__ ((section (".noinit")));
DATE Date __attribute__ ((section (".noinit")));
TIME ATime[14] __attribute__ ((section (".noinit")));
TIME Time __attribute__ ((section (".noinit")));

//----------------------------------------------------------------------
//-------------------- Routines ----------------------------------------
//----------------------------------------------------------------------

ISR(RTC_INT)
{
	Status |= STAT_RTC_UPDATE;
	RTC_DateTime();
//#if BIGAVR == 1
//	PORTB |= 0x04;				//test LED
//#endif
	sei();
}


void RTC_DateTime(void)
{
	Time.s++;
	if(Time.s == 60)
	{
		Time.s = 0;
		Time.m++;
		if(Time.m==60)
		{
			Time.m = 0;
			Time.h++;
			if(Time.h==24)
			{
				Time.h = 0;
				Date.d++;
				switch(Date.m)
				{
					case	2:	if(Date.d>28)
								{
									if(Date.y%4)
									{
										Date.d = 1;
										Date.m++;
									}
									else if(Date.d==30)
									{
										Date.d = 1;
										Date.m++;
									}
									
								}
								break;
					case	4:
					case	6:
					case	9:
					case	11:	if(Date.d==31)
								{
									Date.d = 1;
									Date.m++;
								}
								break;
					default	:	if(Date.d==32)
								{
									Date.d = 1;
									Date.m++;
								}
				}
				if(Date.m==13)
				{
					Date.m = 1;
					Date.y++;
				}
			}
		}
	}
}


void RTC_Init(void)
{
	PRR &= ~(_BV(PRTIM2));
 	RTC_TIMSK &= ~(_BV(RTC_TOIE)|_BV(RTC_OCIE));	//zakaz prerusenie od TC
											//pgm_read_byte_near(TRLUT+TR-0x01);	//vycitaj z prgm mem spravnu konstantu - PRG_RDB
 	ASSR = RTC_ASSR_VAL;					//set-up the asynchron. operation

//#if BIGAVR == 1
//	DDRD |= 0x80;							//Test LED
//	RTC_TCCR = 0x0F;						//start the timer
//#else
	RTC_TCCR = _BV(WGM21);					// CTC mode
	RTC_TCCRB = _BV(CS20)|_BV(CS21)|_BV(CS22); 	//start the timer
//#endif		//BIGAVR

 	RTC_OCR = 31;							//vloz konstantu do OCR - 1s time tick
 	RTC_TCNT = 0x00;						//Vynuluj count register
 	while(ASSR & 0x1F);
//	{
//test
//		SPI_Send8(ASSR,0);					//pockaj kym nepride k spravnemu nastaveniu
//	}
//endtest
	RTC_TIFR = 0xFF;						//zrus pripadne interrupty
	RTC_TIMSK = _BV(RTC_OCIE);				//povol prerusenie od OCIE.
//	sei();
}


uint8_t RTC_CmpTime(TIME *time1, TIME *time2)
{
	if((time1->h == time2->h) && (time1->m == time2->m))
	{
		if(time1->s == time2->s)
		{
			return(1);
		}
		else
		{
			if(time1->s > time2->s)
			{
				return(2);
			}
			else
			{
				return(0);
			}
		}
	}
	else
		return(0);
}

uint8_t RTC_CmpDate(DATE *time1, DATE *time2)
{
	if((time1->y == time2->y) && (time1->m == time2->m) && (time1->d == time2->d))
	{
		return(1);
	}
	else
		return(0);
}
