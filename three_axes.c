/*
 * three_axes.c
 *
 *  Created on: July 7, 2016
 *      Author: Harvard Tseng
 */

#include "three_axes.h"
#include "rtos.h"
#include "TM4C123GH6PM.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"

struct axis *x_axis;
struct axis *y_axis;
struct axis *z_axis;
const float duty = 0.01;
const uint32_t full_Period = 65000; //unit = ticks/10ms

void axis_move(struct axis *axis){
	//axis->working = false;

	if(axis->remain > 0){
		if(axis->dir == 'r' || axis->dir == 'u'){
			GPIOPinWrite(GPIOF_BASE, axis->dir_pin, axis->dir_pin);
		}
		else{
			GPIOPinWrite(GPIOF_BASE, axis->dir_pin, 0);
		}

		axis->current = axis->next;
		//pulse_Gen(axis);
		axis->remain -= axis->current;
	}

	if(axis->remain < 0){
		position_Modify(axis);
	}

	if(axis->remain == 0){
		//axis->working = true;
		axis->next = 0;
		GPIOPinWrite(GPIOF_BASE, axis->dir_pin, 0);
	}
}

void position_Modify(struct axis *axis){
	//inverse direction
	if(axis->dir == 'r' || axis->dir == 'u'){
		GPIOPinWrite(GPIOF_BASE, axis->dir_pin, 0);
	}
	else{
		GPIOPinWrite(GPIOF_BASE, axis->dir_pin, axis->dir_pin);
	}

	axis->remain = -axis->remain;	//prefer positive value
	axis->next = axis->remain % 5;
	do{
		axis->current = axis->next;
		//pulse_Gen(axis);
		axis->remain -= axis->current;
		if(axis->next == 0) axis->next = 5;
	}while(axis->remain != 0);
}

void axes_init(void){
	//Configure PWM Clock to match system
	SysCtlPWMClockSet(SYSCTL_PWMDIV_64);

	// Enable the peripherals used by this program.
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM1);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER2);
	GPIOPinTypeGPIOOutput(GPIOF_BASE, GPIO_PIN_3);

	x_axis_Init();
	y_axis_Init();
	z_axis_Init();
}
