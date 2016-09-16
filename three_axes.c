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

void set_speed(struct pulse_Gen_info *pulse_Gen, int max_speed);
void pulse_Generator(struct pulse_Gen_info *pulse_Gen, bool bEnable);
void set_dir(struct axis *axis, bool state);

struct axis *x_axis;
struct axis *y_axis;
struct axis *z_axis;
const float duty = 0.5;
const uint32_t full_Period = 16000/2*5; //unit = ticks/5ms

//!	state == true means speed increase or constant speed
//!	state == false means speed decrease
void axis_move(struct pulse_Gen_info *pulse_Gen, bool on_off){
	if(pulse_Gen->finished == true){
		if(pulse_Gen == &x_pulse_Gen_info)
			set_dir(x_axis, true);
		else if(pulse_Gen == &y_pulse_Gen_info)
			set_dir(y_axis, true);
		else if(pulse_Gen == &z_pulse_Gen_info)
			set_dir(z_axis, true);
	}

	if(on_off != false)
		set_speed(pulse_Gen, 12);
	else
		set_speed(pulse_Gen, 0);

	if(pulse_Gen->speed != 0){
		pulse_Generator(pulse_Gen, true);
		pulse_Gen->finished = false;
	}
	else{
		pulse_Generator(pulse_Gen, false);
//		pulse_Gen->finished = true;
	}
}

void axis_modify(struct pulse_Gen_info *pulse_Gen){
	if(pulse_Gen->current - pulse_Gen->total > 0){
		if(pulse_Gen == &x_pulse_Gen_info)
			set_dir(x_axis, false);
		else if(pulse_Gen == &y_pulse_Gen_info)
			set_dir(y_axis, false);
		else if(pulse_Gen == &z_pulse_Gen_info)
			set_dir(z_axis, false);

		pulse_Gen->speed = 6;
	}
	else if(pulse_Gen->current - pulse_Gen->total < 0){
		if(pulse_Gen == &x_pulse_Gen_info)
			set_dir(x_axis, true);
		else if(pulse_Gen == &y_pulse_Gen_info)
			set_dir(y_axis, true);
		else if(pulse_Gen == &z_pulse_Gen_info)
			set_dir(z_axis, true);

		pulse_Gen->speed = 1;
	}
	else{
		pulse_Gen->speed = 0;
	}

	set_speed(pulse_Gen, pulse_Gen->speed);

	if(pulse_Gen->speed != 0){
		pulse_Generator(pulse_Gen, true);
		pulse_Gen->finished = false;
	}
	else{
		pulse_Generator(pulse_Gen, false);
		pulse_Gen->finished = true;
	}
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
//	y_axis_Init();
//	z_axis_Init();
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
//	else if(pulse_Gen == &y_pulse_Gen_info)
//		y_pwm_Speed_Set(pulse_Gen->speed);
//	else if(pulse_Gen == &z_pulse_Gen_info)
//		z_pwm_Speed_Set(pulse_Gen->speed);
}

//!	state == true means enable PWM
//!	state == false means disable PWM
void pulse_Generator(struct pulse_Gen_info *pulse_Gen, bool bEnable){
	if(bEnable == false){
		disable_os();

		if(pulse_Gen == &x_pulse_Gen_info)
			x_pwm_Stop();
//		else if(pulse_Gen == &y_pulse_Gen_info)
//			y_pwm_Stop();
//		else if(pulse_Gen == &z_pulse_Gen_info)
//			z_pwm_Stop();

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


