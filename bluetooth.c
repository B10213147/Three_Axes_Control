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

	if(UARTCharsAvail(UART2_BASE)){ //there is data in the receive FIFO
		rx_data = UARTCharGetNonBlocking(UART2_BASE);
		if(rx_data != -1){
			rtos_pipe_write(bt_rx_Fifo, &rx_data, 1);
		}
	}

	if(!UARTBusy(UART2_BASE)){ //all transmissions are complete
		if(rtos_pipe_read(bt_tx_Fifo, &tx_data, 1)){
			UARTCharPutNonBlocking(UART2_BASE, tx_data);
		}
	}
}

void bluetooth_init(void){
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART2);

	//
	// Check if the peripheral access is enabled.
	//
	while(!SysCtlPeripheralReady(SYSCTL_PERIPH_UART2))
	{
	}

	//Configure PD6, PD7 Pin as UARTRx, UARTTx
	GPIOPinConfigure(GPIO_PD6_U2RX);
	GPIOPinConfigure(GPIO_PD7_U2TX);
	GPIOPinTypeUART(GPIOD_BASE, GPIO_PIN_6 | GPIO_PIN_7);

	UARTConfigSetExpClk(UART2_BASE, SysCtlClockGet(),
			9600, UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE);
	UARTFIFOEnable(UART2_BASE);
	UARTEnable(UART2_BASE);

	bt_rx_Fifo = rtos_pipe_create(10);
	bt_tx_Fifo = rtos_pipe_create(30);
}
