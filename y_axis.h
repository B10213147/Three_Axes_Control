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
extern void y_pwm_Speed_Set(int speed);
extern void y_pwm_Start(void);
extern void y_pwm_Stop(void);
extern uint32_t y_Timer_Value_Get(void);

extern struct pulse_Gen_info y_pulse_Gen_info;

#endif /* Y_AXIS_H_ */
