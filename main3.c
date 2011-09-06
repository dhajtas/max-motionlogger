
#include <stdio.h>
#include <inttypes.h>
#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include "hw.h"
#include "spi.h"
#include "sd_routines.h"

#include "rtc.h"


volatile uint8_t Status;


int main(void)
{
	uint8_t i, j, cardType, error;
	
	DDRB = 0xFF;
	PORTB = 0x00;
	SD_Init_hw();	// cs of sd disable!
	SPI_Init();		//only one SPI on BIGAVR... need to change for ATmega328
	cardType = 0;

//	error = SD_Init(2);
	_delay_us(10);
	RTC_Init();
	sei();
	
	while(1)
	{
		if(Status & RTC_UPDATE)
		{
			PORTB |= _BV(PB2);		//sd deassert ako test rtc interruptu
			sei();
			for(j=0; j<8; j++)
			{
				SPI_Send8(0xAA,0);
				_delay_us(10);
			}
			Status &= ~RTC_UPDATE;
		}
		SPI_Send8(0xF0,0);
	}
}