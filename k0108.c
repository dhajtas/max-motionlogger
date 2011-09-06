
#if DISP == 1

#include <avr/io.h>
#include <util/delay.h>
#include <inttypes.h>

#include "hw.h"
#include "k0108.h"

//----------------------------------------------------------
//					GLOBAL VARIABLES
//----------------------------------------------------------

//uint8_t lcd_buffer[9][128];

//----------------------------------------------------------
//						FUNCTIONS
//----------------------------------------------------------

void k0108_init_port(void)
{
	GLCD_DATA = 0x00;   							//Clear data
	GLCD_DATA_DDR = 0xFF;							//set data port to output
	GLCD_CTRL_DDR |= GLCD_CTRL_MASK;				//set control pins as output
	GLCD_CTRL &= ~GLCD_CTRL_MASK;					//clear all ctrl bits - Disp in reset
//	GLCD_CTRL |= (_BV(GLCD_CS0) | _BV(GLCD_CS1));	// disable CS for lcd controllers
}

//-----------------------------------------------------------

void k0108_reset(void)
{
	GLCD_CTRL &= ~(_BV(GLCD_RST));
	_delay_us(100);
	GLCD_CTRL |= _BV(GLCD_RST);
	_delay_us(10);
}

//-----------------------------------------------------------

void k0108_pulse_e(void)
{
	GLCD_CTRL |= _BV(GLCD_E);
	_delay_us(1);
	GLCD_CTRL &= ~(_BV(GLCD_E));
}

//-----------------------------------------------------------

void k0108_slct(uint8_t controller)
{
	if(controller == LEFT)
	{
		GLCD_CTRL &= ~(_BV(GLCD_CS0));
		GLCD_CTRL |= _BV(GLCD_CS1);
	}
	else
	{
		GLCD_CTRL &= ~(_BV(GLCD_CS1));
		GLCD_CTRL |= _BV(GLCD_CS0);
	}
}

//-----------------------------------------------------------

void k0108_wait_busy(void)
{
   	GLCD_CTRL &= ~(_BV(GLCD_DI));       	/* Instruction mode */
	GLCD_DATA_DDR = 0x00;					/* dataport as input*/
  	GLCD_CTRL |= _BV(GLCD_RW);		       	/* read mode */

	GLCD_CTRL |= _BV(GLCD_E);
	_delay_us(1);
	while(GLCD_DATA_IN & BUSY)
	{
		GLCD_CTRL &= ~(_BV(GLCD_E));
		_delay_us(1);
		GLCD_CTRL |= _BV(GLCD_E);
	}
	GLCD_CTRL &= ~(_BV(GLCD_E));
	GLCD_DATA_DDR = 0xFF;
}

//-----------------------------------------------------------

void k0108_cmd_write(uint8_t controller, uint8_t cmd)
{
	k0108_slct(controller);
   	k0108_wait_busy();						/* wait until LCD not busy */
   	GLCD_CTRL &= ~(_BV(GLCD_DI));       	/* Instruction mode */
  	GLCD_CTRL &= ~(_BV(GLCD_RW));       	/* Write mode */
	 	
  	GLCD_DATA = cmd;						/* outbyte */
  	_delay_us(0.5);  	
  	k0108_pulse_e();
}

//-----------------------------------------------------------

uint8_t k0108_cmd_read(uint8_t controller)
{
	uint8_t data=0xff;

	k0108_slct(controller);
   	k0108_wait_busy();						/* wait until LCD not busy */
   	GLCD_CTRL |= ~(_BV(GLCD_DI));       	/* Instruction mode */
	GLCD_DATA_DDR = 0x00;					/* data port as input*/
  	GLCD_CTRL |= _BV(GLCD_RW);		       	/* read mode */

	GLCD_CTRL |= _BV(GLCD_E);				/* set E high*/
	_delay_us(1);
	data = GLCD_DATA_IN;					/* read data from data port*/
	GLCD_CTRL &= ~(_BV(GLCD_E));			/* set E low */
	GLCD_DATA_DDR = 0xff;
		
	return(data);
}


//-----------------------------------------------------------

uint8_t k0108_data_read_hw(uint8_t controller)
{
	uint8_t data=0xff;

	k0108_slct(controller);
   	k0108_wait_busy();						/* wait until LCD not busy */
   	GLCD_CTRL |= _BV(GLCD_DI);       		/* data mode */
	GLCD_DATA_DDR = 0x00;					/* data port as input*/
	GLCD_DATA = 0x00;						/* Hi Z */
  	GLCD_CTRL |= _BV(GLCD_RW);		       	/* read mode */

	GLCD_CTRL |= _BV(GLCD_E);				/* set E high*/
	_delay_us(1);
	data = GLCD_DATA_IN;					/* read data from data port*/
	GLCD_CTRL &= ~(_BV(GLCD_E));			/* set E low */
	GLCD_DATA_DDR = 0xff;					/* data port as output */ 
		
	return(data);
}


//-----------------------------------------------------------
//------------------Public functions-------------------------
//-----------------------------------------------------------

uint8_t k0108_data_read(uint8_t controller)
{
	uint8_t temp;
	temp = k0108_data_read_hw(controller);				//dummy read - needed by display
	temp = k0108_data_read_hw(controller);
	return(temp);
}

//-----------------------------------------------------------

void k0108_data_write(uint8_t controller, uint8_t data)
{
	k0108_slct(controller);
   	k0108_wait_busy();						/* wait until LCD not busy */
   	GLCD_CTRL |= _BV(GLCD_DI);       		/* data mode */
  	GLCD_CTRL &= ~(_BV(GLCD_RW));       	/* Write mode */
	 	
  	GLCD_DATA = data;						/* outbyte */
  	_delay_us(0.5);  	
  	k0108_pulse_e();
}

//-----------------------------------------------------------

void k0108_init_glcd(void)
{
	uint8_t i;
	
	k0108_init_port();
	k0108_reset();
	
	for(i=0; i<2; i++)
	{
		k0108_cmd_write(i, GLCD_OFF);
		k0108_cmd_write(i, GLCD_START_LINE);
		k0108_cmd_write(i, GLCD_SET_PAGE);
		k0108_cmd_write(i, GLCD_SET_Y_ADDR);
		k0108_cmd_write(i, GLCD_ON);
	}

	_delay_ms(100);
}

//-----------------------------------------------------------

uint8_t k0108_setxy(uint8_t x, uint8_t y)
{
	uint8_t page = y/8;
	uint8_t ctrl;

	if(x>63)
	{
		ctrl = RIGHT;
		x = x-64;
	}
	else
		ctrl = LEFT;
	
	k0108_cmd_write(ctrl, GLCD_SET_PAGE | (page & 0x07));	//set page - aka Y 	
	k0108_cmd_write(ctrl, GLCD_SET_Y_ADDR | (x & 0x3f));	//set X in proper controller  
	return(ctrl);
}

//-----------------------------------------------------------

void k0108_set_startline(uint8_t y)
{
	k0108_cmd_write(0, GLCD_START_LINE | (y & 0x3f));	//	
	k0108_cmd_write(1, GLCD_START_LINE | (y & 0x3f));	//
}


//-----------------------------------------------------------

void k0108_clr_lcd(void)
{
	uint8_t x,y,ctrl;
	
	for(x=0; x<128; x++)
	{
		for(y=0; y<64; y+=8)
		{
			ctrl = k0108_setxy(x,y);
			k0108_data_write(ctrl,0x00);
		}
	}
	k0108_setxy(0,0);
	k0108_set_startline(0);
}

//-----------------------------------------------------------

void k0108_set_dot(uint8_t x, uint8_t y)
{
	uint8_t temp, ctrl;

	ctrl = k0108_setxy(x, y);
//	temp = k0108_data_read(ctrl);	// dummy read
	temp = k0108_data_read(ctrl);	// read back current value
	ctrl = k0108_setxy(x, y);
	k0108_data_write(ctrl, temp | _BV(y % 8));

	k0108_set_startline(0);
}

//-----------------------------------------------------------

void k0108_clr_dot(uint8_t x, uint8_t y)
{
	uint8_t temp, ctrl;

	ctrl = k0108_setxy(x, y);
//	temp = k0108_data_read(ctrl);	// dummy read
	temp = k0108_data_read(ctrl);	// read back current value
	ctrl = k0108_setxy(x, y);
	k0108_data_write(ctrl, temp & ~(_BV(y % 8)));

	k0108_set_startline(0);
}
//-----------------------------------------------------------

void k0108_toggle_dot(uint8_t x, uint8_t y)
{
	uint8_t temp, ctrl;

	ctrl = k0108_setxy(x, y);
//	temp = k0108_data_read(ctrl);	// dummy read
	temp = k0108_data_read(ctrl);	// read back current value
	ctrl = k0108_setxy(x, y);
	if (temp & _BV(y % 8))
		k0108_data_write(ctrl, temp & ~(_BV(y % 8)));
	else
		k0108_data_write(ctrl, temp | _BV(y % 8));

	k0108_set_startline(0);
}

#endif			//DISP