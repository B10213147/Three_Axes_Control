/*
 * y_axis.h
 *
 *  Created on: July 31, 2016
 *      Author: Harvard Tseng
 */

#ifndef Y_AXIS_H_
#define Y_AXIS_H_

#include "three_axes.h"

extern void y_axis_Init(void);
extern void y_pwm_Start(void);

extern struct pulse_Gen_info y_pulse_Gen_info;

#endif /* Y_AXIS_H_ */
