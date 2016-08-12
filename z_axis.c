/*
 * z_axis.c
 *
 *  Created on: July 31, 2016
 *      Author: Harvard Tseng
 */

#include "z_axis.h"
#include "rtos.h"
#include "TM4C123GH6PM.h"
#include "driverlib/timer.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/interrupt.h"
#include "driverlib/pwm.h"
#include "inc/hw_ints.h"

void z_timer_End(void);

struct pulse_Gen_info z_pulse_Gen_info =
	{0, 0, 0, 0, false, true};

void z_axis_Init(void){
	z_axis = (struct axis*)malloc(sizeof(struct axis));
	*z_axis = *x_axis;
	z_axis->pulse_Gen = &z_pulse_Gen_info;
}

void z_pwm_Start(void){

}

void z_timer_End(void){

}

