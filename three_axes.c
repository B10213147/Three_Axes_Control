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

uint32_t axis_timer_feedback(struct pulse_Gen_info *pulse_Gen);
void set_speed(struct pulse_Gen_info *pulse_Gen, int max_speed);
void pulse_Generator(struct pulse_Gen_info *pulse_Gen, bool bEnable);
void set_dir(struct axis *axis, bool state);

struct axis *x_axis, *y_axis, *z_axis;
const float duty = 0.5;
const uint32_t full_Period = 16000/2*5; //unit = ticks/5ms

//!	state == true means speed increase or constant speed
//!	state == false means speed decrease
void axis_move(struct axis *axis, bool on_off){
	if(axis->pulse_Gen->finished == true){
		set_dir(axis, true);
	}

	if(on_off != false)
		set_speed(axis->pulse_Gen, 12);
	else
		set_speed(axis->pulse_Gen, 0);

	if(axis->pulse_Gen->last_onoff_state && !on_off){
		axis->pulse_Gen->total = axis_timer_feedback(axis->pulse_Gen);
	}

	if(axis->pulse_Gen->speed != 0){
		pulse_Generator(axis->pulse_Gen, true);
		axis->pulse_Gen->finished = false;
	}
	else{
		pulse_Generator(axis->pulse_Gen, false);
	}

	axis->pulse_Gen->current = axis_timer_feedback(axis->pulse_Gen);
	axis->pulse_Gen->last_onoff_state = on_off;
}

void axis_modify(struct axis *axis){
	int current_pulses;

	if(axis->pulse_Gen->current - axis->pulse_Gen->total > 0){
		set_dir(axis, false);
		axis->pulse_Gen->speed = 5;
	}
	else{
		axis->pulse_Gen->speed = 0;
	}
	// Constant speed
	set_speed(axis->pulse_Gen, axis->pulse_Gen->speed);

	if(axis->pulse_Gen->speed != 0){
		pulse_Generator(axis->pulse_Gen, true);
		axis->pulse_Gen->finished = false;
	}

	// First calculation
	if(axis->pulse_Gen->current > axis->pulse_Gen->total){
		current_pulses = axis->pulse_Gen->current - axis_timer_feedback(axis->pulse_Gen);
		if(current_pulses <= axis->pulse_Gen->total){
			axis->pulse_Gen->changed_value = axis->pulse_Gen->current - current_pulses;
			axis->pulse_Gen->current = current_pulses;
			axis->pulse_Gen->speed = 0;
		}
	}

	if(axis->pulse_Gen->speed == 0){
		pulse_Generator(axis->pulse_Gen, false);
		axis->pulse_Gen->finished = true;
		axis->current_pos += pulse2position(axis);
		axis->next_move = 0;
		axis->pulse_Gen->current = 0;
		axis->pulse_Gen->total = 0;
		rtos_running_task->delete_flag = true;
	}
}

double pulse2position(struct axis *axis){
	double changed_pos;

	if(axis->dir == 'p' || axis->dir == 'u'){
		changed_pos = axis->pulse_Gen->current / axis->scale;
	}
	else{
		changed_pos = -axis->pulse_Gen->current / axis->scale;
	}

	return changed_pos;
}

void axes_init(void){
	//Configure PWM Clock to match system
	SysCtlPWMClockSet(SYSCTL_PWMDIV_2);

	// Enable the peripherals used by this program.
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM0);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER2);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER3);

	x_axis_Init();
	y_axis_Init();
	z_axis_Init();
}

uint32_t axis_timer_feedback(struct pulse_Gen_info *pulse_Gen){
	uint32_t counter = 0;

	if(pulse_Gen == &x_pulse_Gen_info)
		counter = x_Timer_Value_Get();
	else if(pulse_Gen == &y_pulse_Gen_info)
		counter = y_Timer_Value_Get();
	else if(pulse_Gen == &z_pulse_Gen_info)
		counter = z_Timer_Value_Get();

	return counter;
}

void set_speed(struct pulse_Gen_info *pulse_Gen, int max_speed){
	if(pulse_Gen->speed < max_speed){
		// Accelerate
		pulse_Gen->speed++;
	}
	else if(pulse_Gen->speed > max_speed){
		// Decelerate
		pulse_Gen->speed--;
	}
	else{
		// Constant speed
		pulse_Gen->speed = max_speed;
	}

	if(pulse_Gen == &x_pulse_Gen_info)
		x_pwm_Speed_Set(pulse_Gen->speed);
	else if(pulse_Gen == &y_pulse_Gen_info)
		y_pwm_Speed_Set(pulse_Gen->speed);
	else if(pulse_Gen == &z_pulse_Gen_info)
		z_pwm_Speed_Set(pulse_Gen->speed);
}

//!	state == true means enable PWM
//!	state == false means disable PWM
void pulse_Generator(struct pulse_Gen_info *pulse_Gen, bool bEnable){
	if(bEnable == false){
		disable_os();

		if(pulse_Gen == &x_pulse_Gen_info)
			x_pwm_Stop();
		else if(pulse_Gen == &y_pulse_Gen_info)
			y_pwm_Stop();
		else if(pulse_Gen == &z_pulse_Gen_info)
			z_pwm_Stop();

		enable_os();
	}

	if((pulse_Gen->working == false) && (bEnable == true)){
		disable_os();

		if(pulse_Gen == &x_pulse_Gen_info)
			x_pwm_Start();
		else if(pulse_Gen == &y_pulse_Gen_info)
			y_pwm_Start();
		else if(pulse_Gen == &z_pulse_Gen_info)
			z_pwm_Start();

		enable_os();
	}
}

//!	state == true means non_reverse
//!	state == false means reverse
void set_dir(struct axis *axis, bool state){
	if(state != false){
		if(axis->dir == 'p' || axis->dir == 'u')
			GPIOPinWrite(GPIOA_BASE, axis->dir_pin, axis->dir_pin);
		else GPIOPinWrite(GPIOA_BASE, axis->dir_pin, 0);
	}
	else{
		if(axis->dir == 'p' || axis->dir == 'u')
			GPIOPinWrite(GPIOA_BASE, axis->dir_pin, 0);
		else GPIOPinWrite(GPIOA_BASE, axis->dir_pin, axis->dir_pin);
	}
}
