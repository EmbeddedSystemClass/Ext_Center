/*
 * Sensors.c
 *
 *  Created on: 2013-1-9
 *      Author: Daniel
 */

#include "Sensors.h"

#define DELAY	__delay_cycles(40)

#define SHT11_SCK_1		P5OUT |= BIT6
#define SHT11_SCK_0 	P5OUT &= ~BIT6

#define SHT11_DATA_1	P5OUT |= BIT7
#define SHT11_DATA_0	P5OUT &= ~BIT7
#define SHT11_DATA_IN	P5DIR &= ~BIT7
#define SHT11_DATA_OUT	P5DIR |= BIT7

#define SHT11_DATA_READ	(P5IN & BIT7)


//Unprocess Temperature
int g_nTemperature	= 0;
//Unprocess Humidity
int g_nHumidity		= 0;

static unsigned char s_ucCRC;

void InitSensors()
{
}

void Init_SHT11()
{
}

void StartCommand()
{
	DELAY;
	SHT11_DATA_OUT;
	SHT11_DATA_1;

	SHT11_SCK_1;
	DELAY;
	SHT11_DATA_0;
	DELAY;
	SHT11_SCK_0;
	DELAY;
	SHT11_SCK_1;
	DELAY;
	SHT11_DATA_1;
	DELAY;
	SHT11_SCK_0;
	DELAY;
}

void SendByte(unsigned char data)
{
	int				i;
	unsigned char	bit = 0x80;

	for (i = 0; i < 8; i++)
	{
		DELAY;

		//prepare data;
		if (data & bit)
		{
			//SHT11_DATA_1;
			SHT11_DATA_IN;		//pull up by 10K resistor
		}
		else
		{
			SHT11_DATA_OUT;
			SHT11_DATA_0;
		}

		//raise SCK
		SHT11_SCK_1;
		DELAY;
		SHT11_SCK_0;

		bit = bit >> 1;
	}

	//read ACK
	DELAY;
	SHT11_SCK_1;
	//TODO:check ACK, data is low.
	DELAY;
	SHT11_SCK_0;
}

unsigned char ReadByte(bool isCRC)
{
	unsigned char	data;
	int				i;

	for (i = 0; i < 8; i++)
	{
		DELAY;
		SHT11_SCK_1;

		data = data << 1;
		//read data
		data += (P5IN & BIT7) ? 0x01 : 0x00;

		DELAY;
		SHT11_SCK_0;
	}

	//confirm ACK
	DELAY;
	if (!(isCRC))
	{
		SHT11_DATA_OUT;
		SHT11_DATA_0;
	}
	SHT11_SCK_1;
	DELAY;
	SHT11_SCK_0;

	SHT11_DATA_IN;

	return data;
}

void WaitResult()
{
	while (1)
	{
		DELAY;
		//check DATA is low.
		if (!(SHT11_DATA_READ))
		{
			break;
		}
	}
}

void StartTemperature()
{
	StartCommand();
	SendByte(3);		//Measure Temperature 00011
}

bool GetTemperature()
{
	//check DATA is low.
	if (SHT11_DATA_READ)
	{
		//still high, measuring...
		return false;
	}

	g_nTemperature = ReadByte(false);
	g_nTemperature = g_nTemperature << 8;
	g_nTemperature += ReadByte(false);
	s_ucCRC	= ReadByte(true);

	return true;
}


void StartHumidity()
{
	StartCommand();
	SendByte(5);		//Measure Relative Humidity 00101
}

bool GetHumidity()
{
	//check DATA is low.
	if (SHT11_DATA_READ)
	{
		//still high, measuring...
		return false;
	}

	g_nHumidity = ReadByte(false);
	g_nHumidity = g_nHumidity << 8;
	g_nHumidity += ReadByte(false);
	s_ucCRC	= ReadByte(true);

	return true;
}

//static const float C1 = -2.0468; 	// for 12 Bit
//static const float C2 = +0.0367; 	// for 12 Bit
//static const float C3 = -1.5955e6; 	// for 12 Bit
//
//static const float D1 = -39.7; // for 14 Bit @ 3.5V
//static const float D2 = +0.01; // for 14 Bit @ 3.5V
//
//static const float T1 = 0.01; 	// for 12 bit
//static const float T2 = 0.00008; 	// for 12 bit
//
//// 将温度转换为可读的实际数据（返回0.1度倍数，即返回值放大了10倍）
//int CalculateTemperature(unsigned int temperature)
//{
//	float t_C; 		// t_C : Temperature
//
//	t_C = temperature * D2 + D1;
//	t_C *= 10;		// 放大10倍后取整
//
//	return (int)t_C;
//}
//
//// 获取真实的湿度数据（需要温度值参考）（返回0.1倍数，返回值放大了10倍）
//int CalculateHumidity(unsigned int humidity, int temperature)
//{
//	float rh_lin; 	// rh_lin: Humidity linear
//	float rh_true; 	// rh_true: Temperature compensated humidity
//
//	rh_lin = C3 * humidity * humidity + C2 * humidity + C1; //calc. humidity from ticks to [%RH]
//	rh_true = (temperature - 250) * (T1+T2 * humidity) + rh_lin * 10; //calc. temperature compensated humidity [%RH]（一起放大10倍）
//	if (rh_true > 1000) rh_true = 1000; //cut if the value is outside of
//	if(rh_true < 1) rh_true = 1; //the physical possible range
//
//	return (int)rh_true;
//}





