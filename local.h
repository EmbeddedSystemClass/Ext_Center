/*
 * local.h
 *
 *  Created on: 2013-1-8
 *      Author: Daniel
 */

#ifndef LOCAL_H_
#define LOCAL_H_

#include <msp430.h>

void SendLocalData();

// 消息ID（无线消息）
#define		MSGID_CENTER_INFO				2			// 中心分机消息（温度、湿度，没有大气压）

struct struCenterInfo
{
	unsigned int	unSerial;			// 序号
	unsigned char	ucMsgID;			// 消息ID
	unsigned char	ucState;			// 其他状态

	int				nSolar;				// 太阳能电压
	int				nBattery;			// 电池电压
	int				nCharge;			// 充电电流
	int				nTemperature;		// 温度
	int				nHumidity;			// 湿度
};

#endif /* LOCAL_H_ */
