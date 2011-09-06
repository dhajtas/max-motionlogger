
#ifndef GLCD_H_
#define GLCD_H_

#if DISP == 1

#include "font.h"

//---------------------------------------------------------
//-------------------- Structure Defs ---------------------
//---------------------------------------------------------

typedef struct GLCD_POS_
{
	uint8_t X;		// X position
	uint8_t Y;		// Y position
} GLCD_POS;

//---------------------------------------------------------
//----------------- Function prototypes -------------------
//---------------------------------------------------------


#define GLCD_Set_Dot(x,y)		k0108_set_dot(x,y)
#define GLCD_Setxy(x,y)			k0108_setxy(x,y)
#define GLCD_Clr()				k0108_clr_lcd()

void GLCD_Init(void);

void GLCD_Line(uint8_t, uint8_t, uint8_t, uint8_t);

void GLCD_Circle(uint8_t, uint8_t, uint8_t);

void GLCD_Rectangle(uint8_t, uint8_t, uint8_t, uint8_t);

void GLCD_DisplayPicture (PGM_P PictureData);

void GLCD_Putchar (uint8_t Char, FONT_DEF *toto, uint8_t overlay);

void GLCD_Locate (uint8_t Column, uint8_t Line);

int glcd_putchar(char c, FILE*);

void GLCD_Set_Font(FONT_DEF*);

#endif		//DISP

#endif		//GLCD_H_
