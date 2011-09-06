#include <avr/io.h>
#include <avr/signal.h>
#include <avr/interrupt.h>
#include <inttypes.h>

#include "driver_lcd.h"
#include "driver_KEYB.h"
#include "driver_DC.h"
#include "driver_FGEN.h"
#include "init.h"
#include "fgen.h"
//#include "myeeprom.h"
//#include "rsc0.h"



void Init(void)
{
	SFIOR = 0x00;	
	init_LCD_port();
	init_LCD();
//	init_KEYB_port();
	init_DC_port();
	init_FGEN_port();
	init_TIMER();			// system timer0, KEYB & LCD depends on;
	init_DC();				// operatio of DC: PWM setup, ADC setup, calibration? timer1
	init_FGEN();			// operation of FGENs: clk, data, PLL, DAC... timer3
//	init_RSC0(26);			// 9600 baud?
	init_IRQ();
}

//-----------------------------------------------------------


void init_TIMER(void)
{
//	TCCR0  = 0x06;		//normal operation; 256 prescaler, overflow
//	TCNT0  = 0x00;

	TIMSK  = 0x00; 		//toto obsluhuje RTos //enable TOV0
	ETIMSK = 0x00;		//disable all ....
//	TIFR   = 0xFF;		//vymazanie interruptov
//	ETIFR  = 0xFF;


}


//-----------------------------------------------------------

void init_IRQ(void)
{
//	tick=0;
	WDTCR = 0x18;
	WDTCR = 0x00;
 	MCUSR = 0x00;		//vynulovanie akehokolvek exter, poweron, wdt resetu.
 	MCUCR = 0x18;		//Power-save sleep mode, sleep not enabled
	EIMSK = 0x00;
}


//--------------------------------------------------------------

void SetInt(void)
{
	cli();
//	EIMSK = 0xD1;			//interrupt mask - 0,(1),4,6,7 - potom pripocitat 0x02
	sei();
}
