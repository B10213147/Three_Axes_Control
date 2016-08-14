/*
 * bluetooth.c
 *
 *  Created on: August 9, 2016
 *      Author: Harvard Tseng
 */

#include "bluetooth.h"
#include "rtos.h"
#include "TM4C123GH6PM.h"
#include "driverlib/uart.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"

struct rtos_pipe *bt_rx_Fifo;
struct rtos_pipe *bt_tx_Fifo;

void bluetooth_driver(void){
	char rx_data, tx_data;

	if(UARTCharsAvail(UART5_BASE)){ //there is data in the receive FIFO
		rx_data = UARTCharGetNonBlocking(UART5_BASE);
		if(rx_data != -1){
			rtos_pipe_write(bt_rx_Fifo, &rx_data, 1);
		}
	}

	if(!UARTBusy(UART5_BASE)){ //all transmissions are complete
		if(rtos_pipe_read(bt_tx_Fifo, &tx_data, 1)){
			UARTCharPutNonBlocking(UART5_BASE, tx_data);
		}
	}
}

void bluetooth_init(void){
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART5);

	//
	// Check if the peripheral access is enabled.
	//
	while(!SysCtlPeripheralReady(SYSCTL_PERIPH_UART5))
	{
	}

	//Configure PE4, PE5 Pin as UARTRx, UARTTx
	GPIOPinConfigure(GPIO_PE4_U5RX);
	GPIOPinConfigure(GPIO_PE5_U5TX);
	GPIOPinTypeUART(GPIOE_BASE, GPIO_PIN_4 | GPIO_PIN_5);

	UARTConfigSetExpClk(UART5_BASE, SysCtlClockGet(),
			9600, UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE);
	UARTFIFOEnable(UART5_BASE);
	UARTEnable(UART5_BASE);

	bt_rx_Fifo = rtos_pipe_create(10);
	bt_tx_Fifo = rtos_pipe_create(30);
}
