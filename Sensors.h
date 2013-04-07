/*
 * Sensors.h
 *
 *  Created on: 2013-1-9
 *      Author: Daniel
 */

#ifndef SENSORS_H_
#define SENSORS_H_

#include <msp430.h>
#include <stdbool.h>

void InitSensors();
void Init_SHT11();

void StartTemperature();
bool GetTemperature();

void StartHumidity();
bool GetHumidity();

int CalculateTemperature(unsigned int temperature);
int CalculateHumidity(unsigned int humidity, int temperature);


//Unprocess Temperature
extern int g_nTemperature;
//Unprocess Humidity
extern int g_nHumidity;

#endif /* SENSORS_H_ */
