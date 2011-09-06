//LIS3LV driver

#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <inttypes.h>
#include <stdio.h>

#include "hw.h"
#include "lis3lv.h"
#include "spi.h"
#include "spi_ram.h"

//--------------------------------------------------------------------------------------//
//---------------------------- Global Variables ----------------------------------------//
//--------------------------------------------------------------------------------------//

//volatile XYZ LIS_Data[10];
//volatile uint8_t LIS_Index;

//--------------------------------------------------------------------------------------//
//-------------------------------- Routines --------------------------------------------//
//--------------------------------------------------------------------------------------//

/*
ISR(LIS_RDY_INT_VECT,ISR_NOBLOCK)
{
	static uint8_t tmp_index = 0;
	
	LIS_read_XYZ(LIS_Index, 0);

	tmp_index++;
	if(tmp_index>1)
	{
		tmp_index = 0;
		LIS_Index++;
		if(LIS_Index == 10)
		{
			LIS_Index = 0;
			Status |= STAT_MAIN_ERROR;
		}
		Status |= STAT_LIS_DATA_RDY;

	}
	sei();
}
*/

ISR(LIS_RDY_INT_VECT)			//,ISR_NOBLOCK)		//??
{
	static uint8_t tmp_index = 0;
	XYZ XYZ_data;
	
	LIS_read_XYZ(&XYZ_data);

	tmp_index++;
	if(tmp_index>1)
	{
		tmp_index = 0;
		RAM_Write_XYZ(&XYZ_data);			//, 1);	//atomic!
//		Status |= STAT_LIS_DATA_RDY;
	}
	sei();
}


//--------------------------------------------------------------------------------------//

void LIS_Init_hw(void)
{
	LIS_CS_PORT |= _BV(LIS_CS);				// setting up the CS for LIS
	LIS_CS_DDR |= _BV(LIS_CS);
	
#if BIGAVR == 1
	LIS_RDY_PORT &= (~_BV(LIS_RDY));		//must be HiZ on BIGAVR (5V! device))
	LIS_RDY_DDR |= 0xF0;					// test _BV(LIS_INT_LED);
#else
	LIS_RDY_PORT |= _BV(LIS_RDY);		//pull-up enable on 3V device
#endif		//BIGAVR
	LIS_RDY_DDR &= (~_BV(LIS_RDY));		//setting the interrupt line (active H??)
}

//--------------------------------------------------------------------------------------//

uint8_t LIS_Init(void)
{
	uint8_t init_data;

	init_data = LIS_read_reg_8(WHO_AM_I);
	if(init_data == 0x3A)
	{
#if BIGAVR == 1
		printf_P(PSTR("LIS3LV02DQ found"));
#endif			//BIGAVR
//setting up interrupt
		EIMSK &= ~(_BV(LIS_RDY_INT));				// disable INT0
		EICRA |= LIS_RDY_VAL;	//rising edge
		//EICRA |= _BV(ISC01);				//falling edge
	}
	else
	{
#if BIGAVR == 1
		printf_P(PSTR("LIS3LV02DQ not found"));
#endif //BIGAVR
		return(1);
	}


//	LIS_write_reg_8(CTRL2,(_BV(BDU)|_BV(IEN)|_BV(DRDY)));	// FS=+/-2g, update after read, little endian, data ready on rdy pin,
//	LIS_write_reg_8(0x21, 0x04);
	LIS_write_reg_8(CTRL2,(_BV(FS)|_BV(DRDY)));				// FS=+/-6g, little endian, data ready on rdy pin,
	LIS_write_reg_8(CTRL3,_BV(HPDD));						// enable HP filter for direction change detector (for future interrupt)
	
// add DD_cfg, dd_src, dd_ths settings, change the interrupt behavior...

//	LIS_Index = 0;
	return(0);
}

//--------------------------------------------------------------------------------------//

void LIS_Start(void)
{
	XYZ dump;
	
	LIS_write_reg_8(CTRL1,(_BV(LIS_PD1)|_BV(LIS_PD0)|DF_512|_BV(ZEN)|_BV(YEN)|_BV(XEN)));		// device on, decimation 512x, x, y, z enable
	EIMSK |= 0x01;						//enable INT0
	EIFR  = 0x01;						//clear any potential interrupts
	
	LIS_read_XYZ(&dump);
	sei();
	
}

//--------------------------------------------------------------------------------------//

void LIS_Stop(void)
{

	LIS_write_reg_8(CTRL1,0);			// device off,
	EIMSK &= ~(0x01);					// disable INT0
	
}

//--------------------------------------------------------------------------------------//

/*
void LIS_read_XYZ(uint8_t index, uint8_t acc)
{
	if(!acc)
	{
		LIS_Data[index].x = 0;
		LIS_Data[index].y = 0;
		LIS_Data[index].z = 0;
	}

	LIS_Data[index].x += LIS_read_reg_16(OUTX_L);
	LIS_Data[index].y += LIS_read_reg_16(OUTY_L);
	LIS_Data[index].z += LIS_read_reg_16(OUTZ_L);
}
*/

void LIS_read_XYZ(XYZ *xyz_data)
{
	
	xyz_data->x = LIS_read_reg_16(OUTX_L);
	xyz_data->y = LIS_read_reg_16(OUTY_L);
	xyz_data->z = LIS_read_reg_16(OUTZ_L);
}

//--------------------------------------------------------------------------------------//

/*
void LIS_decimate_XYZ(uint8_t index)
{
		LIS_Data[index].x = LIS_Data[index].x>>1;
		LIS_Data[index].y = LIS_Data[index].y>>1;
		LIS_Data[index].z = LIS_Data[index].z>>1;
}
*/

//--------------------------------------------------------------------------------------//

uint8_t LIS_read_reg_8(uint8_t reg)
{
	uint8_t data;
	
	reg |= 0x80;		// r/~w bit set for read
	
	LIS_CS_ASSERT;
	
	SPI_Transfer8(reg,LIS_SPI_PORT);
	data = SPI_Transfer8(0,LIS_SPI_PORT);
	
	LIS_CS_DEASSERT;
	
	return(data);
}

//--------------------------------------------------------------------------------------//

uint16_t LIS_read_reg_16(uint8_t reg)
{
	uint16_t data;

	reg |= 0xC0;		// r/~w bit and m/~s bit set for multi read
	
	LIS_CS_ASSERT;
	
	SPI_Transfer8(reg,LIS_SPI_PORT);
	data = SPI_Transfer16(0,LE,LIS_SPI_PORT);
	
	LIS_CS_DEASSERT;
	
	return(data);
}

//--------------------------------------------------------------------------------------//

void LIS_write_reg_8(uint8_t reg, uint8_t data)
{
	reg &= 0x7F;		// r/~w bit cleared for write
	
	LIS_CS_ASSERT;
	
	SPI_Transfer8(reg,LIS_SPI_PORT);
	data = SPI_Transfer8(data,LIS_SPI_PORT);
	
	LIS_CS_DEASSERT;
}

//--------------------------------------------------------------------------------------//

void LIS_write_reg_16(uint8_t reg, uint16_t data)
{
	reg &= 0x7F;		// r/~w bit cleared for write
	reg |= 0x40;		// m/~s bit set for multi write
	
	LIS_CS_ASSERT;
	
	SPI_Transfer8(reg,LIS_SPI_PORT);
	data = SPI_Transfer16(data,LE,LIS_SPI_PORT);
	
	LIS_CS_DEASSERT;
}