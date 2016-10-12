/*
 * touchpanel_driver.c
 *
 *  Created on: Oct 12, 2016
 *      Author: Harvard Tseng
 */

#include "touchpanel_driver.h"
#include "rtos.h"
#include "TM4C123GH6PM.h"
#include "driverlib/i2c.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"

void touchpanel_driver(void){


}

void touchpanel_driver_init(void){
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C1);

	//
	// Check if the peripheral access is enabled.
	//
	while(!SysCtlPeripheralReady(SYSCTL_PERIPH_I2C1))
	{
	}

	//Configure PA6, PA7 Pin as I2CSCL, I2CSDA
	GPIOPinConfigure(GPIO_PA6_I2C1SCL);
	GPIOPinConfigure(GPIO_PA7_I2C1SDA);
	GPIOPinTypeI2CSCL(GPIOA_BASE, GPIO_PIN_6);
	GPIOPinTypeI2C(GPIOA_BASE, GPIO_PIN_7);

	uint32_t sysctl = SysCtlClockGet();
	I2CMasterInitExpClk(I2C1_BASE, sysctl, false);
	I2CMasterEnable(I2C1_BASE);
}
