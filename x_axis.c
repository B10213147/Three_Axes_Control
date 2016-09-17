/*
 * x_axis.c
 *
 *  Created on: July 31, 2016
 *      Author: Harvard Tseng
 */

#include "x_axis.h"
#include "rtos.h"
#include "TM4C123GH6PM.h"
#include "driverlib/timer.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/pwm.h"

struct pulse_Gen_info x_pulse_Gen_info =
	{0, 0, 0, 0, false, true};

void x_axis_Init(void){
	x_axis = (struct axis *)malloc(sizeof(struct axis));
	x_axis->pulse_Gen = &x_pulse_Gen_info;
	x_axis->dir_pin = GPIO_PIN_4;
	x_axis->dir = 'p';
	x_axis->total = 0;
	x_axis->remain = 0;
	x_axis->current_pos = 0;
	x_axis->next_move = 0;

	GPIOPinTypeGPIOOutput(GPIOA_BASE, GPIO_PIN_4);

	//Configure PB5 Pin as PWM
	GPIOPinConfigure(GPIO_PB5_M0PWM3);
	GPIOPinTypePWM(GPIOB_BASE, GPIO_PIN_5);
	//Configure PB3 Pin as Timer Capture
//	GPIOPinConfigure(GPIO_PB3_T3CCP1);
//	GPIOPinTypeTimer(GPIOB_BASE, GPIO_PIN_3);
	GPIOPinConfigure(GPIO_PF3_T1CCP1);
	GPIOPinTypeTimer(GPIOF_BASE, GPIO_PIN_3);
	//Configure PWM Options
	//PWM_GEN_1 Covers M0PWM2 and M0PWM3 See page 207 4/11/13 DriverLib doc
	PWMGenConfigure(PWM0_BASE, PWM_GEN_1, PWM_GEN_MODE_UP_DOWN | PWM_GEN_MODE_NO_SYNC);

	//
	// Configure the timer captures.
	//
//	TimerConfigure(TIMER3_BASE, TIMER_CFG_SPLIT_PAIR | TIMER_CFG_A_CAP_COUNT_UP | TIMER_CFG_B_CAP_COUNT_UP);
	TimerConfigure(TIMER1_BASE, TIMER_CFG_SPLIT_PAIR | TIMER_CFG_A_CAP_COUNT_UP | TIMER_CFG_B_CAP_COUNT_UP);
}

void x_pwm_Speed_Set(int speed){
	uint32_t period = full_Period / speed;	//unit = ticks/cycle
	uint32_t width_H = period * duty;

	PWMGenPeriodSet(PWM0_BASE, PWM_GEN_1, period);
	PWMPulseWidthSet(PWM0_BASE, PWM_OUT_3, width_H);
}

void x_pwm_Start(void){
	// Reset Timer counter value
//	TIMER3->TBV = 0;
	TIMER1->TBV = 0;
	// Enable Timer CCP
//	TimerEnable(TIMER3_BASE, TIMER_B);
	TimerEnable(TIMER1_BASE, TIMER_B);
	// Enable the PWM generator
	PWMGenEnable(PWM0_BASE, PWM_GEN_1);
	// Turn on the Output pins
	PWMOutputState(PWM0_BASE, PWM_OUT_3_BIT, true);

	x_pulse_Gen_info.working = true;
}

void x_pwm_Stop(void){
	// Turn off the Output pins
	PWMOutputState(PWM0_BASE, PWM_OUT_3_BIT, false);
	// Disable the PWM generator
	PWMGenDisable(PWM0_BASE, PWM_GEN_1);
	// Disable Timer CCP
//	TimerDisable(TIMER3_BASE, TIMER_B);
	TimerDisable(TIMER1_BASE, TIMER_B);

	x_pulse_Gen_info.working = false;
}

uint32_t x_Timer_Value_Get(void){
//	return TimerValueGet(TIMER3_BASE, TIMER_B);
	return TimerValueGet(TIMER1_BASE, TIMER_B);
}

