/*
 * three_axes.h
 *
 *  Created on: July 7, 2016
 *      Author: Harvard Tseng
 */

#ifndef THREE_AXES_H_
#define THREE_AXES_H_

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "x_axis.h"
#include "y_axis.h"
#include "z_axis.h"

#define RED		GPIO_PIN_1
#define BLUE	GPIO_PIN_2
#define GREEN	GPIO_PIN_3

struct pulse_Gen_info{
	int total;
	int remain;
	int current;
	int next;
	bool working;
	bool finished;
};

struct axis{
	struct pulse_Gen_info *pulse_Gen;
	uint8_t dir_pin;
	char dir;
	int total;
	int remain;
	float current_pos;	/* unit: mm */
	float next_move;		/* unit: mm */
};

extern void axes_init(void);
extern void axis_move(struct pulse_Gen_info *pulse_Gen, int pulses);

extern struct axis *x_axis;
extern struct axis *y_axis;
extern struct axis *z_axis;
extern const float duty;
extern const uint32_t full_Period;

#endif /* THREE_AXES_H_ */
