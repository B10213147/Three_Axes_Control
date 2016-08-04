/*
 * uart_driver.h
 *
 *  Created on: Jun 23, 2016
 *      Author: Harvard Tseng
 */

#ifndef UART_DRIVER_H_
#define UART_DRIVER_H_

extern struct rtos_pipe *uart_rx_Fifo;
extern struct rtos_pipe *uart_tx_Fifo;

extern void uart_driver_init(void);
extern void uart_driver(void);

#endif /* UART_DRIVER_H_ */
