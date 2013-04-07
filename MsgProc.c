/*
 * MsgProc.c
 *
 *  Created on: 2013-1-20
 *      Author: Daniel
 *
 *      消息处理函数，解码无线消息，转发到串口。
 *      或将串口消息转换为无线消息
 */

#include "MsgProc.h"
#include "rs232.h"

void Decode_Main_Info(signed char Node, unsigned char* pData)
{

}

void Decode_Ext_W_Humidity()
{

}

void Decode_Ext_Info()
{

}

// 解码
void DecodeMsg(signed char Node, unsigned char* pData)
{
	// 消息至少3个字节或以上，第3个字节是消息ID
}


