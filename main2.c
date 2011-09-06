
#include <stdint.h>
#include <stdio.h>
//#include <util/delay.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/wdt.h>

#include "hw.h"
//#include "glcd.h"
//#include "k0108.h"
#include "sd_routines.h"
#include "routines.h"
#include "FAT32.h"
#include "spi.h"
#include "lis3lv.h"
#include "rtc.h"
#include "spi_ram.h"
#include "adc.h"

uint8_t Alarm_i __attribute__ ((section (".noinit")));
uint8_t Resets __attribute__ ((section (".noinit")));
uint32_t Reset_long __attribute__ ((section (".noinit")));
uint8_t Log_num __attribute__ ((section (".noinit")));

volatile uint8_t Power;
volatile uint8_t Status;

void wdt_init(void) __attribute__((naked)) __attribute__((section(".init3")));

void wdt_init(void)
{
    MCUSR = 0;
    wdt_disable();
    return;
}

int main(void)
{
	uint8_t error = 0, sm = 0, i = 0, tst = 0; 
	uint16_t data_i=0, battery;
	XYZ dummy_data, empty_data;
	
//	DATE adate[2];
//	TIME atime[2];

	char filename_dat[14];
	char data_txt[16];

	Resets++;
	dummy_data.x = 0x55AA;
	dummy_data.y = 0x55AA;
	dummy_data.z = 0x55AA;
	empty_data.x = 0x0000;
	empty_data.y = 0x0000;
	empty_data.z = 0x0000;

	while(1)
	{
		
		switch(sm)
		{
			case	0:	//HW init
					SD_Init_hw();	// sd port HIZ to disable card when power is off! 
					RAM_Init_hw();
					LIS_Init_hw();
					ADC_Init_hw();
					DDRC |= 0b00111000;		//define unused portc levels to cut the input buffer leakage
					PORTC &= 0b11000111;
					DDRB |= 0b00000001;
					PORTB &= 0b11111110;
					DDRD |= 0b11100000;
					PORTD &= 0b00011111;
					sm++;
					break;
			case	1:	//SW init
					if(Reset_long == 0x55AAAA55)
						RTC_Init();
//					else
					Status = 0;
						
					SPI_Init(SPI_SD_OFF);
					SPI_Init(SPI_LIS);		// SD spi HiZ not to phantom power SD-card
					SPI_Switch(SPI_LIS);
					error += LIS_Init();
					LIS_Stop();
					SPI_Switch(SPI_RAM);
					error += RAM_Init();
					if(!error)
						sm++;
					break;
			case	2:	//Enable interrupts, 
					SPI_Init(SPI_LIS_OFF);
					sm++;
					sei();
					break;
			case	3:	//Power down until sd crd is inserted
//#if	BIGAVR == 0
//					SD_Check();
					if(Status & STAT_SD_PRESENT)
//#endif					
					{
						SD_PULLUP_OFF;		// disable pullup
						sm++;
						i = 0;
						cardType = 0;
						break;
					}
//#if BIGAVR == 0
					PRR = _BV(PRTWI)|_BV(PRTIM0)|_BV(PRTIM1)|_BV(PRADC)|_BV(PRTIM2)|_BV(PRSPI)|_BV(PRUSART0);	//power reduction register, SPI, UART, I2C, ADC, TIMER0,1,(2?), comparator, brown-out, 
					//skontrolovat opatovne spustenie az bude treba!!!!
//#endif					
					SMCR = POWER_DOWN;
					sleep_bod_disable();
					sei();
					sleep_enable();
					sleep_cpu();
					asm volatile(	"nop\n\t"
									"nop\n\t"
									"nop\n\t"
									"nop\n\t"
								::);
					sleep_disable();
					sei();
					break;
			case	4:	// SD-card int
					
					if(i>10)
					{
						sm = 0;
						break;
					}
					Power = 2;
					error = SD_Init();		// power on and setup SD card
					if(!error) 
					{
//						Status |= STAT_SD_PRESENT;
						sm++;
						break;
					}
					i++;
					break;
			case	5:	// Read CRD info
					error = getBootSectorData(); //read boot sector and keep necessary data in global variables, nonatomic
					if(error) 	
						break;
					Atomic = 0;
//					alarm_i = readCfgFile(filename_dat, 0);	//non atomic
					error = readCfgFile(filename_dat);		//, 0);	//non atomic

					if(error > 0)
					{
						RTC_Init();				// init RTC, time is remembered or set later
						Reset_long = 0x55AAAA55;
						Resets = 0;
						Log_num = 0;
						Alarm_i = error;
						Status = 0;
					}
					Power = SD_Idle(0);					//keep power off before start of measurement, SPI disabled as well
//					Power = 1;
					sei();
						
					sm++;
//					CLKPR = 0x80;					// reduce internal clk speed to 1MHz to save power during RTC events
//					CLKPR = 0x03;
					PRR = _BV(PRTWI)|_BV(PRTIM0)|_BV(PRTIM1)|_BV(PRADC)|_BV(PRSPI)|_BV(PRUSART0);	//power reduction register, SPI, UART, I2C, ADC, TIMER0,1,(2?), comparator, brown-out, 
					SMCR = POWER_SAVE;
					break;
			case	6: // Power down until defined start of measurement
					
					sei();
					sleep_enable();
					sleep_cpu();
					asm volatile(	"nop\n\t"
									"nop\n\t"
									"nop\n\t"
									"nop\n\t"
								::);
					sleep_disable();
					sei();
					break;
			case	7:	// Open File
//					CLKPR = 0x80;					//CLK = 8MHz 
//					CLKPR = 0x00;
					
					SPI_Init(SPI_LIS);				// enable USART0 clk tree, reinitialise port
					
					SD_Init();						// enable SPI clk tree, reinitialise port, initialise SD card
					
					Atomic = 0;
					
					SaveLog(filename_dat, data_txt, 0xFF);
					
//					sprintf_P(filename_dat,PSTR("aaa.dat"));
//					data_txt = CreateFilename(data_txt, filename_dat, Log_num);
					
					openFile(CreateFilename(data_txt, filename_dat, Log_num));				//,0,0);		// create file with name from config
					SaveLog(filename_dat, data_txt, 0xFE);
					
					i = openFile(filename_dat);
					closeFile();
					SaveLog(filename_dat, data_txt, i);
					
					openFile(filename_dat);
					
					empty_data.x = 0x0001;			// start of block
					empty_data.y = (uint16_t)Log_num;

					SPI_RAM_w_ptr = 0;
					SPI_RAM_r_ptr = 0;

					RAM_Write_XYZ(&dummy_data);		//,0);	// Start of new data block
					RAM_Write_XYZ(&empty_data);		//,0);
					RAM_Write_XYZ(&dummy_data);		//,0);	// Start of new data block

					battery = ADC_Check_Batt();		// measure battery

					Status |= STAT_MEASUREMENT;		// starts measurement mode

					LIS_Start();					// starts LIS
					
					Power = SD_Idle(1);				// switch off SD card

					sei();
					sm++;
					break;
			case	8:	// Measuring
//					SMCR = POWER_SAVE;				// if not working correctly use POWER_EXTENDED
					sei();
					if(!(Status & STAT_WRITE_DATA))
					{
						sleep_enable();
						sleep_cpu();
						asm volatile(	"nop\n\t"
										"nop\n\t"
										"nop\n\t"
										"nop\n\t"
									::);
						sleep_disable();
						sei();
					}
					break;
			case	9:	// test - end of all
					SPI_Switch(SPI_LIS);
					LIS_Stop();
					Status &= ~STAT_MEASUREMENT;

					SD_Init();
					
					if(SPI_RAM_r_ptr > SPI_RAM_w_ptr)
						SPI_RAM_end = 2;
					else if(SPI_RAM_r_ptr < SPI_RAM_w_ptr)
						SPI_RAM_end = 1;
					else
						SPI_RAM_end = 0;
						
					while(SPI_RAM_end)
					{
						writeFile();		// save last data + zeroes until end of the block
					}
					closeFile();

					Atomic = 0;
					battery = ADC_Check_Batt();

					SaveLog(filename_dat, data_txt, battery);
					Log_num++;
					
					if(Log_num > ((Alarm_i/2)-1))
						sm++;
					else
						sm = 6;
						
					if(Status & STAT_BATT_LOW)
						sm = 11;

					Power = SD_Idle(0);			// put SD into IDLE mode and switch off the power
					PRR = _BV(PRSPI)|_BV(PRUSART0)|_BV(PRTWI)|_BV(PRTIM0)|_BV(PRTIM1)|_BV(PRADC);	//power reduction register, SPI, UART, I2C, ADC, TIMER0,1,(2?), comparator, brown-out, 
					SMCR = POWER_SAVE;
					break;
			case	10:							// waiting until card is removed - rtc maintains regular checkink of the SD card
					sei();
					sleep_enable();
					sleep_cpu();
					asm volatile(	"nop\n\t"
									"nop\n\t"
									"nop\n\t"
									"nop\n\t"
								::);
					sleep_disable();
					sei();
					break;
			case	11:							//total power down, battery needs to by cycled
					PRR = _BV(PRSPI)|_BV(PRUSART0)|_BV(PRTWI)|_BV(PRTIM0)|_BV(PRTIM1)|_BV(PRTIM2)|_BV(PRADC);	//power reduction register, SPI, UART, I2C, ADC, TIMER0,1,(2?), comparator, brown-out, 
					SMCR = POWER_DOWN;
					cli();
					sleep_enable();
					sleep_cpu();
					while(1);
		}

		if (Status & STAT_RTC_UPDATE)
		{
			if (sm > 3)	
			{
				error = SD_Check();
//				error = 1;
				if(error == 0)
				{
					if(Status & STAT_MEASUREMENT)
					{
						SPI_Switch(SPI_LIS);
						LIS_Stop();
						Status &= ~STAT_MEASUREMENT;
					}
					sm = 3;
					
				}
			}
		
			Status &= ~STAT_RTC_UPDATE;
		}


		if(Status & STAT_MEASUREMENT)
		{
//test
			if(Time.s == 0)
			{
				if(tst == 0)
				{
					Atomic = 1;
					RAM_Write_XYZ(&dummy_data);		//,1);		// time tick within data
					Atomic = 0;
					tst = 1;
				}
			}
			else
			{
				tst = 0;
			}
//endtest		
			if(RTC_CmpDate(&Date, &ADate[(Log_num<<1)+1]))
			{
				if(RTC_CmpTime(&Time, &ATime[(Log_num<<1)+1]))
				{
					sm++;
				}
			}
		}
		else if(sm > 5)
		{
			if(RTC_CmpDate(&Date, &ADate[(Log_num<<1)]))
			{
				if(RTC_CmpTime(&Time, &ATime[(Log_num<<1)]))
				{
					sm = 7;
				}
			}
		}

		if (Status & STAT_WRITE_DATA)			//save data
		{	
			if(Power < 2)					// if waking up from power down or idle, else already initialised...
			{
				SD_Init();					// start the SD card and initialise if powered off
			}
				
			writeFile();					//atomic for read RAM! and other stuff as well - probably interrupt during SD work will corrupt stack...
			data_i++;

			if((Status & STAT_WRITE_DATA) == 0)		// just finished 16k block
			{
					battery = ADC_Check_Batt();
					Power = SD_Idle(1);
			}
		}
	}
}