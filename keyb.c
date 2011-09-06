
#if KEYB == 1

#include <avr/io.h>
#include <inttypes.h>

#include "hw.h"
#include "keyb.h"


void KB_Init(void)
{
	KB_L_DDR &= ~(_BV(KB_LEFT));							//set all buttons as input
	KB_R_DDR &= ~(_BV(KB_RIGHT));
	KB_DDR &= ~(_BV(KB_UP) | _BV(KB_OK) | _BV(KB_DOWN));
	
	KB_L_PORT |= _BV(KB_LEFT);								// enable pull-ups (buttons pulls low)
	KB_R_PORT |= _BV(KB_RIGHT);
	KB_PORT |= _BV(KB_UP) | _BV(KB_OK) | _BV(KB_DOWN);

}

uint8_t KB_getkey(uint8_t wait)
{
	uint8_t pom;

	do
	{	
		pom = (~KB_PIN) & (_BV(KB_UP) | _BV(KB_OK) | _BV(KB_DOWN));
	
		if(~KB_L_PIN & _BV(KB_LEFT))
			pom |= 0x08;
		
		if(~KB_R_PIN & _BV(KB_RIGHT))
			pom |= 0x10;
	
		switch(pom)
		{
			case	0x01:	pom = KBUP;
							break;
			case	0x02:	pom = KBOK;
							break;
			case	0x04:	pom = KBDOWN;
							break;
			case	0x08:	pom = KBLEFT;
							break;
			case	0x10:	pom = KBRIGHT;
							break;
			default:		pom = 0;
		}
		if(wait)
		{
			if(pom != 0)
				wait = 0;
		}
	}while(wait);
	
	return(pom);
}

#endif		//KEYB