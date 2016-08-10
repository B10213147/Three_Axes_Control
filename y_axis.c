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
#include "driverlib/interrupt.h"
#include "driverlib/pwm.h"
#include "inc/hw_ints.h"

void y_timer_End(void);

struct pulse_Gen_info y_pulse_Gen_info =
	{0, 0, 0, 0, false, true};

void y_axis_Init(void){
	y_axis = (struct axis*)malloc(sizeof(struct axis));
	*y_axis = *x_axis;
	y_axis->pulse_Gen = &y_pulse_Gen_info;

	//Configure PF2 Pin as PWM
	GPIOPinConfigure(GPIO_PF2_M1PWM6);
	GPIOPinTypePWM(GPIOF_BASE, GPIO_PIN_2);
	//Configure PB1 Pin as Timer Capture
	GPIOPinConfigure(GPIO_PB1_T2CCP1);
	GPIOPinTypeTimer(GPIOB_BASE, GPIO_PIN_1);
	//Configure PWM Options
	//PWM_GEN_3 Covers M1PWM6 and M1PWM7 See page 207 4/11/13 DriverLib doc
	PWMGenConfigure(PWM1_BASE, PWM_GEN_3, PWM_GEN_MODE_UP_DOWN | PWM_GEN_MODE_NO_SYNC);

	//
	// Configure the timer captures.
	//
	TimerConfigure(TIMER2_BASE, TIMER_CFG_SPLIT_PAIR | TIMER_CFG_A_CAP_COUNT | TIMER_CFG_B_CAP_COUNT);
	TimerIntRegister(TIMER2_BASE, TIMER_B, y_timer_End);
	IntPrioritySet(INT_TIMER2B, 0x20);	//set Timer2B to 1 priority
	TimerMatchSet(TIMER2_BASE, TIMER_B, 0);
	TimerIntEnable(TIMER2_BASE, TIMER_CAPB_MATCH);
}

void y_pwm_Start(void){
	uint32_t period = full_Period / y_pulse_Gen_info.current;	//unit = ticks/cycle
	uint32_t width_H = period * duty;

	TimerLoadSet(TIMER2_BASE, TIMER_B, y_pulse_Gen_info.current);

	TimerEnable(TIMER2_BASE, TIMER_B);

	PWMGenPeriodSet(PWM1_BASE, PWM_GEN_3, period);
	PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6, width_H);
	// Enable the PWM generator
	PWMGenEnable(PWM1_BASE, PWM_GEN_3);
	// Turn on the Output pins
	PWMOutputState(PWM1_BASE, PWM_OUT_6_BIT, true);
}

void y_timer_End(void){
	disable_os();
	if(TimerIntStatus(TIMER2_BASE, true) & TIMER_CAPB_MATCH){
		// Disable the PWM generator
		PWMGenDisable(PWM1_BASE, PWM_GEN_3);
		// Turn off the Output pins
		PWMOutputState(PWM1_BASE, PWM_OUT_6_BIT, false);
		TimerIntClear(TIMER2_BASE, TIMER_CAPB_MATCH);
		y_pulse_Gen_info.working = false;
	}
	enable_os();
}

