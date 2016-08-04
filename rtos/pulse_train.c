/*
 * pulse_train.c
 *
 *  Created on: Jun 13, 2016
 *      Author: Harvard Tseng
 */

#include "pulse_train.h"
#include "rtos.h"
#include "TM4C123GH6PM.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"
#include "driverlib/sysctl.h"

#define disable	0
#define enable	1
#define delay	2
#define RED		GPIO_PIN_1
#define BLUE	GPIO_PIN_2
#define GREEN	GPIO_PIN_3

struct rtos_pipe *pulse_Fifo;

const uint32_t period = 1000;	//unit = ms
char latch;
void pulse_train(struct pulse_info *info){
	if(info->output == delay){	//delay 1s
		info->count++;
		if(info->count == period){
			info->output = disable;
		}
	}

	if(info->output == disable){
		info->count = 0;
		info->finish = 0;

		if(rtos_pipe_read(pulse_Fifo, &latch, 1)){
			if(latch>='1' && latch<='9'){	//number
				info->output = enable;
			}
			else if(latch=='r' || latch=='R'){
				info->color = RED;
			}
			else if(latch=='g' || latch=='G'){
				info->color = GREEN;
			}
			else if(latch=='b' || latch=='B'){
				info->color = BLUE;
			}
			else{
				info->output = disable;
			}
		}
		else{	//nothing in pipe
			info->output = disable;
		}
	}

	if(info->output == enable){
		info->count++;
		if(info->count >= period/(latch-48)){
			if(info->pin_state == 0x00){
				info->pin_state = info->color;
			}
			else{
				info->pin_state = 0x00;
				info->finish++;
				info->count = 0;
			}

			GPIOPinWrite(GPIOF_BASE, info->color, info->pin_state);

			if(info->finish == latch-48){
				info->output = delay;
			}
		}
	}
}

struct pulse_info *pulse_train_init(void){
	struct pulse_info *info;
	info = (struct pulse_info *)malloc(sizeof(struct pulse_info));
	info->count = 0;
	info->finish = 0;
	info->output = disable;
	info->color = GREEN;

	//
	// Enable the GPIO port that is used for the on-board LED.
	//
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

	//
	// Check if the peripheral access is enabled.
	//
	while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF))
	{
	}

	GPIOPinTypeGPIOOutput(GPIOF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);

	pulse_Fifo = rtos_pipe_create(10);

	return info;
}
