/*
 * z_axis.h
 *
 *  Created on: July 31, 2016
 *      Author: Harvard Tseng
 */

#ifndef Z_AXIS_H_
#define Z_AXIS_H_

#include "three_axes.h"

extern void z_axis_Init(void);
extern void z_pwm_Start(void);

extern struct pulse_Gen_info z_pulse_Gen_info;

#endif /* Z_AXIS_H_ */
