/*
 * y_axis.c
 *
 *  Created on: July 31, 2016
 *      Author: Harvard Tseng
 */

#include "y_axis.h"
#include "rtos.h"
#include "TM4C123GH6PM.h"
#include "driverlib/timer.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/pwm.h"

struct pulse_Gen_info y_pulse_Gen_info =
	{0, 0, 0, false, true, false};

void y_axis_Init(void){
	y_axis = (struct axis*)malloc(sizeof(struct axis));
	*y_axis = *x_axis;
	y_axis->pulse_Gen = &y_pulse_Gen_info;
	y_axis->dir_pin = GPIO_PIN_3;

	GPIOPinTypeGPIOOutput(GPIOA_BASE, GPIO_PIN_3);

	//Configure PC4 Pin as PWM
	GPIOPinConfigure(GPIO_PC4_M0PWM6);
	GPIOPinTypePWM(GPIOC_BASE, GPIO_PIN_4);
	//Configure PB0 Pin as Timer Capture
	GPIOPinConfigure(GPIO_PB0_T2CCP0);
	GPIOPinTypeTimer(GPIOB_BASE, GPIO_PIN_0);
	//Configure PWM Options
	//PWM_GEN_3 Covers M1PWM6 and M1PWM7 See page 207 4/11/13 DriverLib doc
	PWMGenConfigure(PWM0_BASE, PWM_GEN_3, PWM_GEN_MODE_UP_DOWN | PWM_GEN_MODE_NO_SYNC);

	//
	// Configure the timer captures.
	//
	TimerConfigure(TIMER2_BASE, TIMER_CFG_SPLIT_PAIR | TIMER_CFG_A_CAP_COUNT_UP | TIMER_CFG_B_CAP_COUNT_UP);
}

void y_pwm_Speed_Set(int speed){
	uint32_t period = full_Period / speed;	//unit = ticks/cycle
	uint32_t width_H = period * duty;

	PWMGenPeriodSet(PWM0_BASE, PWM_GEN_3, period);
	PWMPulseWidthSet(PWM0_BASE, PWM_OUT_6, width_H);
}
void y_pwm_Start(void){
	// Reset Timer counter value
	TIMER2->TAV = 0;
	// Enable Timer CCP
	TimerEnable(TIMER2_BASE, TIMER_A);
	// Enable the PWM generator
	PWMGenEnable(PWM0_BASE, PWM_GEN_3);
	// Turn on the Output pins
	PWMOutputState(PWM0_BASE, PWM_OUT_6_BIT, true);

	y_pulse_Gen_info.working = true;
}

void y_pwm_Stop(void){
	// Turn off the Output pins
	PWMOutputState(PWM0_BASE, PWM_OUT_6_BIT, false);
	// Disable the PWM generator
	PWMGenDisable(PWM0_BASE, PWM_GEN_3);
	// Disable Timer CCP
	TimerDisable(TIMER2_BASE, TIMER_A);

	y_pulse_Gen_info.working = false;
}

uint32_t y_Timer_Value_Get(void){
	return TimerValueGet(TIMER2_BASE, TIMER_A);
}
