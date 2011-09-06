#ifndef	K0108_H_
#define K0108_H_

#if DISP == 1

#include "hw.h"

//----------------------------------------------------------
//						DEFS
//----------------------------------------------------------

#ifndef GLUE
	#define GLUE(a, b)     a##b
	#define PORT(x)        GLUE(PORT, x)
	#define PIN(x)         GLUE(PIN, x)
	#define DDR(x)         GLUE(DDR, x)
#endif	//GLUE


#define 	GLCD_DATA		PORT(DISP_DATA_PORT)
#define 	GLCD_DATA_IN	PIN(DISP_DATA_PORT)
#define		GLCD_DATA_DDR	DDR(DISP_DATA_PORT)
#define		GLCD_CTRL		PORT(DISP_CTRL_PORT)
#define		GLCD_CTRL_DDR	DDR(DISP_CTRL_PORT)
#define		GLCD_CS0		DISP_CS1
#define		GLCD_CS1		DISP_CS2
#define		GLCD_DI			DISP_RS
#define		GLCD_RW 		DISP_RW
#define		GLCD_E			DISP_E
#define		GLCD_RST		DISP_RST

// HD61202/KS0108 command set
#define GLCD_OFF			0x3E	// 00111110: turn display off
#define GLCD_ON				0x3F	// 00111111: turn display on

#define GLCD_START_LINE		0xC0	// 11XXXXXX: set lcd start line

#define GLCD_SET_PAGE		0xB8	// 10111XXX: set lcd page (X) address
#define GLCD_SET_Y_ADDR		0x40	// 01YYYYYY: set lcd Y address

#define GLCD_STATUS_BUSY	0x80	// (1)->LCD IS BUSY
#define GLCD_STATUS_ONOFF	0x20	// (0)->LCD IS ON
#define GLCD_STATUS_RESET	0x10	// (1)->LCD IS RESET

#define RIGHT				0
#define LEFT				1
#define BUSY				0x80

// determine the number of controllers
// (make sure we round up for partial use of more than one controller)
#define GLCD_NUM_CONTROLLERS	2
#define GLCD_CTRL_MASK		(_BV(GLCD_CS0)|_BV(GLCD_CS1)|_BV(GLCD_DI)|_BV(GLCD_RW)|_BV(GLCD_E)|_BV(GLCD_RST))


//-----------------------------------------------------------------------------------------------//
//				structure definitions 
//-----------------------------------------------------------------------------------------------//
typedef struct LCD_LOC
{
	uint8_t page;
	uint8_t adr;
} LCD_LOC;

//typedef struct LCD_BUFF
//{
//	uint8_t page;
//	uint
//}

//----------------------------------------------------------
//					GLOBAL VARIABLES
//----------------------------------------------------------

//extern uint8_t lcd_buffer[9][128];
//extern PGM_P LOGO_LUT[9][132];
//extern PGM_P ALPHA_LUT[26][5];
//extern PGM_P alpha_LUT[26][5];
//extern PGM_P NUM_LUT[10][5];

//----------------------------------------------------------
//						FUNCTIONS
//----------------------------------------------------------


void k0108_data_write(uint8_t, uint8_t);

uint8_t k0108_data_read(uint8_t);

void k0108_init_glcd(void);

uint8_t k0108_setxy(uint8_t, uint8_t);

void k0108_set_startline(uint8_t);

void k0108_clr_lcd(void);

void k0108_set_dot(uint8_t, uint8_t);

void k0108_clr_dot(uint8_t, uint8_t);

void k0108_toggle_dot(uint8_t, uint8_t);

#endif		//DISP

#endif 		//K0108_H_

