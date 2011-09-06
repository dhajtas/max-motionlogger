#ifndef __ADC_H__
#define __ADC_H__


//----------------------------------------------------------
//						DEFS
//----------------------------------------------------------

#ifndef GLUE
	#define GLUE(a, b)     a##b
	#define PORT(x)        GLUE(PORT, x)
	#define PIN(x)         GLUE(PIN, x)
	#define DDR(x)         GLUE(DDR, x)
#endif	//GLUE

#define ADC_PORT	PORT(ADC_SWITCH_P)
#define ADC_DDR		DDR(ADC_SWITCH_P)



void ADC_Init_hw(void);

uint16_t ADC_Capture(uint8_t channel);

uint16_t ADC_Check_Batt(void);

#endif //#ifndef __ADC_H__