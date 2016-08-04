/*
 * uart_driver.c
 *
 *  Created on: Jun 23, 2016
 *      Author: Harvard Tseng
 */

#include "uart_driver.h"
#include "rtos.h"
#include "TM4C123GH6PM.h"
#include "driverlib/uart.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"

struct rtos_pipe *uart_rx_Fifo;
struct rtos_pipe *uart_tx_Fifo;

void uart_driver(void){
	char rx_data, tx_data;

	if(UARTCharsAvail(UART0_BASE)){ //there is data in the receive FIFO
		rx_data = UARTCharGetNonBlocking(UART0_BASE);
		if(rx_data != -1){
			rtos_pipe_write(uart_rx_Fifo, &rx_data, 1);
		}
	}

	if(!UARTBusy(UART0_BASE)){ //all transmissions are complete
		if(rtos_pipe_read(uart_tx_Fifo, &tx_data, 1)){
			UARTCharPutNonBlocking(UART0_BASE, tx_data);
		}
	}
}

void uart_driver_init(void){
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

	//
	// Check if the peripheral access is enabled.
	//
	while(!SysCtlPeripheralReady(SYSCTL_PERIPH_UART0))
	{
	}

	GPIOPinTypeUART(GPIOA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
	UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(),
			9600, UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE);
	UARTFIFOEnable(UART0_BASE);
	UARTEnable(UART0_BASE);

	uart_rx_Fifo = rtos_pipe_create(10);
	uart_tx_Fifo = rtos_pipe_create(30);
}
