
#if DISP == 1

#include <stdio.h>
#include <inttypes.h>
#include <avr/pgmspace.h>

#include "hw.h"
#include "k0108.h"
#include "font.h"
#include "glcd.h"

//----------------------------------------------------------
//					GLOBAL VARIABLES
//----------------------------------------------------------

//uint8_t lcd_buffer[9][128];

GLCD_POS GLCD_txt_pos;

FILE glcd_str = FDEV_SETUP_STREAM(glcd_putchar, NULL, _FDEV_SETUP_WRITE);

FONT_DEF *GLCD_FONT;

//----------------------------------------------------------
//						FUNCTIONS
//----------------------------------------------------------

void GLCD_Init(void)
{
	k0108_init_glcd();
	k0108_clr_lcd();
	GLCD_FONT = &Font_System5x8;
	stdout = stderr = &glcd_str; // setup streams
}

//----------------------------------------------------------

void GLCD_Line(uint8_t X1, uint8_t Y1, uint8_t X2, uint8_t Y2)
{
	uint8_t CurrentX, CurrentY;
	uint8_t *CurrentXX, *CurrentYY;
	int8_t Xinc, Yinc;
	int8_t XXinc, YYinc;
	int8_t Dx, Dy, TwoDx, TwoDy; 
	int8_t DXX, TwoDXX, TwoDYY, XX2;
	int8_t TwoDxAccumulatedError;

	Dx = (X2-X1); 					// dlzka x
	Dy = (Y2-Y1); 					// dlzka y

	TwoDx = Dx + Dx; 				// 2x dlzka
	TwoDy = Dy + Dy; 				// 2x dlzka

	CurrentX = X1; 					// zaciatok
	CurrentY = Y1; 					// 

	Xinc = 1; 						// krok 
	Yinc = 1; 						// krok

	if(Dx < 0) 						// ak je dlzka zaporna
	{
		Xinc = -1; 					// krok zaporny
		Dx = -Dx;  					// abs(Dx)
		TwoDx = -TwoDx; 			// abs(TwoDx)
	}

	if (Dy < 0) 					// ak je vyska zaporna
	{
		Yinc = -1; 					// krok zaporny
		Dy = -Dy; 					// abs(Dy)
		TwoDy = -TwoDy; 			// abs(TwoDy)
	}

	GLCD_Set_Dot(X1,Y1); 			// prvy bod usecky

	if ((Dx != 0) || (Dy != 0)) 	// ak ma usecka viac ako 1 bod
	{
		if (Dy <= Dx) 					// ak je X vacsi, krokuj X, pocitaj Y
		{ 
			CurrentXX = &CurrentX;
			CurrentYY = &CurrentY;
			XXinc = Xinc;
			YYinc = Yinc;
			TwoDYY = TwoDy;
			TwoDXX = TwoDx;
			DXX = Dx;
			XX2 = X2;
		}
		else
		{ 
			CurrentXX = &CurrentY;
			CurrentYY = &CurrentX;
			XXinc = Yinc;
			YYinc = Xinc;
			TwoDYY = TwoDx;
			TwoDXX = TwoDy;
			DXX = Dy;
			XX2 = Y2;
		}

		TwoDxAccumulatedError = 0; 		// vynuluj zmenu
		do 								// zacni cyklus
		{
			*CurrentXX += XXinc; 		// inkrementuj aktualnu poziciu 
			TwoDxAccumulatedError += TwoDYY; 	// pripocitaj dvojity krok z kolmeho smeru do chyby
			if(TwoDxAccumulatedError > DXX)  	// ak je chyba vacsia ako krok
			{
				*CurrentYY += YYinc; 			// inkrementuj kolmy smer
				TwoDxAccumulatedError -= TwoDXX; 	// odpocitaj dvojity krok
			}
			GLCD_Set_Dot(CurrentX,CurrentY);// stawiamy nast�pny krok (zapalamy piksel)
		}while (*CurrentXX != XX2); // idziemy tak d�ugo, a� osi�gniemy punkt docelowy
	}
}

//------------------------------------------------------------------------------

void GLCD_Circle(uint8_t xcenter, uint8_t ycenter, uint8_t radius)
{
	int16_t tswitch, y, x = 0;
	uint8_t d;

	d = ycenter - xcenter;
	y = radius;
	tswitch = 3 - (2 * radius);
	while (x <= y) 
	{
		GLCD_Set_Dot(xcenter + x, ycenter + y);     
		GLCD_Set_Dot(xcenter + x, ycenter - y);
		GLCD_Set_Dot(xcenter - x, ycenter + y);     
		GLCD_Set_Dot(xcenter - x, ycenter - y);
		GLCD_Set_Dot(ycenter + y - d, ycenter + x); 
		GLCD_Set_Dot(ycenter + y - d, ycenter - x);
		GLCD_Set_Dot(ycenter - y - d, ycenter + x); 
		GLCD_Set_Dot(ycenter - y - d, ycenter - x);

		if (tswitch < 0) 
			tswitch += (4 * x + 6);
		else 
		{
			tswitch += (4 * (x - y) + 10);
			y--;
		}
		x++;
	}
}

//------------------------------------------------------------------------------

void GLCD_Rectangle(uint8_t x, uint8_t y, uint8_t a, uint8_t b)
{
	uint8_t j;

	for (j = 0; j < a; j++) 
	{
		GLCD_Set_Dot(x, y + j);
		GLCD_Set_Dot(x + b - 1, y + j);
	}
	for (j = 0; j < b; j++)	
	{
		GLCD_Set_Dot(x + j, y);
		GLCD_Set_Dot(x + j, y + a - 1);
	}
}

//------------------------------------------------------------------------------

void GLCD_DisplayPicture (PGM_P PictureData)
{
	uint8_t Page=0;
  	uint8_t Column=0;
      
  	for (Page = 0; Page < 8; Page++) 						/* loop on the 8 pages */
   	{
   		k0108_setxy(0,(Page<<3)); 							/* Set the page on left controller*/
		k0108_setxy(64,(Page<<3));							/* Set the page on right controller*/
   		for (Column = 0; Column < 64; Column++)
		{
			k0108_data_write(0,pgm_read_byte_near(PictureData+(128*Page)+Column));		/* write data from flash to left controller*/
			k0108_data_write(1,pgm_read_byte_near(PictureData+(128*Page)+Column+64));	/* write data from flash to right controller*/
		}
	}
}

//------------------------------------------------------------------------------

void GLCD_Putchar (uint8_t Char, FONT_DEF *toto, uint8_t overlay)
{
	uint8_t CharColumn=0; 
	uint8_t shift=0;
	uint8_t UpperCharPointer=1;
	uint8_t buffer[9][3];
	uint8_t ctrl, page;
	uint8_t dist = 2;
	uint32_t clmbuffer, clmmask = 0x000000ff;
	
/* test for carrier return or line feed in char */
	
	if (Char == '\n' || Char == '\r')
	{
		GLCD_txt_pos.Y += ((toto->H) + dist);
		GLCD_txt_pos.X = 0;
	}
	else
	{
/* test for carrier return -> automatic wrap?? */

		if (GLCD_txt_pos.X > 128 - (toto->W)) 
		{	
			GLCD_txt_pos.X = 0;
			GLCD_txt_pos.Y += ((toto->H) + dist);
		}
		if (GLCD_txt_pos.Y > (63 - (toto->H))) 
			GLCD_txt_pos.Y = 0;

/* read out data from area of character */
		for(page = 0; page < ((toto->H)/5 + 1); page++)
		{
			for(CharColumn = 0; CharColumn < (toto->W); CharColumn++)
			{
				ctrl = GLCD_Setxy(GLCD_txt_pos.X + CharColumn, GLCD_txt_pos.Y + (page<<3));
				buffer[CharColumn][page]=k0108_data_read(ctrl);
			}
		}
	
	    	
/* Draw a char in buffer */
		CharColumn = 0;
		shift = GLCD_txt_pos.Y % 8;
		while (CharColumn < (toto->W))
		{
			clmbuffer = 0;
			if((toto->H) > 8)
			{
				UpperCharPointer = 2;
				clmbuffer = (uint32_t)pgm_read_byte_near((toto->FontTable) + ((Char - (toto->O))*(toto->W)*UpperCharPointer) + (CharColumn*UpperCharPointer) + 1);
				clmbuffer = clmbuffer<<8;
				clmmask = 0x000fff;
			}
			clmbuffer |= (uint32_t)pgm_read_byte_near((toto->FontTable) + ((Char - (toto->O))*(toto->W)*UpperCharPointer) + (CharColumn*UpperCharPointer));
			if((toto->H)==6)
			{
				clmbuffer = clmbuffer>>2;
				clmmask = 0x0000003f;
			}
			if((toto->H)==12)
				clmbuffer = clmbuffer>>4;
			
			clmbuffer = clmbuffer<<(shift);
			clmmask = clmmask<<(shift);
			clmmask = ~clmmask;
			buffer[CharColumn][0] &= (uint8_t)clmmask;
			buffer[CharColumn][0] |= (uint8_t)clmbuffer;
			buffer[CharColumn][1] &= (uint8_t)(clmmask>>8);
			buffer[CharColumn][1] |= (uint8_t)(clmbuffer>>8);
			buffer[CharColumn][2] &= (uint8_t)(clmmask>>16);
			buffer[CharColumn][2] |= (uint8_t)(clmbuffer>>16);
			CharColumn++;
		}

/* Insert a space after a char */	

		buffer[CharColumn][0] &= (uint8_t)clmmask;
		buffer[CharColumn][1] &= (uint8_t)(clmmask>>8);
		buffer[CharColumn][2] &= (uint8_t)(clmmask>>16);
		

/* write char buffer back to display */
		shift = GLCD_txt_pos.X + (toto->W);						//to save space in conditions
		for(page = 0; page < ((toto->H)/5 + 1); page++)
		{
			if (shift < 128) 										/* Check if this is the last char of the line */
			{
				for(CharColumn = 0; CharColumn < ((toto->W) +1); CharColumn++)
				{
					ctrl = GLCD_Setxy(GLCD_txt_pos.X + CharColumn, GLCD_txt_pos.Y + (page<<3));
					k0108_data_write(ctrl,buffer[CharColumn][page]);
				}
			}
			else
			{
				for(CharColumn = 0; CharColumn < ((toto->W)); CharColumn++)
				{
					ctrl = GLCD_Setxy(GLCD_txt_pos.X + CharColumn, GLCD_txt_pos.Y + (page<<3));
					k0108_data_write(ctrl,buffer[CharColumn][page]);
				}
			}
		}

		if (shift < 128) 		/* Check if this is the last char of the line */
			GLCD_txt_pos.X = shift + 1;		//update position
		else
			GLCD_txt_pos.X = shift;			//update position
	}
}

//------------------------------------------------------------------------------

void GLCD_Locate (uint8_t Column, uint8_t Line)
{
	GLCD_txt_pos.X = Column;
	GLCD_txt_pos.Y = Line;
}

//------------------------------------------------------------------------------

int glcd_putchar(char c, FILE *unused)
{
	GLCD_Putchar (c, GLCD_FONT, 0);
	return 0;
}

void GLCD_Set_Font(FONT_DEF *toto)
{
	GLCD_FONT = toto;
}

#endif			//DISP