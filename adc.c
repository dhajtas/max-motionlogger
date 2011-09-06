#include <inttypes.h>
#include <avr/io.h>
#include <util/delay.h>

#include "hw.h"
#include "adc.h"

/*---------------------------------------------------------------------------*/
/*		ADC interrup routine												 */
/*---------------------------------------------------------------------------*/

//#if ADC_INT_ENABLE == 1

//volatile int16_t ADC_Buffer[10];
//volatile uint16_t ADC_Index;


void ADC_Init_hw(void)
{
	ADC_DDR |= _BV(ADC_SW); 			// configure battery switch as output low
	ADC_PORT &= ~(_BV(ADC_SW)); 		
	DIDR0 = 0x3F;						//disable digital input buffers on the ADC inputs - reduce icc 	
	DIDR1 = 0x03;
}


#if ADC_INT_ENABLE == 0

uint16_t ADC_Capture (uint8_t channel)
{
	uint16_t i;
	
	ADMUX = 0xC0 + channel;				// selects single-ended conversion on channel (ADC0 ~ ADC7)
										// selects internal Vref
										// selects right adjust of ADC result

	ADCSRA |= _BV(ADSC); 			// start a conversion by writing a one to the ADSC bit (bit 6)
	while(!(ADCSRA & _BV(ADIF))); 	// wait for conversion to complete: ADIF = 1

	i = ADC;


	return(i);
}
#endif //adc_int_enable


uint16_t ADC_Check_Batt(void)
{
	uint16_t ref, batt, pom;

	PRR &= ~(_BV(PRADC));				// enable ADC in power reduction register
	ADCSRA = _BV(ADEN)|_BV(ADPS2)|_BV(ADPS1);		// enables ADC by setting bit 7 (ADEN) in the ADCSRA
	_delay_us(80);						// delay to stabilise internal reference

	ref = ADC_Capture(0x0E);					// measure ref

	ADC_PORT |= _BV(ADC_SW);			// switch on divider
	batt = ADC_Capture(ADC_CHANNEL);			// measure ref
	ADC_PORT &= ~(_BV(ADC_SW));		// disable divider	

	ADCSRA = 0;						// disable the ADC
	PRR |= _BV(PRADC);				// disable ADC in power reduction register
	
	pom = (1024/ref)*batt;
	
	if(pom < 645)
	{
		Status |= STAT_BATT_LOW;
	}
	
	return(pom);
}