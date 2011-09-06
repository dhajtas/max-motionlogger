
#ifndef HW_H_
#define HW_H_

extern volatile uint8_t Status; 
extern volatile uint8_t Power;
 
#define STAT_LIS_DATA_RDY	0x80
#define STAT_RTC_UPDATE		0x40
#define STAT_WRITE_DATA		0x20
#define STAT_DATA_BANK		0x10
#define STAT_SD_PRESENT		0x08
#define STAT_BATT_LOW		0x04
#define STAT_MEASUREMENT	0x02
#define STAT_MAIN_ERROR		0x01
#define SPI_RAM				2
#define SPI_LIS				1
#define SPI_LIS_OFF			3
#define SPI_SD				0
#define SPI_SD_OFF			2
#define SD_SPI_PORT			0
#define POWER_DOWN			0x04			//waiting for crd
#define POWER_ADC			0x02			//measuring batt
#define POWER_SAVE			0x06			//waiting for measurement start
#define POWER_EXTENDED		0x0E			//in regular measurement - maybe IDLE?
#define POWER_IDLE			0x00

#if BIGAVR == 1

#define DISP				1
#define DISP_DATA_PORT 		A
#define DISP_CTRL_PORT 		E
#define DISP_RS   			PE4
#define DISP_RW   			PE5
#define DISP_E    			PE6
#define DISP_CS1			PE2
#define DISP_CS2			PE3
#define DISP_RST			PE7
#define DISP_D0   			PA0
#define DISP_D1   			PA1
#define DISP_D2   			PA2
#define DISP_D3   			PA3
#define DISP_D4   			PA4
#define DISP_D5   			PA5
#define DISP_D6   			PA6
#define DISP_D7   			PA7

#define STATUS_PORT			PORTC
#define STATUS_DDR			DDRC

#define SD_CTRL_PORT		G					// SD card defs from mmcconf.h
#define SD_CS				PG1

#define RAM_CTRL_PORT		B
#define RAM_CS				PB7
#define RAM_SPI_PORT		0

#define LIS_CTRL_PORT		B
#define LIS_CS				PB4
#define LIS_RDY_P			D
#define LIS_RDY				PD0
#define LIS_RDY_INT			INT0
#define LIS_RDY_INT_VECT	INT0_vect
#define LIS_RDY_VAL			(_BV(ISC00)|_BV(ISC01))
#define LIS_INT_LED			PD1
#define LIS_SD_ATOMIZE		EIMSK&=~(_BV(0))
#define LIS_SD_DEATOMIZE  	EIMSK|=_BV(0)
#define LIS_SPI_PORT		0

#define SPI0_P				B
#define SPI0_MOSI			PB2
#define SPI0_MISO			PB3
#define SPI0_SS				PB0
#define SPI0_SCK			PB1

#define RTC_TIMSK			TIMSK
#define RTC_TCCR			TCCR0
#define RTC_OCR				OCR0
#define RTC_TCNT			TCNT0
#define RTC_TIFR			TIFR
#define RTC_ASSR_VAL		0x08
#define RTC_TOIE			TOIE0
#define RTC_OCIE			OCIE0
#define RTC_INT				TIMER0_COMP_vect 

#define KEYB				1
#define KB_UP				PF1
#define KB_OK				PF2
#define KB_DOWN				PF3
#define KB_LEFT				PE1
#define KB_RIGHT			PG2
#define KB_PRT_LEFT			E
#define KB_PRT_RIGHT		G
#define KB_PRT				F			

//#define CYCLES_PER_US ((F_CPU+500000)/1000000) 	// cpu cycles per microsecond from global.h

#else

#define DISP				0
#define KEYB				0

#define SD_CTRL_PORT		B					// SD card defs from mmcconf.h
#define SD_CS				PB2
#define SD_DETECT_P			D
#define SD_DETECT			PD3
#define SD_DETECT_INT		INT1
#define SD_DETECT_INT_VECT	INT1_vect
#define SD_DETECT_VAL		~(_BV(ISC11)|_BV(ISC10))
#define SD_PWR_P			C
#define SD_PWR				PC0

#define RAM_CTRL_PORT		C
#define RAM_CS				PC2
#define RAM_SPI_PORT		1


#define LIS_CTRL_PORT		C
#define LIS_CS				PC1
#define LIS_RDY_P			D
#define LIS_RDY				PD2
#define LIS_RDY_INT			INT0
#define LIS_RDY_INT_VECT	INT0_vect
#define LIS_RDY_VAL			(_BV(ISC00)|_BV(ISC01))
#define LIS_SD_ATOMIZE		EIMSK&=~(_BV(0))
#define LIS_SD_DEATOMIZE  	EIMSK|=_BV(0)
#define LIS_SPI_PORT		1

#define SPI0_P				B
#define SPI0_MOSI			PB3
#define SPI0_MISO			PB4
#define SPI0_SS				PB2
#define SPI0_SCK			PB5
#define SPI1_P				D
#define SPI1_MOSI			PD1
#define SPI1_MISO			PD0
#define SPI1_SCK			PD4

#define RTC_TIMSK			TIMSK2
#define RTC_TCCR			TCCR2A
#define RTC_TCCRB			TCCR2B
#define RTC_OCR				OCR2A
#define RTC_TCNT			TCNT2
#define RTC_TIFR			TIFR2
#define RTC_ASSR_VAL		0x20
#define RTC_TOIE			TOIE2
#define RTC_OCIE			OCIE2A
#define RTC_INT				TIMER2_COMPA_vect 

#define ADC_SWITCH_P		B
#define ADC_SW				PB1
#define ADC_CHANNEL			6

#endif		//BIGAVR

#endif		// HW_H_