/*
 * nRF24L01P.c
 *
 *  Created on: 2013-1-8
 *      Author: Daniel
 */


#include "nRF24L01P.h"
#include "SPI.h"
#include "Process.h"
#include "adc.h"
#include "Sensors.h"
#include "rs232.h"

static unsigned char		s_ucRecvData[32] = {0};

#define CE_0		P3OUT &= ~BIT5
#define CE_1		P3OUT |= BIT5

//initialize nRF24L01+
void InitRF()
{
	unsigned char	ucData[6] = {0};

	//CONFIG(all interrupt, CRC 2 bytes, TX mode)
	ucData[0] = EN_CRC + CRCO + PRIM_RX + PWR_UP;
	SPIWrite((WRITE_REG + CONFIG), ucData, 1);

	//SETUP_AW(default 5bytes)

	//SETUP_RETR(2000us * 6 times)	250kbps
	ucData[0] = ((7<<4) + 6);
	SPIWrite((WRITE_REG + SETUP_RETR), ucData, 1);

	//RF_CH(Debug:70, Release:70)
#ifdef NDEBUG
	ucData[0] = 70;
#else
	ucData[0] = 70;
#endif
	SPIWrite((WRITE_REG + RF_CH), ucData, 1);

	//RF_SETUP(250kbps)
	ucData[0] = RF_PWR1 + RF_PWR0 + RF_DR_LOW;
	SPIWrite((WRITE_REG + RF_SETUP), ucData, 1);

	//Address(0x3B4B5B6B 01)
	//RX_ADDR_P0, TX_ADDR
	ucData[0] = 0x01;
	ucData[1] = 0x6B;
	ucData[2] = 0x5B;
	ucData[3] = 0x4B;
	ucData[4] = 0x3B;

	SPIWrite(WRITE_REG + RX_ADDR_P0, ucData, 5);
	SPIWrite(WRITE_REG + TX_ADDR, ucData, 5);

	//RX_ADDR_P1(0x3B4B5B6B 12)
	ucData[0] = 0x12;
	SPIWrite(WRITE_REG + RX_ADDR_P1, ucData, 5);

	//RX_ADDR_P2(0x3B4B5B6B 23)
	ucData[0] = 0x23;
	SPIWrite(WRITE_REG + RX_ADDR_P2, ucData, 1);

	//RX_ADDR_P3(0x3B4B5B6B 34)
	ucData[0] = 0x34;
	SPIWrite(WRITE_REG + RX_ADDR_P3, ucData, 1);

	//RX_ADDR_P4(0x3B4B5B6B 45)
	ucData[0] = 0x45;
	SPIWrite(WRITE_REG + RX_ADDR_P4, ucData, 1);

	//RX_ADDR_P5(0x3B4B5B6B 56)
	ucData[0] = 0x56;
	SPIWrite(WRITE_REG + RX_ADDR_P5, ucData, 1);

	//EN_RXADDR(Enable all pipes)
	ucData[0] = BIT0 + BIT1 + BIT2 + BIT3 + BIT4 + BIT5;
	SPIWrite(WRITE_REG + EN_RXADDR, ucData, 1);


	//DYNPD(P0,P1,P2,P3,P4,P5 DPL)
	ucData[0] = BIT0 + BIT1 + BIT2 + BIT3 + BIT4 + BIT5;
	SPIWrite(WRITE_REG + DYNPD, ucData, 1);


	//FEATURE(EN_DPL, EN_ACK_PAY)
	ucData[0] = EN_DPL + EN_ACK_PAY;
	SPIWrite(WRITE_REG + FEATURE, ucData, 1);

	//if warm reset, then need clean TX/RX buffer
	SPIWrite(FLUSH_RX, 0, 0);
	SPIWrite(FLUSH_RX, 0, 0);
	SPIWrite(FLUSH_RX, 0, 0);

	SPIWrite(FLUSH_TX, 0, 0);
	SPIWrite(FLUSH_TX, 0, 0);
	ucData[0] = SPIWrite(FLUSH_TX, 0, 0);

	//clean IRQ on nRF24L01+
	ucData[0] =0x70;
	SPIWrite(WRITE_REG + STATUS, ucData, 1);

	//reset ifg
	P2IFG &= ~BIT7;

	CE_1;
}


//nRF24L01+ ISR  Port2.7
#pragma vector=PORT2_VECTOR
__interrupt void Port2_ISR()
{
	//check whether nRF24L01+ interrupt
	if (P2IFG & BIT7)
	{
		P2IFG &= ~BIT7;	//clear IFG
		nRF_ISR();
	}
}

void nRF_ISR()
{
	//read status
	unsigned char	ucStatus;

	ucStatus = SPIRead(NRF_NOP, 0, 0);

	if (ucStatus & MAX_RT)
	{
		//Send failed, need flush_tx
		nRF_ISR_MAXRT(ucStatus);
	}

	if (ucStatus & TX_DS)
	{
		//Send success
		nRF_ISR_DS(ucStatus);
	}

	if (ucStatus & RX_DR)
	{
		//have ACK payload
		nRF_ISR_DR(ucStatus);
	}

	//clean IRQ on nRF24L01+
	ucStatus = 0x70;
	SPIWrite(WRITE_REG + STATUS, &ucStatus, 1);
}

void nRF_ISR_MAXRT(unsigned char ucStatus)
{
	//clear the data which was sent failed.
	SPIWrite(FLUSH_TX, 0, 0);
}

void nRF_ISR_DS(unsigned char ucStatus)
{
}

void nRF_ISR_DR(unsigned char ucStatus)
{
	do
	{
		ReadData();
		ucStatus = SPIRead(NRF_NOP, 0, 0);
	} while (!((ucStatus & RX_P_NO) == RX_P_NO));
}

unsigned char ReadData()
{
	unsigned char	ucLength;
	unsigned char 	ucStatus;
	unsigned char	ucPipe;

	//Read payload length
	SPIRead(RD_RX_PLOAD_W, &ucLength, 1);

	//Read payload
	if (ucLength > 32)
	{
		//wrong payload
		ucStatus = SPIWrite(FLUSH_RX, 0, 0);
	}
	else
	{
		// 长度不大于32，即认为合法
		ucStatus = SPIRead(RD_RX_PLOAD, s_ucRecvData, ucLength);

		//pipe number
		ucPipe = (ucStatus & RX_P_NO) >> 1;

		rs232_send(ucPipe, ucLength, s_ucRecvData);
	}

	return ucStatus;
}


