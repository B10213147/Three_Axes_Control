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

uint8_t pin_state = 0;
uint32_t data = 0;
void touchpanel_driver(void){
	for(int i=0; i<50; i++);
	for(int i=0; i<32; i++){
		for(int j=0; j<100; j++);
		if(pin_state != 0){
			if((GPIOPinRead(GPIOD_BASE, GPIO_PIN_1) | GPIO_PIN_1) == 0){
				data |= 0x1 << i;
			}
			pin_state = 0;

		}
		else{
			pin_state = GPIO_PIN_0;
		}
		GPIOPinWrite(GPIOD_BASE, GPIO_PIN_0, pin_state);

	}
	data = 0;
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
	GPIOPinWrite(GPIOD_BASE, GPIO_PIN_0, pin_state);
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
