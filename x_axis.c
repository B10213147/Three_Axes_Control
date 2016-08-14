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
#include "driverlib/interrupt.h"
#include "driverlib/pwm.h"
#include "inc/hw_ints.h"

void x_timer_End(void);

struct pulse_Gen_info x_pulse_Gen_info =
	{0, 0, 0, 0, false, true};

void x_axis_Init(void){
	x_axis = (struct axis *)malloc(sizeof(struct axis));
	x_axis->pulse_Gen = &x_pulse_Gen_info;
	x_axis->dir_pin = GPIO_PIN_4;
	x_axis->dir = 'l';
	x_axis->total = 0;
	x_axis->remain = 0;
	x_axis->current_pos = 0;
	x_axis->next_move = 0;

	GPIOPinTypeGPIOOutput(GPIOA_BASE, GPIO_PIN_4);

	//Configure PB5 Pin as PWM
	GPIOPinConfigure(GPIO_PB5_M0PWM3);
	GPIOPinTypePWM(GPIOB_BASE, GPIO_PIN_5);
	//Configure PB3 Pin as Timer Capture
	GPIOPinConfigure(GPIO_PB3_T3CCP1);
	GPIOPinTypeTimer(GPIOB_BASE, GPIO_PIN_3);
	//Configure PWM Options
	//PWM_GEN_1 Covers M0PWM2 and M0PWM3 See page 207 4/11/13 DriverLib doc
	PWMGenConfigure(PWM0_BASE, PWM_GEN_1, PWM_GEN_MODE_UP_DOWN | PWM_GEN_MODE_NO_SYNC);

	//
	// Configure the timer captures.
	//
	TimerConfigure(TIMER3_BASE, TIMER_CFG_SPLIT_PAIR | TIMER_CFG_A_CAP_COUNT | TIMER_CFG_B_CAP_COUNT);
	TimerIntRegister(TIMER3_BASE, TIMER_B, x_timer_End);
	IntPrioritySet(INT_TIMER3B, 0x20);	//set Timer3B to 1 priority
	TimerMatchSet(TIMER3_BASE, TIMER_B, 0);
	TimerIntEnable(TIMER3_BASE, TIMER_CAPB_MATCH);
}

void x_pwm_Start(void){
	uint32_t period = full_Period / x_pulse_Gen_info.current;	//unit = ticks/cycle
	uint32_t width_H = period * duty;

	TimerLoadSet(TIMER3_BASE, TIMER_B, x_pulse_Gen_info.current);

	TimerEnable(TIMER3_BASE, TIMER_B);

	PWMGenPeriodSet(PWM0_BASE, PWM_GEN_1, period);
	PWMPulseWidthSet(PWM0_BASE, PWM_OUT_3, width_H);
	// Enable the PWM generator
	PWMGenEnable(PWM0_BASE, PWM_GEN_1);
	// Turn on the Output pins
	PWMOutputState(PWM0_BASE, PWM_OUT_3_BIT, true);
}

void x_timer_End(void){
	disable_os();
	if(TimerIntStatus(TIMER3_BASE, true) & TIMER_CAPB_MATCH){
		// Disable the PWM generator
		PWMGenDisable(PWM0_BASE, PWM_GEN_1);
		// Turn off the Output pins
		PWMOutputState(PWM0_BASE, PWM_OUT_3_BIT, false);
		TimerIntClear(TIMER3_BASE, TIMER_CAPB_MATCH);
		x_pulse_Gen_info.working = false;
	}
	enable_os();
}

