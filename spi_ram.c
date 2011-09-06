#include <stdio.h>
#include <inttypes.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

#include "hw.h"
#include "spi.h"
//#include "lis3lv.h"
#include "spi_ram.h"
#include "sd_routines.h"
//#include "glcd.h"
//#include "k0108.h"

volatile uint16_t SPI_RAM_w_ptr, SPI_RAM_r_ptr;
volatile uint8_t SPI_RAM_end;

extern uint8_t Atomic;

//-----------------------------------------------------------------------

void RAM_Init_hw(void)
{
	RAM_CS_DDR |= _BV(RAM_CS);		// release chip select
	RAM_CS_PORT |= _BV(RAM_CS);	
}

//-----------------------------------------------------------------------

uint8_t RAM_Init(void)
{

	uint8_t error;

	RAM_CS_ASSERT;
	SPI_Send8(RAM_RDSR,RAM_SPI_PORT);
	error = SPI_Transfer8(0x00,RAM_SPI_PORT);
	RAM_CS_DEASSERT;

	_delay_us(1);
	
	RAM_CS_ASSERT;
	SPI_Send8(RAM_WRSR,RAM_SPI_PORT);
	SPI_Send8(RAM_SEQMODE,RAM_SPI_PORT);
	RAM_CS_DEASSERT;

	_delay_us(1);

	RAM_CS_ASSERT;
	SPI_Send8(RAM_RDSR,RAM_SPI_PORT);
	error = SPI_Transfer8(0xFF,RAM_SPI_PORT);
	RAM_CS_DEASSERT;

	_delay_us(1);

	
	if(error != RAM_SEQMODE)
	{
#if BIGAVR == 1
		printf_P(PSTR("SPI RAM PROBLEM"));
#endif		//BIGAVR

		return(1);
	}
	
	SPI_RAM_w_ptr = 0;
	SPI_RAM_r_ptr = 0;
	SPI_RAM_end = 0;
	return(0);
}

//-----------------------------------------------------------------------

void RAM_Write8_t(uint8_t data)			//, uint8_t atomic)
{
	
	if(Atomic)
	{
		LIS_SD_ATOMIZE;				// disable INT0	to make it quasi atomic
	}
	
	RAM_CS_ASSERT;
	
	SPI_Send8(RAM_WRITE,RAM_SPI_PORT);
	SPI_Send8((uint8_t)(SPI_RAM_w_ptr>>8),RAM_SPI_PORT);
	SPI_Send8((uint8_t)SPI_RAM_w_ptr,RAM_SPI_PORT);
	SPI_Send8(data,RAM_SPI_PORT);
	SPI_RAM_w_ptr ++;
		if((Status & STAT_DATA_BANK) == 0)
		{
			if(SPI_RAM_w_ptr > 0x3FFF)
			{
				Status |= STAT_DATA_BANK;
				Status |= STAT_WRITE_DATA;
			}
		}
		else
		{
			if(SPI_RAM_w_ptr > 0x7FFF)
			{
				SPI_RAM_w_ptr = 0;
				Status &= ~(STAT_DATA_BANK);
				Status |= STAT_WRITE_DATA;
			}
		}
	RAM_CS_DEASSERT;

	if(Atomic)
	{
		LIS_SD_DEATOMIZE;				// enable INT0	to make it quasi atomic
	}
}

//-----------------------------------------------------------------------

uint8_t RAM_Read8_t(void)				//(uint8_t atomic)
{
	uint8_t data;
	
	if(Atomic)
	{
		LIS_SD_ATOMIZE;				// disable INT0	to make it quasi atomic
	}
	
	RAM_CS_ASSERT;
	
	SPI_Send8(RAM_READ,RAM_SPI_PORT);
	SPI_Send8((uint8_t)(SPI_RAM_r_ptr>>8),RAM_SPI_PORT);
	SPI_Send8((uint8_t)SPI_RAM_r_ptr,RAM_SPI_PORT);
	data = SPI_Receive8(RAM_SPI_PORT);
	RAM_CS_DEASSERT;

	if(Atomic)
	{
		LIS_SD_DEATOMIZE;				// enable INT0	to make it quasi atomic
	}

	SPI_RAM_r_ptr ++;
	return(data);
}

//---------------------------------------------------------------------------------

void RAM_Write_XYZ(XYZ *xyz_data)		//, uint8_t atomic)
{
	uint8_t *data8_t;
	uint8_t i;
	
	data8_t = (uint8_t*)xyz_data;

	if(Atomic)
	{
		LIS_SD_ATOMIZE;				// disable INT0	to make it quasi atomic
	}
	
	RAM_CS_ASSERT;
	
	SPI_Send8(RAM_WRITE,RAM_SPI_PORT);
	SPI_Send8((uint8_t)(SPI_RAM_w_ptr>>8),RAM_SPI_PORT);
	SPI_Send8((uint8_t)SPI_RAM_w_ptr,RAM_SPI_PORT);
	for(i=0;i<6;i++)
	{
		SPI_Send8(data8_t[i],RAM_SPI_PORT);
		SPI_RAM_w_ptr ++;
		if((Status & STAT_DATA_BANK) == 0)
		{
			if(SPI_RAM_w_ptr > 0x3FFF)
			{
				Status |= STAT_DATA_BANK;
				Status |= STAT_WRITE_DATA;
			}
		}
		else
		{
			if(SPI_RAM_w_ptr > 0x7FFF)
			{
				SPI_RAM_w_ptr = 0;
				Status &= ~(STAT_DATA_BANK);
				Status |= STAT_WRITE_DATA;
			}
		}
	}
	
	RAM_CS_DEASSERT;

	if(Atomic)
	{
		LIS_SD_DEATOMIZE;				// enable INT0	to make it quasi atomic
	}

}

//-----------------------------------------------------------------------

//void RAM_Write_LIS_Data(uint8_t index)		//, uint8_t atomic)
//{
//	XYZ *xyz_data;
	
//	xyz_data = (XYZ*)&LIS_Data[index];
//	RAM_Write_XYZ(xyz_data);				//, atomic);

/*	if(atomic)
	{
		LIS_SD_ATOMIZE;				// disable INT0	to make it quasi atomic
	}
	
	RAM_CS_ASSERT;
	
	SPI_Send8(RAM_WRITE,RAM_SPI_PORT);
	SPI_Send8((uint8_t)(SPI_RAM_w_ptr>>8),RAM_SPI_PORT);
	SPI_Send8((uint8_t)SPI_RAM_w_ptr,RAM_SPI_PORT);
	for(i=0;i<6;i++)
	{
		SPI_Send8(data8_t[i],RAM_SPI_PORT);
		SPI_RAM_w_ptr ++;
		if((Status & STAT_DATA_BANK) == 0)
		{
			if(SPI_RAM_w_ptr > 0x3FFF)
			{
				Status |= STAT_DATA_BANK;
				Status |= STAT_WRITE_DATA;
			}
		}
		else
		{
			if(SPI_RAM_w_ptr > 0x7FFF)
			{
				SPI_RAM_w_ptr = 0;
				Status &= ~(STAT_DATA_BANK);
				Status |= STAT_WRITE_DATA;
			}
		}
	}
	
	RAM_CS_DEASSERT;

	if(atomic)
	{
		LIS_SD_DEATOMIZE;				// enable INT0	to make it quasi atomic
	}
*/
//}

//-----------------------------------------------------------------------

uint16_t RAM_Read_block(uint16_t size)				//, uint8_t atomic)
{
	uint16_t i;

	if(Atomic)
	{
		LIS_SD_ATOMIZE;				// disable INT0	to make it quasi atomic
	}

	RAM_CS_ASSERT;

	SPI_Send8(RAM_READ,RAM_SPI_PORT);
	SPI_Send8((uint8_t)(SPI_RAM_r_ptr>>8),RAM_SPI_PORT);
	SPI_Send8((uint8_t)SPI_RAM_r_ptr,RAM_SPI_PORT);
	for(i=0;i<size;i++)
	{
		SPI_RAM_r_ptr ++;
		SPI_RAM_r_ptr &= 0x7FFF;
		
		if((Status & STAT_MEASUREMENT) == 0)
		{
			switch(SPI_RAM_end)
			{
				case	2:	buffer[i] = SPI_Receive8(RAM_SPI_PORT);
							if(SPI_RAM_r_ptr == 0x7FFF)
								SPI_RAM_end = 1;
							break;
				case	1:
							if(SPI_RAM_r_ptr < SPI_RAM_w_ptr)
							{
								buffer[i] = SPI_Receive8(RAM_SPI_PORT);
								break;
							}
							else
								SPI_RAM_end = 0;
				case	0:	buffer[i] = 0x00;
							break;
								
			}
		}
		else
		{
			buffer[i] = SPI_Receive8(RAM_SPI_PORT);
		}
		
	}

	RAM_CS_DEASSERT;

	if(Atomic)
	{
		LIS_SD_DEATOMIZE;				
	}

	if(SPI_RAM_r_ptr == 0x4000 || SPI_RAM_r_ptr == 0x0000)
		Status &= ~STAT_WRITE_DATA;
	
	return(i);
}