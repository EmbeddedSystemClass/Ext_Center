/*
 * main.c
 */

#include <msp430.h>
#include "main.h"
#include "Macro.h"
#include "adc.h"
#include "timer.h"
#include "SPI.h"
#include "Sensors.h"
#include "nRF24L01P.h"
#include "Process.h"
#include "rs232.h"

void main(void)
{
  	Init();

	//switch to idle mode
	__low_power_mode_0();
	
	return;
}

//Initialize system
void Init()
{
	unsigned int		i;

	IFG1 &= ~OFIFG;

	// Stop watchdog timer to prevent time out reset
    WDTCTL = WDTPW + WDTHOLD;

	//Adjust DCO frequency
//    //DCOCTL	= DCO0 + DOC1 +DOC2;
//	DCOCTL	= 0x80;
//	BCSCTL1	= XT2OFF + RSEL0 + RSEL1 + RSEL2;
//	BCSCTL2	= 0;

	//调整DCO的频率
	DCOCTL = 0xE0;

	//切换MCLK到外部高速晶振(需要检测是否起振)
	//BCSCTL1 &= ~XT2OFF;		//启动高速晶振
	BCSCTL1 = RSEL0 + RSEL1 + RSEL2;

	do
	{
		//清除振荡器失效标志位，再延迟检查
		IFG1 &= ~OFIFG;
		for (i = 0; i < 1000; i++)
		{
			__no_operation();
		}
	} while (IFG1 & OFIFG);

	//使用XT2, SMCLK = MCLK = 8MHz
	BCSCTL2 = SELM1 + SELS;

	//允许外部振荡器失效后，重启系统
	IE1 |= OFIE;


	InitPort();

	InitUART();

	InitADC();

	InitSPI();

	InitRF();

	InitSensors();

	InitTimer();


	ShutdownModule();
}


void InitPort()
{
	//Default to output first.
	P1SEL = 0;
	P1DIR = 0xFF;
	P1OUT = 0;

	P2SEL = 0;
	P2DIR = 0xFF;
	P2OUT = 0;

	P3SEL = 0;
	P3DIR = 0xFF;
	P3OUT = 0;

	P4SEL = 0;
	P4DIR = 0xFF;
	P4OUT = 0;

	P5SEL = 0;
	P5DIR = 0xFF;
	P5OUT = 0;

	P6SEL = 0;
	P6DIR = 0xFF;
	P6OUT = 0;


	//LED inside box
	P3DIR |= BIT4;		//Output
	P3SEL &= ~BIT4;		//GPIO function
	TEST_LED_OFF;

	//Solar control Port
	P4DIR |= BIT1;
	P4SEL &= ~BIT1;
	P4OUT &= ~BIT1;		//output 0

	//default:turn on charge switcher
	SOLAR_ON;

	//LED control Port(4)
	P4DIR |= BIT2 + BIT3 + BIT4 + BIT5;
	P4SEL &= ~(BIT2 + BIT3 + BIT4 + BIT5);
	LAMP_OFF;

	//TEST
	//P4OUT |= (BIT2 + BIT3 + BIT4 + BIT5);
	//LAMP_ON; (don't user this. Otherwise, the state will be wrong.)
	LED_On();

	//Solar battery Voltage Port
	P6DIR &= ~BIT0;		//Input;
	P6SEL |= BIT0;		//ADC function.

	//12V PB Battery Voltage Port
	P6DIR &= ~BIT1;
	P6SEL |= BIT1;

	//Charge current Port
	P6DIR &= ~BIT2;
	P6SEL |= BIT2;

	//SPI(USART0 3 wire SPI Mode)(//nRF24L01+)
	P3SEL &= ~(BIT0 + BIT5);		//CSN and CE
	P3DIR |= BIT0 + BIT5;
	P3OUT &= ~BIT5;					//CE low
	P3OUT |= BIT0;					//CSN high

	P2SEL &= ~BIT7;					//IRQ
	P2DIR &= ~BIT7;
	P2IES |= BIT7;					//fall edge interrupt
	P2IE  |= BIT7;					//Enable P2.7 interrupt

	P3SEL |= BIT1 + BIT2 + BIT3;	//SPI(SIMO/SOMI/UCLK)


	//Sensor(SHT11 Temperature & humidity)
	P5SEL &= ~(BIT6 + BIT7);
	P5DIR |= BIT6;				//for SCK
	P5OUT &= ~BIT6;
	P5DIR |= BIT7;				//for DATA, thir-state first.

	//RS232
	P3SEL |= BIT6 + BIT7;
	P3DIR |= BIT6;				//P3.6 is TX, output
	P3DIR &= ~BIT7;				//P3.7 is RX, input
}



//shutdown the module which don't work, to save power.
void ShutdownModule()
{
}


// 没有使用的中断（如果出现，则复位）
#pragma vector=ADC12_VECTOR, COMPARATORA_VECTOR, NMI_VECTOR, PORT1_VECTOR, TIMERB0_VECTOR, TIMERB1_VECTOR, USART0RX_VECTOR, USART0TX_VECTOR, WDT_VECTOR
__interrupt void ISR_trap(void)
{
	// the following will cause an access violation which results in a PUC reset
	WDTCTL = 0;
}

