/*
 * rs232.h
 *
 *  Created on: 2013-1-9
 *      Author: Daniel
 */

#ifndef RS232_H_
#define RS232_H_

#include <msp430.h>
#include "nRF24L01P.h"

//need add Package header and tail.
struct struSendPackage
{
	unsigned char			Length;		// 长度指ucData的长度
	signed char				Node;		// 分机编号

	unsigned char			ucData[32];	// 数据	（第一个字节是消息ID）
};


void InitUART();

void rs232_sendbegin();
void rs232_send(signed char Node, unsigned char ucLength, const unsigned char* pData);

#endif /* RS232_H_ */
