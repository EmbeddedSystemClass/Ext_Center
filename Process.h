/*
 * Process.h
 *
 *  Created on: 2013-1-8
 *      Author: Daniel
 */

#ifndef PROCESS_H_
#define PROCESS_H_

#include <msp430.h>
#include <stdbool.h>

void Process();

void StartMeasure();

void SolarOn();
void SolarOff();

void LED_On();
void LED_off();

void CheckVoltage();

bool TestLED_Proc();

void TestLED_MeasureFailed();
void TestLED_MeasureSuccess();
void TestLED_SendSuccess();
void TestLED_SendFailed();

extern unsigned char			g_ucState;

#endif /* PROCESS_H_ */
