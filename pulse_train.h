/*
 * pulse_train.h
 *
 *  Created on: Jun 13, 2016
 *      Author: Harvard Tseng
 */

#ifndef PULSE_TRAIN_H_
#define PULSE_TRAIN_H_

struct pulse_info{
	int count;
	int finish;
	int output;
	unsigned int pin_state;
	unsigned int color;
};

extern struct rtos_pipe *pulse_Fifo;

extern struct pulse_info *pulse_train_init(void);
extern void pulse_train(struct pulse_info *info);

#endif /* PULSE_TRAIN_H_ */
