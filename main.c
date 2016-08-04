/*
 * main.c
 */
#include "rtos.h"
#include "TM4C123GH6PM.h"
#include "driverlib/sysctl.h"
#include "pulse_train.h"
#include "keys_driver.h"
#include "uart_driver.h"
#include "three_axes.h"


void startup(void);
void print_string(char *string);

struct pulse_info *green_pulse;
int main(void) {
	startup();
	axes_init();

	rtos_task_create(keys_driver, 0, 2);
	rtos_task_create(uart_driver, 0, 3);
//	rtos_task_create(x_axis_Move, 0, 1000);
//	rtos_task_create(pwm_Y_GEN, 0, 1000);

	char temp;
	while(1){
		if(rtos_pipe_read(uart_rx_Fifo, &temp, 1)){
			rtos_pipe_write(uart_tx_Fifo, &temp, 1);
			rtos_pipe_write(pulse_Fifo, &temp, 1);
			print_string("\n\r");	//change line and start from left
		}

		if(rtos_pipe_read(keys_Fifo, &temp, 1)){
			switch(temp){
			case 'R':
				print_string("Right botton pressed\n\r");
				break;
			case 'L':
				print_string("Left botton pressed\n\r");
				break;
			}
		}

		x_axis_Move(81);
	}
}

void print_string(char *string){
	int byte;
	for(byte = 0; string[byte] != 0; byte++);

	rtos_pipe_write(uart_tx_Fifo, string, byte);
}

void startup(void){
	//
	// Set the clocking to run directly from the crystal.
	//
	SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN |
			SYSCTL_XTAL_16MHZ);

//	pwm1_init();

	rtos_init(1000);	//slice = 1000us

	keys_driver_init();

	//	green_pulse = pulse_train_init();

	uart_driver_init();

	enable_os();
}
