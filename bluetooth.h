/*
 * bluetooth.h
 *
 *  Created on: August 9, 2016
 *      Author: Harvard	Tseng
 */

#ifndef BLUETOOTH_H_
#define BLUETOOTH_H_

extern struct rtos_pipe *bt_rx_Fifo;
extern struct rtos_pipe *bt_tx_Fifo;

extern void bluetooth_init(void);
extern void bluetooth_driver(void);



#endif /* BLUETOOTH_H_ */
