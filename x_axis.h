/*
 * x_axis.h
 *
 *  Created on: July 31, 2016
 *      Author: Harvard Tseng
 */

#ifndef X_AXIS_H_
#define X_AXIS_H_

#include "three_axes.h"

extern void x_axis_Init(void);
extern void x_pwm_Speed_Set(int speed);
extern void x_pwm_Start(void);
extern void x_pwm_Stop(void);
extern uint32_t x_Timer_Value_Get(void);

extern struct pulse_Gen_info x_pulse_Gen_info;

#endif /* X_AXIS_H_ */
