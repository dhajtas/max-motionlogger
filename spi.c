/*! \file spi.c \brief SPI interface driver. */
//*****************************************************************************
//
// File Name	: 'spi.c'
// Title		: SPI interface driver
// Author		: Pascal Stang - Copyright (C) 2000-2002
// Created		: 11/22/2000
// Revised		: 06/06/2002
// Version		: 0.6
// Target MCU	: Atmel AVR series
// Editor Tabs	: 4
//
// NOTE: This code is currently below version 1.0, and therefore is considered
// to be lacking in some functionality or documentation, or may not be fully
// tested.  Nonetheless, you can expect most functions to work.
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//
//*****************************************************************************

#include <avr/io.h>
#include <avr/interrupt.h>

#include "hw.h"
#include "spi.h"

// Define the SPI_USEINT key if you want SPI bus operation to be
// interrupt-driven.  The primary reason for not using SPI in
// interrupt-driven mode is if the SPI send/transfer commands
// will be used from within some other interrupt service routine
// or if interrupts might be globally turned off due to of other
// aspects of your program
//
// Comment-out or uncomment this line as necessary
//#define SPI_USEINT

// global variables

#ifdef SPI_USEINT
volatile uint8_t spiTransferComplete;
#endif


// SPI interrupt service handler
//#ifdef SPI_USEINT
//test
//ISR(SPI_STC_vect)
//{
//	spiTransferComplete = TRUE;
//}

//ISR(USART_RX_vect)
//{
//	spi1TransferComplete = TRUE;
//}

//#endif

// access routines

void SPI_Init(uint8_t sd_lis)
{
	uint8_t temp;
	
	switch(sd_lis)
	{
		case SPI_SD:
						PRR &= ~(_BV(PRSPI));						// enable SPI clk tree
						SPI0_DDR |= (_BV(SPI0_SCK)|_BV(SPI0_MOSI)|_BV(SPI0_SS));	//set outputs
						SPI0_DDR &= ~(_BV(SPI0_MISO));								//set input
						SPI0_PORT |= _BV(SPI0_SS);				//set ss Hi; pullup on MISO??? 
						SPI0_PORT &= ~(_BV(SPI0_MISO));		//Hiz on MISO
						//SPI clk 62kHz
						SPCR = (_BV(MSTR)|_BV(SPE)|_BV(SPR0));		//|_BV(CPOL)|_BV(CPHA));
						temp = SPSR;						// clear status
						temp = SPDR;
						break;
	
/* setup SPI interface :
	SPR0 = 1	
	SPR1 = 0	// fclk/16
	CPHA = 1?	// rising edge sample, falling edge setup  	was 0 	for LIS3LV 1???
	CPOL = 1?	// sck is HI when idle						was 0	for LIS3LV 1???
	MSTR = 1	// master
	DORD = 0	// MSB first
	SPE  = 1	// SPI enable		*/
		case SPI_LIS:
#if BIGAVR == 0							//in real circ 2 SPI are used!!
						PRR &= ~(_BV(PRUSART0));					// enable USART clk tree
						UBRR0 = 0;	
						SPI1_DDR |= (_BV(SPI1_SCK));				// Setting the XCKn port pin as output, enables master mode. 
						UCSR0C = _BV(UMSEL01)|_BV(UMSEL00);		// Set MSPI mode of operation and SPI data mode 0. 
						UCSR0B = _BV(RXEN0)|_BV(TXEN0);			// Enable receiver and transmitter. 
						UBRR0 = 3;
						break;
																	// Set baud rate. 
																	// IMPORTANT: The Baud Rate must be set after the transmitter is enabled
		case SPI_LIS_OFF:
						UCSR0B &= ~(_BV(RXEN0)|_BV(TXEN0));		// disble uart
						PRR |= _BV(PRUSART0);						// disable uart clk tree
						break;
		case SPI_SD_OFF:
						SPCR &= ~(_BV(SPE));			//disable SPI
						SPI0_DDR |= _BV(SPI0_SCK)|_BV(SPI0_MOSI)|_BV(SPI0_SS);			//set as outputs
						SPI0_PORT &= ~(_BV(SPI0_SS)|_BV(SPI0_MISO)|_BV(SPI0_SCK)|_BV(SPI0_MOSI));	//set L all
						PRR |= _BV(PRSPI);				//disable SPI clk tree
						break;
						
	}

	

	#ifdef SPI_USEINT
//		spi1TransferComplete = TRUE;			// enable USART SPI RX interrupt
//		UCSR0B |= _BV(RXCIE0);
	#endif
#endif			//BIGAVR

	#ifdef SPI_USEINT
//		spiTransferComplete = TRUE;				// enable SPI interrupt
//		SPCR |= _BV(SPIE);
	#endif
}


void SPI_Switch(uint8_t data)
{
//#if BIGAVR == 1	
//	uint8_t temp;

//	SPCR &= ~(_BV(SPIE));
//	if (data == SPI_LIS)
//		SPCR = SPCR | (_BV(CPHA)|_BV(CPOL));
//	else
//		SPCR = SPCR & ~(_BV(CPHA)|_BV(CPOL));
//	temp = SPSR;							// clear status

//	#ifdef SPI_USEINT
//		spiTransferComplete = TRUE;			// enable SPI interrupt
//		SPCR |= _BV(SPIE);
//	#endif
//#else

	UCSR0B &= ~(_BV(RXEN0)|_BV(TXEN0)|_BV(RXCIE0));	// Disable receiver, transmitter and interrupt. 
	
	if(data == SPI_LIS)
		UCSR0C = _BV(UMSEL01)|_BV(UMSEL00)|_BV(UCPHA0)|_BV(UCPOL0);		// Set MSPI mode of operation and SPI data mode 3. 
	else
		UCSR0C = _BV(UMSEL01)|_BV(UMSEL00);		// Set MSPI mode of operation and SPI data mode 0. 

	UCSR0B = _BV(RXEN0)|_BV(TXEN0);			// Enable receiver and transmitter. 

//	#ifdef SPI_USEINT
//		spi1TransferComplete = TRUE;				// enable SPI interrupt
//		UCSR0B |= _BV(RXCIE0);
//	#endif

//#endif		//BIGAVR

}


void SPI_Send8(uint8_t data, uint8_t port)
{
	uint8_t dump;
	if(port)
	{
		UDR0 = data;
//		#ifdef SPI_USEINT
//			while(!spi1TransferComplete);	// send a byte over SPI and ignore reply
//			spi1TransferComplete = FALSE;
//		#else
			while(!(UCSR0A & _BV(RXC0)));
//		#endif
		dump = UDR0;
	}
	else
	{
		SPDR = data;
//		#ifdef SPI_USEINT
//			while(!spiTransferComplete);	// send a byte over SPI and ignore reply
//			spiTransferComplete = FALSE;
//		#else
			while(!(SPSR & _BV(SPIF)));
//		#endif
		dump = SPDR;
	}
}


uint8_t SPI_Receive8(uint8_t port)
{
	if(port)
	{
		UDR0 = 0xff;
	
//		#ifdef SPI_USEINT
//			while(!spiTransferComplete);	// send a byte over SPI and ignore reply
//		#else
			while(!(UCSR0A & _BV(RXC0)));
//		#endif
		return(UDR0);
	}
	else
	{
		SPDR = 0xff;

//		#ifdef SPI_USEINT
//			while(!spiTransferComplete);	// send a byte over SPI and ignore reply
//		#else
			while(!(SPSR & _BV(SPIF)));
//		#endif
		return(SPDR);
	}
}


uint8_t SPI_Transfer8(uint8_t data, uint8_t port)
{
	if(port)
	{
//		#ifdef SPI_USEINT
//			spi1TransferComplete = FALSE;		// send the given data
//			UDR0 = data;
//			while(!spi1TransferComplete);		// wait for transfer to complete
//		#else
			UDR0 = data;						// send the given data
			while(!(UCSR0A & _BV(RXC0)));		// wait for transfer to complete
//		#endif
		return(UDR0);							// return the received data
	}
	else
	{
//		#ifdef SPI_USEINT
//			spiTransferComplete = FALSE;		// send the given data
//			SPDR = data;
//			while(!spiTransferComplete);		// wait for transfer to complete
//		#else
			SPDR = data;						// send the given data
			while(!(SPSR & _BV(SPIF)));		// wait for transfer to complete
//		#endif
		return(SPDR);						// return the received data
	}
}


uint16_t SPI_Transfer16(uint16_t data, uint8_t endian, uint8_t port)
{
	uint16_t rxData = 0;
	if(endian == BE)
	{
		rxData = (SPI_Transfer8((uint8_t)(data>>8), port))<<8;	// send MS byte of given data
		rxData |= (SPI_Transfer8((uint8_t)data, port));			// send LS byte of given data
	}
	else
	{
		rxData = SPI_Transfer8((uint8_t)data, port);				// send LS byte of given data
		rxData |= (SPI_Transfer8((uint8_t)(data>>8), port))<<8;	// send MS byte of given data
	}

	return(rxData);										// return the received data
}

