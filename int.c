#include <avr/io.h>
#include <avr/signal.h>
#include <avr/interrupt.h>
#include <inttypes.h>

#include "fgen.h"
#include "int.h"
#include "driver_FGEN.h"
#include "driver_KEYB.h"
//#include "myeeprom.h"
//#include "rsc0.h"
#include "init.h"


SIGNAL(SIG_ADC)			//End of AD conversion interrupt
{
//	stat_MEAS |= 0x20;	//ADC skoncene						MEAS.5 = 1
}



/*
SIGNAL(SIG_INTERRUPT0)		// Activity on RX interrupt
{				
	EIMSK &= 0xFE;		//zachovaj ostatne INT, vypni int0
	stat_TX |= 0x20;	//aktivita na rx - statUTX.5 = 1 zmenit vsetky stat byty na 1 strukturu...
	my_delay(2,0xC3);
	while(K);
	stat_TX &= 0xEF;	//pretoze dojde k nastaveniu v T2 				TX.4 = 0
	stat_WD |= 0x04;	//Ser_com_Init();						WD.2 = 1
}
*/
//-----------------------------------------------------------------------------------

EMPTY_INTERRUPT(SIG_SPI);

//-----------------------------------------------------------------------------------

SIGNAL(SIG_INTERRUPT6)	// PLL lock - FGEN1
{
	if (PINE & 0x40)
		FGEN[1].lock = 0;
	else
		FGEN[1].lock = 1;
	sei();
}

//-------------------------------------------------------------------------------------

SIGNAL(SIG_INTERRUPT7)	// PLL lock - FGEN0
{
	if (PINE & 0x40)
		FGEN[0].lock = 0;
	else
		FGEN[0].lock = 1;
	sei();
}

//-------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------


SIGNAL(SIG_OUTPUT_COMPARE3A)	//generovanie 2. clocku, ak 1. mal > f
{
	uint8_t cnt;
	
	if(cnt == cntb)
	{
//		TCCR3C = 0x40;			//force compare match for B channel
		if(PORTE & 0x10)
			PORTE & 0xEF;
		else
			PORTE | 0x10;
		cnt=0;
	}
	cnt++;
	sei();
}

//------------------------------------------------------------------------------------------

SIGNAL(SIG_OUTPUT_COMPARE3B)	//generovanie 1. clocku, ak 2. mal > f
{
	uint8_t cnt;
	
	if(cnt == cnta)
	{
//		TCCR3C = 0x80;			//force compare match for A channel
		if(PORTE & 0x08)
			PORTE & 0xF7;
		else
			PORTE | 0x08;
		cnt=0;
	}
	cnt++;
	sei();
}

//----------------------------------------------------------------------------

/*
	This SIGNAL is maitained by the RTOS
*/
//SIGNAL(SIG_OVERFLOW0)		//tc0 8bit  System tick
//{
//	if(tick==0x01)
//	{
//		if(!(key))
//		{
//			key = KEYB_scan();
//		}
//	}
/*
	if(!(stat_TX & 0x20))	// ak nie je aktivita na linke				!TX.5?
	{
	 if(stat_STAT & 0x20)	// ak je sleep enabled					STAT.5?
	 {
	   stat_RX &= 0xFE;		// vypni komunikaciu					RX.0 = 0
	 }
	 else
	 {
	  if(stat_RX & 0x02)	// Ak nebola dokoncena aktivna komunikacia		RX.1?
	   stat_WD |= 0x04;		// WD.2 = 1
	 }
	}	
*/	
//	keyb_char = KEYB_scan();	// scan tlacitok
//	tick++;
//	if(tick > 20)
//		tick = 0;
//	sei();
//}

//---------------------------------------------------------------------------------

#ifdef	RSCOM0_H
SIGNAL(SIG_UART0_TRANS)		//utxc
{
	stat_TX |= 0x04;	// do buducna vyriesit softwareovo
				//				TX.2 = 1
	sei();
}				// netestovat stat.TX ale priamo priznak...

//-------------------------------------------------------------------------------------

SIGNAL(SIG_UART0_RECV)		//_isrURXC
{
 static uint8_t transfer;
 static uint8_t cnt;
 register uint8_t pom;
 static uint8_t *checksum;

 stat_TX |= 0x20;		//aktivita na linke					TX.5 = 1
 
 if(!(stat_RX & 0x02))		//Aktivna komunikacia?					?!RX.1
 {				//NO
  if(!(stat_RX & 0x04))		//wake-up byte away?					!RX.2?
  {					//not
	 if(UDR0 != 0x55)		//flush UDR (wakeup byte) if not 0x55
	 	stat_TX |= 0x08;	//set out of synchro				TX.3 = 1
	 else
	 {
	 	stat_RX |= 0x04;	//set wake-up byte away				RX.2 = 1
		cnt = 0;
	 }
  } 
  else					//wake-up byte is away 
  {
  	 switch(cnt)
  	 {
  	 	case 0: *checksum = 0;
  	 		pom = receive(checksum);
			if(pom == 0x00)
			 stat_RX = stat_RX | 0x0A;
			if(pom == LocalID)
			{
			 stat_RX |= 0x02;	//				 	RX.1 = 1
			 stat_RX &= 0xF7;	//					RX.3 = 0
			}
			transfer = 0;
			cnt++;
			break;
		case 1:	transfer = receive(checksum);
			if(transfer < 0x0B)	//if transfer < 11
			{
			 stat_TX &= 0xF7;	//clear out of synchro			TX.3 = 0
			 transfer++;		//aj checksum has to be flushed
			}
			else
			 stat_RX &= 0xFB;	//else wake-up byte is not away		RX.2 = 0
			cnt++;
			break;
		case 2:	inp(UDR0);		//flush UDR
			transfer--;
			if(transfer == 0x00)
			 stat_RX &= 0xFB;	//Wake-up byte is not away - next packet RX.2 = 0
			break;
	}
  }
 }
 else
 {					//YES
 	if(transfer == 0)
 	{
 	 	if(cnt == 1)
 	 	{
 	 		transfer = receive(checksum);
 	 		cnt++;
  	 	}
 	 	else
 	 	{
 	 		receive(checksum);
 	 		stat_RX |= 0x40;		//paket prijaty			RX.6 = 1
 	 		stat_RX &= 0xF9;		//cakaj na dalsi wake-up byte 	RX.2 = 0
	 						//neaktivna komm!		RX.1 = 0
 	 		if(checksum)
 	 		{
 	 		 stat_RX |= 0x80;		//zly checksum			RX.7 = 1
			}
 	 	}
 	 }
 	 else
 	 {
		transfer--;
		uart_buffer[uart_tail] = receive(checksum);
		uart_tail++;
		uart_tail &= 0x1F;
		stat_RX &= 0xDF;
		if(uart_head == uart_tail)
		{
			uart_head++;
			uart_head &= 0x1F;		// rotuj pointer na head na 32 bytov
			stat_RX |= 0x10;		// buffer overrun		RX.4 = 1
		}
	 }
 }
 sei();
}

/*
===============================================================================================
					rutiny		 	 
===============================================================================================
*/

uint8_t receive(register uint8_t *checksum)
{
 uint8_t data;
 
	if(bit_is_set(UCSR0A,4) || bit_is_set(UCSR0A,3))	//Frame error during comm? or overrun of the UDR during comm?
		stat_RX |= 0x80;				//error 		RX.7 = 1
	data = UDR0;						//read byte from UDR
	*checksum += data;
	return(data);
}
#endif
