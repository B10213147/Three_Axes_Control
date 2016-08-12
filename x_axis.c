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
	x_axis->dir_pin = GREEN;
	x_axis->dir = 'l';
	x_axis->total = 0;
	x_axis->remain = 0;
	x_axis->current_pos = 0;
	x_axis->next_move = 0;

	//Configure PF1 Pin as PWM
	GPIOPinConfigure(GPIO_PF1_M1PWM5);
	GPIOPinTypePWM(GPIOF_BASE, GPIO_PIN_1);
	//Configure PB0 Pin as Timer Capture
	GPIOPinConfigure(GPIO_PB0_T2CCP0);
	GPIOPinTypeTimer(GPIOB_BASE, GPIO_PIN_0);
	//Configure PWM Options
	//PWM_GEN_2 Covers M1PWM4 and M1PWM5 See page 207 4/11/13 DriverLib doc
	PWMGenConfigure(PWM1_BASE, PWM_GEN_2, PWM_GEN_MODE_UP_DOWN | PWM_GEN_MODE_NO_SYNC);

	//
	// Configure the timer captures.
	//
	TimerConfigure(TIMER2_BASE, TIMER_CFG_SPLIT_PAIR | TIMER_CFG_A_CAP_COUNT | TIMER_CFG_B_CAP_COUNT);
	TimerIntRegister(TIMER2_BASE, TIMER_A, x_timer_End);
	IntPrioritySet(INT_TIMER2A, 0x20);	//set Timer2A to 1 priority
	TimerMatchSet(TIMER2_BASE, TIMER_A, 0);
	TimerIntEnable(TIMER2_BASE, TIMER_CAPA_MATCH);
}

void x_pwm_Start(void){
	uint32_t period = full_Period / x_pulse_Gen_info.current;	//unit = ticks/cycle
	uint32_t width_H = period * duty;

	TimerLoadSet(TIMER2_BASE, TIMER_A, x_pulse_Gen_info.current);

	TimerEnable(TIMER2_BASE, TIMER_A);

	PWMGenPeriodSet(PWM1_BASE, PWM_GEN_2, period);
	PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5, width_H);
	// Enable the PWM generator
	PWMGenEnable(PWM1_BASE, PWM_GEN_2);
	// Turn on the Output pins
	PWMOutputState(PWM1_BASE, PWM_OUT_5_BIT, true);
}

void x_timer_End(void){
	disable_os();
	if(TimerIntStatus(TIMER2_BASE, true) & TIMER_CAPA_MATCH){
		// Disable the PWM generator
		PWMGenDisable(PWM1_BASE, PWM_GEN_2);
		// Turn off the Output pins
		PWMOutputState(PWM1_BASE, PWM_OUT_5_BIT, false);
		TimerIntClear(TIMER2_BASE, TIMER_CAPA_MATCH);
		x_pulse_Gen_info.working = false;
	}
	enable_os();
}

