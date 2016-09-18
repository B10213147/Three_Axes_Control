/*
 * main.c
 */
#include "rtos.h"
#include "TM4C123GH6PM.h"
#include "driverlib/sysctl.h"
#include "pulse_train.h"
#include "keys_driver.h"
#include "uart_driver.h"
#include "motion_control.h"
#include "three_axes.h"

void startup(void);
void print_string(char *string);
void arrow_decode(void);
extern void move_unknow_distance(struct axis *n_axis);

point Home = {0, 0};
int main(void) {
	startup();
//
	rtos_task_create(keys_driver, 0, 2);
//	rtos_task_create(uart_driver, 0, 1);
//	rtos_task_create(calculate_pos, 0, 50);

	char temp;
	while(1){
		if(rtos_pipe_read(uart_rx_Fifo, &temp, 1)){
			rtos_pipe_write(uart_tx_Fifo, &temp, 1);
			print_string("\n\r");	//change line and start from left

			if(temp == 0x1B || temp == 0x5B){	// 0x1B 0x5B are special character of arrow
			}
			else{	// Normal ASCII
				rtos_pipe_write(mc_Fifo, &temp, 1);
			}
		}

		if(rtos_pipe_read(keys_Fifo, &temp, 1)){
			switch(temp){
			case 'R':
				print_string("Right botton pressed\n\r");
				if(z_axis->onoff != true)
					// interval means acceleration
					rtos_task_create(move_unknow_distance, z_axis, 200);
				z_axis->onoff = true;
				break;
			case 'L':
				print_string("Left botton pressed\n\r");
				z_axis->onoff = false;
				break;
			}
		}
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

	rtos_init(1000);	//slice = 1000us

	keys_driver_init();

	uart_driver_init();

	Axis_Config();

	enable_os();
}
