
#include <stdio.h>
#include <inttypes.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>

#include "hw.h"
#include "glcd.h"
#include "k0108.h"
#include "sd_routines.h"
#include "routines.h"
#include "FAT32.h"
#include "spi.h"
#include "lis3lv.h"
#include "rtc.h"
#include "keyb.h"
#include "spi_ram.h"


volatile uint8_t Status;


int main(void)
{
	uint8_t error, kb, i; 
	uint16_t data_i=0;

	DATE adate;
	TIME atime;

	uint8_t filename_dat[13];

#if BIGAVR == 1	
	GLCD_Init();
	SPI_Init();		//only one SPI on BIGAVR... need to change for ATmega328
	RAM_Init_hw();
	SD_Init_hw();
	LIS_Init_hw();
	KB_Init();

	DDRD |= 0x04;
	STATUS_DDR = 0xFF;		//port C ako status byte output
	Status = 0;

	
	Date.d = 31;
	Date.m = 7;
	Date.y = 30;
	Time.h = 11;
	Time.m = 18;
	Time.s = 30;
	

	GLCD_Set_Font(&Font_System5x8);
	GLCD_Locate(0,0);

	_delay_ms(1000);
	
	GLCD_Clr();
	GLCD_Locate(0,0);
	
// 3 axis sensor init
	
/*	if(error)
	{
		printf_P(PSTR("\n port test...."));
		while(1)
		{
			LIS_CS_ASSERT;
			SPI_Transfer8(0x0F);
			LIS_CS_DEASSERT;
			SPI_Transfer8(0xF0);
			LIS_CS_ASSERT;
			SPI_Transfer8(0xA5);
			LIS_CS_DEASSERT;
			SPI_Transfer8(0x5A);
			_delay_ms(10);
		}
	}
*/

	RAM_Init();

	error = LIS_Init();
	RTC_Init();

// SD card init
	cardType = 0;

	for (i=0; i<10; i++)
	{
			printf_P(PSTR("%d"),i);

		error = SD_Init();
		if(!error) break;
	}

	if(error)
	{
		if(error == 1) 
			printf_P(PSTR("SD NOT DETECTED..\n"));
		if(error == 2) 
			printf_P(PSTR("CRD INIT FAILED..\n"));

		printf_P(PSTR("PORT TEST"));

		while(1)//;  //wait here forever if error in SD init 
		{
			RAM_CS_ASSERT;
			_delay_ms(1);
			RAM_CS_DEASSERT;
			_delay_ms(1);
		}
	}

	switch (cardType)
	{
		case 1:printf_P(PSTR("STD CAPACITY CRD (V 1.X) DET!\n"));
				break;
		case 2:printf_P(PSTR("HIGH CAPACITY CRD DET!\n"));
				break;
		case 3:printf_P(PSTR("STD CAPACITY CRD (V 2.X) DET!\n"));
				break;
		default:printf_P(PSTR("UNKNOWN SD CRD DETECTED!\n"));
				break; 
	}


//	SPI_HIGH_SPEED;	//SCK - 4 MHz
//	_delay_ms(20000);
	
	error = getBootSectorData(0); //read boot sector and keep necessary data in global variables, nonatomic
	if(error) 	
	{
		printf_P (PSTR("FAT32 NOT FOUND!"));  //FAT32 incompatible drive
	}	
	
	readCfgFile(&adate, &atime, filename_dat, 0);	//non atomic

	_delay_ms(1000);
	
	openFile(filename_dat,0,0);

	_delay_ms(1000);
	
	sei();

	LIS_read_XYZ(data, 0);			//dummy read to start proper interrupts (edge triggered)
	GLCD_Clr();
	
	while(1)
	{
//test
		STATUS_PORT = Status;
		
		if (Status & LIS_DATA_RDY)
		{
			for(i=0;i<LIS_Index;i++)
			{
				RAM_Write_XYZ(data+i, 1);	//atomic!
//test
				RAM_Read_block(buffer,6,1);
			}
			LIS_Index = 0;
			Status &= ~LIS_DATA_RDY;
//if LEDS are not to GND - problem with high voltage on INT !!! LED needs to be moved to other port !!!
//			GLCD_Clr();
		}
		else
		{
/*			if (Status & WRITE_DATA)			//save data
			{	
				writeFile(1);					//atomic!

				data_i++;

				if(Status & DATA_BANK)
				{
					if(SPI_RAM_r_ptr > 0x3FFF)
					{
						Status &= ~WRITE_DATA;
					}
				}
				else
				{
					if(SPI_RAM_r_ptr > 0x7FFF)
					{
						Status &= ~WRITE_DATA;
						SPI_RAM_r_ptr = 0;
					}
				}
			}
*/
			if ((Status & (RTC_UPDATE|WRITE_DATA)) == RTC_UPDATE)
			{
				Status &= ~RTC_UPDATE;
				RTC_DateTime();
				GLCD_Set_Font(&Font_System3x6);
				GLCD_Locate(70,0);
				printf_P(PSTR("  :  :  "));
				GLCD_Locate(70,8);
				printf_P(PSTR("  .  .  "));
				GLCD_Locate(70,16);
				printf_P(PSTR("  :  :  "));
				GLCD_Locate(70,24);
				printf_P(PSTR("  .  .  "));
				GLCD_Locate(70,0);
				printf_P(PSTR("%2d:%02d:%02d\n"),Time.h, Time.m, Time.s);
				GLCD_Locate(70,8);
				printf_P(PSTR("%2d.%02d.%02d\n"),Date.d, Date.m, Date.y-20);
				GLCD_Locate(70,16);
				printf_P(PSTR("%2d:%02d:%02d\n"),atime.h, atime.m, atime.s);
				GLCD_Locate(70,24);
				printf_P(PSTR("%2d.%02d.%02d\n"),adate.d, adate.m, adate.y-20);
				GLCD_Locate(70,32);
				printf("%s", filename_dat);

				GLCD_Set_Font(&Font_System5x8);
				GLCD_Locate(0,25);
				printf_P(PSTR("X:       g \n"));
				printf_P(PSTR("Y:       g \n"));
				printf_P(PSTR("Z:       g \n"));
				printf_P(PSTR("i:     \n"));
				GLCD_Locate(0,25);
				printf_P(PSTR("X: %+5i g \n"),data[0].x);
				printf_P(PSTR("Y: %+5i g \n"),data[0].y);
				printf_P(PSTR("Z: %+5i g \n"),data[0].z);
				printf_P(PSTR("i: %4x %4x %d"), SPI_RAM_w_ptr, SPI_RAM_r_ptr,data_i);
	//			PORTD &= 0x7F;						//LED off
			}
//			kb = KB_getkey(0);
//			switch(kb)
//			{
//				case	KBOK:		LIS_read_XYZ(data,0);
//									break;
//				case	KBUP:		LIS_Init();
//									break;
//				case	KBDOWN:		LIS_Init_hw();
//									GLCD_Clr();
//									break;
//				default:			break;
//			}
		}
		
/*		if(data_i == 8471)
		{
			closeFile(0);		//nonatomic
			GLCD_Clr();
			printf_P(PSTR("\n Max i reached\n  File closed\n Remove Card"));
			break;
		}
		
		if(Time.h == atime.h)
		{
			if(Time.m == atime.m)
			{
				if(Time.s == atime.s)
				{
					closeFile(0);	//nonatomic
					GLCD_Clr();
					printf_P(PSTR("\n Max time\n I = %d\n File closed\n Remove Card"), data_i);
					break;
				}
			}
		}
*/
//		_delay_ms(1000);
	}
	
	while(1);


#endif //BIGAVR
		
	return(0);
}