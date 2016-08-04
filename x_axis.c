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

void x_pulse_Gen(void);
void x_pwm_Start(void);
void x_timer_End(void);
void x_reverse(void);
void x_set_dir(int state);
void x_movement(int max_speed);

struct pulse_Gen_info x_pulse_Gen_info =
	{0, 0, 0, 0, false, true};

void x_axis_Move(int pulses){
	if(x_pulse_Gen_info.finished == true){
		x_pulse_Gen_info.total = pulses;
		if(x_pulse_Gen_info.total == 0) return;
		x_pulse_Gen_info.remain = pulses;
		x_pulse_Gen_info.finished = false;
		x_set_dir(non_reverse);
	}

	if(x_pulse_Gen_info.next == 0){
		// Acceleration & Deceleration
		if(x_pulse_Gen_info.total >= 45*2){
			x_movement(10);
		}
		else{	// total < 45*2
			x_movement(5);
		}
		// Position modify
		if(x_pulse_Gen_info.current <= 1 && x_pulse_Gen_info.remain < 0){
			x_reverse();
		}
	}
	x_pulse_Gen();
	if(x_pulse_Gen_info.remain != 0) x_pulse_Gen_info.finished = false;
	else{
		x_pulse_Gen_info.finished = true;
		x_pulse_Gen_info.current = 0;
	}
}

void x_axis_Init(void){
	x_axis = (struct axis *)malloc(sizeof(struct axis));
	x_axis->pulse_Gen = &x_pulse_Gen_info;
	x_axis->dir_pin = GREEN;
	x_axis->dir = 'l';
	x_axis->total = 0;
	x_axis->remain = 0;
	x_axis->current = 0;
	x_axis->next = 0;

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
	IntPrioritySet(INT_TIMER2A, 0x01);	//set Timer2A to 1 priority
	TimerMatchSet(TIMER2_BASE, TIMER_A, 0);
	TimerIntEnable(TIMER2_BASE, TIMER_CAPA_MATCH);
}

void x_movement(int max_speed){
	int sum = 0;
	for(int i=1; i<max_speed; sum+=i, i++);
	if(x_pulse_Gen_info.remain > sum){
		if(x_pulse_Gen_info.current < max_speed){
			// Accelerate
			x_pulse_Gen_info.next = 1 + x_pulse_Gen_info.current;
		}
		else{
			// Constant speed
			x_pulse_Gen_info.next = max_speed;
		}
	}
	else if(x_pulse_Gen_info.current > 0 && x_pulse_Gen_info.remain <= sum){
		// Decelerate
		x_pulse_Gen_info.next = x_pulse_Gen_info.current - 1;
	}
}

void x_reverse(void){
	x_set_dir(reverse);
	x_pulse_Gen_info.next = 1;
	x_pulse_Gen_info.remain += 1*2;
}

void x_set_dir(int state){
	if(state != reverse){
		if(x_axis->dir == 'r') GPIOPinWrite(GPIOF_BASE, x_axis->dir_pin, x_axis->dir_pin);
		else GPIOPinWrite(GPIOF_BASE, x_axis->dir_pin, 0);
	}
	else{
		if(x_axis->dir == 'r') GPIOPinWrite(GPIOF_BASE, x_axis->dir_pin, 0);
		else GPIOPinWrite(GPIOF_BASE, x_axis->dir_pin, x_axis->dir_pin);
	}
}

void x_pulse_Gen(void){
	if((x_pulse_Gen_info.working == false) && (x_pulse_Gen_info.next > 0)){
		disable_os();
		x_pulse_Gen_info.current = x_pulse_Gen_info.next;
		x_pulse_Gen_info.next = 0;
		x_pwm_Start();
		x_pulse_Gen_info.remain -= x_pulse_Gen_info.current;
		x_pulse_Gen_info.working = true;
		enable_os();
	}
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

