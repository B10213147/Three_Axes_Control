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

void movement(struct pulse_Gen_info *pulse_Gen, int max_speed);
void pulse_Generator(struct pulse_Gen_info *pulse_Gen);
void reverse(struct pulse_Gen_info *pulse_Gen);
void set_dir(struct axis *axis, bool state);

struct axis *x_axis;
struct axis *y_axis;
struct axis *z_axis;
const float duty = 0.01;
const uint32_t full_Period = 65000; //unit = ticks/10ms

void axis_move(struct pulse_Gen_info *pulse_Gen, int pulses){
	if(pulse_Gen->finished == true){
		pulse_Gen->total = pulses;
		if(pulse_Gen->total == 0) return;
		pulse_Gen->remain = pulses;
		pulse_Gen->finished = false;

		if(pulse_Gen == &x_pulse_Gen_info)
			set_dir(x_axis, true);
		else if(pulse_Gen == &y_pulse_Gen_info)
			set_dir(y_axis, true);
		else if(pulse_Gen == &z_pulse_Gen_info)
			set_dir(z_axis, true);
	}

	if(pulse_Gen->next == 0){
		// Acceleration & Deceleration
		if(pulse_Gen->total >= 45*2){
			movement(pulse_Gen, 10);
		}
		else{	// total < 45*2
			movement(pulse_Gen, 5);
		}
		// Position modify
		if(pulse_Gen->current <= 1 && pulse_Gen->remain < 0){
			reverse(pulse_Gen);
		}
	}

	pulse_Generator(pulse_Gen);

	if(pulse_Gen->remain != 0) pulse_Gen->finished = false;
	else{
		pulse_Gen->finished = true;
		pulse_Gen->current = 0;
	}
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

void movement(struct pulse_Gen_info *pulse_Gen, int max_speed){
	int sum = 0;
	for(int i=1; i<max_speed; sum+=i, i++);
	if(pulse_Gen->remain > sum){
		if(pulse_Gen->current < max_speed){
			// Accelerate
			pulse_Gen->next = 1 + pulse_Gen->current;
		}
		else{
			// Constant speed
			pulse_Gen->next = max_speed;
		}
	}
	else if(pulse_Gen->current > 0 && pulse_Gen->remain <= sum){
		// Decelerate
		pulse_Gen->next = pulse_Gen->current - 1;
	}
}

void pulse_Generator(struct pulse_Gen_info *pulse_Gen){
	if((pulse_Gen->working == false) && (pulse_Gen->next > 0)){
		disable_os();
		pulse_Gen->current = pulse_Gen->next;
		pulse_Gen->next = 0;

		if(pulse_Gen == &x_pulse_Gen_info)
			x_pwm_Start();
		else if(pulse_Gen == &y_pulse_Gen_info)
			y_pwm_Start();
		else if(pulse_Gen == &z_pulse_Gen_info)
			z_pwm_Start();

		pulse_Gen->remain -= pulse_Gen->current;
		pulse_Gen->working = true;
		enable_os();
	}
}

void reverse(struct pulse_Gen_info *pulse_Gen){
	if(pulse_Gen == &x_pulse_Gen_info)
		set_dir(x_axis, false);
	else if(pulse_Gen == &y_pulse_Gen_info)
		set_dir(y_axis, false);
	else if(pulse_Gen == &z_pulse_Gen_info)
		set_dir(z_axis, false);

	pulse_Gen->next = 1;
	pulse_Gen->remain += 1*2;
}

//!	state == true means non_reverse
//!	state == false means reverse
void set_dir(struct axis *axis, bool state){
	if(state != false){
		if(axis->dir == 'r' || axis->dir == 'u' || axis->dir == 'f')
			GPIOPinWrite(GPIOF_BASE, axis->dir_pin, axis->dir_pin);
		else GPIOPinWrite(GPIOF_BASE, axis->dir_pin, 0);
	}
	else{
		if(axis->dir == 'r') GPIOPinWrite(GPIOF_BASE, axis->dir_pin, 0);
		else GPIOPinWrite(GPIOF_BASE, axis->dir_pin, axis->dir_pin);
	}
}

