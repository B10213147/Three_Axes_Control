/*
 * touchpanel_driver.c
 *
 *  Created on: Oct 12, 2016
 *      Author: Harvard Tseng
 */

#include "touchpanel_driver.h"
#include "rtos.h"
#include "TM4C123GH6PM.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"

void data_valid(void);

void touchpanel_driver(void){
	GPIOIntEnable(GPIOD_BASE, GPIO_INT_PIN_1);
}

void touchpanel_driver_init(void){
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);

	//
	// Check if the peripheral access is enabled.
	//
	while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOD))
	{
	}

	// SCL
	GPIOPinTypeGPIOOutput(GPIOD_BASE, GPIO_PIN_0);
	GPIOPinWrite(GPIOD_BASE, GPIO_PIN_0, GPIO_PIN_0);
	// SDO
	GPIOPinTypeGPIOInput(GPIOD_BASE, GPIO_PIN_1);

	GPIOIntTypeSet(GPIOD_BASE, GPIO_PIN_1, GPIO_FALLING_EDGE);
	GPIOIntRegister(GPIOD_BASE, data_valid);
	GPIOIntEnable(GPIOD_BASE, GPIO_INT_PIN_1);
}

void data_valid(void){
	GPIOIntClear(GPIOD_BASE, GPIO_INT_PIN_1);
	GPIOIntDisable(GPIOD_BASE, GPIO_INT_PIN_1);
	touchpanel_driver();
}
