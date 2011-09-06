#ifndef _SPI_RAM_H_
#define _SPI_RAM_H_

#include "hw.h"
#include "lis3lv.h"

#ifndef GLUE
	#define GLUE(a, b)     a##b
	#define PORT(x)        GLUE(PORT, x)
	#define PIN(x)         GLUE(PIN, x)
	#define DDR(x)         GLUE(DDR, x)
#endif	//GLUE

#define RAM_CS_PORT		PORT(RAM_CTRL_PORT)
#define RAM_CS_DDR		DDR(RAM_CTRL_PORT)


#define RAM_CS_ASSERT     RAM_CS_PORT&=~(_BV(RAM_CS))
#define RAM_CS_DEASSERT   RAM_CS_PORT|=_BV(RAM_CS)


#define	RAM_READ		0x03
#define RAM_WRITE		0x02
#define RAM_RDSR		0x05
#define RAM_WRSR		0x01
#define RAM_BYTEMODE	0x01
#define RAM_PAGEMODE	0x81
#define RAM_SEQMODE		0x41



extern volatile uint16_t SPI_RAM_w_ptr, SPI_RAM_r_ptr;
extern volatile uint8_t SPI_RAM_end;

void RAM_Init_hw(void);

uint8_t RAM_Init(void);

void RAM_Write8_t(uint8_t data);			//, uint8_t atomic);

uint8_t RAM_Read8_t(void);					//(uint8_t atomic);

void RAM_Write_XYZ(XYZ *xyz_data);			//, uint8_t atomic);

//void RAM_Write_LIS_Data(uint8_t index);		//, uint8_t atomic);

uint16_t RAM_Read_block(uint16_t size);		//, uint8_t atomic);
 


#endif //_SPI_RAM_H_