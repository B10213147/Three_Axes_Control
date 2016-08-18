/*
 * keys_driver.h
 *
 *  Created on: Jun 18, 2016
 *      Author: Harvard Tseng
 */

#ifndef KEYS_DRIVER_H_
#define KEYS_DRIVER_H_

extern struct rtos_pipe *keys_Fifo;

extern void keys_driver_init(void);
extern void keys_driver(void);

#endif /* KEYS_DRIVER_H_ */
