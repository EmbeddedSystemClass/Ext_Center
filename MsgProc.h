/*
 * MsgProc.h
 *
 *  Created on: 2013-1-20
 *      Author: Daniel
 */

#ifndef MSGPROC_H_
#define MSGPROC_H_

// 消息ID（无线消息）
#define		MSGID_MAIN_INFO				1			// 主分机消息（带大气压）
//struct	struSolarCtrl
//{
//	unsigned int	unSerial;			// 序号
//
//	unsigned char	ucMsgID;			// 消息ID
//	unsigned char	ucState;			// 其他状态
//
//	int				nSolar;				// 太阳能电压
//	int				nBattery;			// 电池电压
//	int				nCharge;			// 充电电流
//	int				nTemperature;		// 温度
//	int				nHumidity;			// 湿度
//	int				nPressure;			// 大气压（10Pa）
//	int				nTemperature_BMP085;// 温度（0.1摄氏度）
//};

#define		MSGID_EXT_W_HUMIDITY		2			// 普通分机消息（带湿度，和温度）

#define		MSGID_EXT_INFO				3			// 普通分机消息（不带湿度，只有温度。DS18B02）


#endif /* MSGPROC_H_ */
