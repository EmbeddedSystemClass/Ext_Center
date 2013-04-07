/*
 * local.c
 *
 *  Created on: 2013-1-8
 *      Author: Daniel
 *
 *      发送本地传感器数据（到ARM linux系统，本地编码为6）
 */


#include "local.h"
#include "nRF24L01P.h"
#include "rs232.h"
#include "Process.h"
#include "adc.h"
#include "Sensors.h"

static struct struCenterInfo		s_LocalData;

#define LOCAL_PIPE		6		//because nRF24L01 have 6 pipe(0~5)
void SendLocalData()
{
	//prepare local data
	s_LocalData.unSerial		= 0;
	s_LocalData.ucMsgID			= MSGID_CENTER_INFO;
	s_LocalData.ucState			= g_ucState;

	s_LocalData.nSolar			= g_nSolar;
	s_LocalData.nBattery 		= g_nBattery;
	s_LocalData.nCharge			= g_nCharge;
	s_LocalData.nTemperature	= g_nTemperature;
	s_LocalData.nHumidity		= g_nHumidity;

	rs232_send(LOCAL_PIPE, sizeof(s_LocalData), (unsigned char*)&s_LocalData);
}


