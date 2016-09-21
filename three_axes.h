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
	int changed_value;
	int current;
	int speed;
	bool working;
	bool finished;
	bool last_onoff_state;
};

struct axis{
	struct pulse_Gen_info *pulse_Gen;
	bool onoff;
	uint8_t dir_pin;
	char dir;
	int total;
	int remain;
	double current_pos;	/* unit: mm */
	double next_move;	/* unit: mm */
	double scale;		/* unit: pulse/mm */
};

extern void axes_init(void);
extern void axis_move(struct axis *axis, bool on_off);
extern void axis_modify(struct axis *axis);
extern double pulse2position(struct axis *axis);

extern struct axis *x_axis, *y_axis, *z_axis;
extern const float duty;
extern const uint32_t full_Period;

#endif /* THREE_AXES_H_ */
