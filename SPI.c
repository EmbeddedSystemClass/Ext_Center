/*
 * SPI.c
 *
 *  Created on: 2013-1-9
 *      Author: Daniel
 */


#include "SPI.h"

#define RF_CSN_0		P3OUT &= ~BIT0
#define RF_CSN_1		P3OUT |= BIT0

#define WAIT_RXIFG	do{	__delay_cycles(10);} while (!(IFG1 & URXIFG0))
//#define WAIT_TXIFG	do{	__delay_cycles(10);} while (!(IFG1 & UTXIFG0))


void InitSPI()
{
	//use USART0
	U0CTL	|= SWRST;
	U0CTL 	= CHAR + SYNC + MM + SWRST;		//8bit SPI Master
	U0TCTL	= SSEL1 + STC;
	U0RCTL	= 0;
	U0BR1	= 0;
	U0BR0	= 4;							//8MHz / 4, about 2MHz for UCLK

	//Unused
	U0MCTL	= 0;

	ME1		|= USPIE0;						//enable USART0

	//release rest
	U0CTL	&= ~SWRST;
}

//Read from SPI(write 1 byte command first)
//return status
unsigned char SPIRead(unsigned char ucCommand, unsigned char* pData, unsigned char ucLength)
{
	unsigned char	i;
	unsigned char	ucStatus;

	//read RXBUF first, to reset the RXIFG which is set by last operation.
	ucStatus = U0RXBUF;

	//begin
	RF_CSN_0;

	//command and read status
	U0TXBUF = ucCommand;
	WAIT_RXIFG;
	ucStatus = U0RXBUF;

	for (i = 0; i < ucLength; i++)
	{
		U0TXBUF = 0xFF;
		WAIT_RXIFG;
		pData[i] = U0RXBUF;
	}

	//end
	RF_CSN_1;

	return ucStatus;
}

//Write to SPI, pData include ucCommand+Data.
unsigned char SPIWrite(unsigned char ucCommand, unsigned char* pData, unsigned char ucLength)
{
	unsigned char	i;
	volatile unsigned char	ucTemp;
	unsigned char	ucStatus;

	//read RXBUF first, to reset the RXIFG which is set by last operation.
	ucTemp = U0RXBUF;

	//begin
	RF_CSN_0;

	//command and read status
	U0TXBUF = ucCommand;
	WAIT_RXIFG;
	ucStatus = U0RXBUF;

	for (i = 0; i < ucLength; i++)
	{
		U0TXBUF = pData[i];
		WAIT_RXIFG;
		ucTemp = U0RXBUF;
	}

	//end
	RF_CSN_1;

	return ucStatus;
}

