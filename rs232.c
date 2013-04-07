/*
 * rs232.c
 *
 *  Created on: 2013-1-9
 *      Author: Daniel
 */

#include <stdbool.h>
#include "rs232.h"

// 串口发送的环形缓冲区大小
#define	BUFFER_COUNT	10
#define PACKAGE_HEADER	0x55FEAAFE		// FE AA FE 55
#define PACKAGE_TAIL	0xFFFEFFFE		// FE FF FE FF

static struct struSendPackage	s_SendData[BUFFER_COUNT];

static unsigned char	s_ucSendBuffer[(32 + 1 + 1 + 4 + 4)];	// 临时发送缓冲区，数据最多32个字节，1个字节Node，1个字节长度，头尾各4字节
static unsigned char	s_ucReadPackage = 0;
static unsigned char	s_ucSendPos		= 0;	//Position in s_ucSendBuffer
static unsigned char	s_ucSendLength	= 0;	// 发送缓冲区长度
static unsigned char 	s_ucWritePackage = 0;

static bool s_bSending = false;

void InitUART()
{
	unsigned int		i;
	//UART1

	//9600 8-N-1
	//异步模式
	//禁止校验
	//一个停止位
	//字符长度8bit
	U1CTL = CHAR;

	U1TCTL = SSEL1;			//使用SMCLK，8M

	//8M下的9600
	U1BR0 = 0x41;
	U1BR1 = 0x03;
	U1MCTL = 0x92;

	//Init send & receive buffer.
	for (i = 0; i < BUFFER_COUNT; i++)
	{
		s_SendData[i].Node = -1;

		s_SendData[i].Length = 0;
	}

	//清除发送中断标志和接收中断标志
	IFG2 &= ~(UTXIFG1 + URXIFG1);

	//允许接收和发送中断
	ME2 |= UTXE1 + URXE1;
	IE2 |= UTXIE1 + URXIE1;

}

// 发送前，将数据编码（加头、加尾、加长度）
void rs232_encode()
{
	unsigned char		i = 0;

	// 包头标识
	s_ucSendBuffer[i++] = 0xFE;
	s_ucSendBuffer[i++] = 0xAA;
	s_ucSendBuffer[i++] = 0xFE;
	s_ucSendBuffer[i++] = 0x55;

	// 长度
	s_ucSendBuffer[i++] = s_SendData[s_ucReadPackage].Length + 2;		// 多增加两个字节，长度和Node编号

	// 编号
	s_ucSendBuffer[i++] = s_SendData[s_ucReadPackage].Node;

	memcpy(&s_ucSendBuffer[i], s_SendData[s_ucReadPackage].ucData, s_SendData[s_ucReadPackage].Length);
	i += s_SendData[s_ucReadPackage].Length;

	// 尾部标识
	s_ucSendBuffer[i++] = 0xFE;
	s_ucSendBuffer[i++] = 0xFF;
	s_ucSendBuffer[i++] = 0xFE;
	s_ucSendBuffer[i++] = 0xFF;

	s_ucSendLength = i;
}

void rs232_sendbegin()
{
	//send s_ucWritePackage
	rs232_encode();

	U1TXBUF = s_ucSendBuffer[0];
	s_ucSendPos = 1;	//next pos
}

void rs232_send(signed char Node, unsigned char ucLength, const unsigned char* pData)
{
	//check write pointer
	if (s_SendData[s_ucWritePackage].Node == -1)
	{
		//blank buffer
		s_SendData[s_ucWritePackage].Node 		= Node;
		s_SendData[s_ucWritePackage].Length		= ucLength;
		memcpy(s_SendData[s_ucWritePackage].ucData, pData, ucLength);

		//check whether begin send
		if (!s_bSending)
		{
			s_bSending = true;
			s_ucReadPackage = s_ucWritePackage;
			rs232_sendbegin();
		}

		s_ucWritePackage++;

		if (s_ucWritePackage >= BUFFER_COUNT)
		{
			//wrap
			s_ucWritePackage = 0;
		}
	}
	else
	{
		//overflow
	}
}



#pragma vector=USART1TX_VECTOR
__interrupt void UART1_TX_ISR()
{
	//continue send.
	if (s_ucSendLength == 0)
	{
		// 缓冲区没有数据发送
		s_bSending = false;
		return;
	}

	U1TXBUF = s_ucSendBuffer[s_ucSendPos];
	s_ucSendPos++;

	if (s_ucSendPos >= s_ucSendLength)
	{
		s_ucSendPos = 0;
		s_SendData[s_ucReadPackage].Node = -1;	//this package is send. mark it blank
		s_ucReadPackage++;
		if (s_ucReadPackage >= BUFFER_COUNT)
		{
			s_ucReadPackage = 0;
		}

		// 编码下一个
		if (s_SendData[s_ucReadPackage].Node != -1)
		{
			rs232_encode();
		}
		else
		{
			// 没有数据发送
			s_ucSendPos = 0;
			s_ucSendLength = 0;
		}
	}
}

#pragma vector=USART1RX_VECTOR
__interrupt void UART1_RX_ISR()
{
	//no control data now.
}



