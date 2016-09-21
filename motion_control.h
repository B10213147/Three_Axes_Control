/*
 * motion_control.h
 *
 *  Created on: August 10, 2016
 *      Author: Harvard Tseng
 */

#ifndef MOTION_CONTROL_H_
#define MOTION_CONTROL_H_

#include <stdint.h>

typedef struct{
	uint16_t MotorStepPerRev;		/* Motor steps per revolution */
	uint16_t DriverMicrostepping;   /* Driver Microstepping */
	uint16_t MotorCoupleLeadscrew;  /* Motor:Leadscrew */
	double LeadscrewPitch;			/* unit: mm/rev */
}AxisInitTypeDef;

typedef struct{
	double x;	/* unit: mm */
	double y;	/* unit: mm */
}point;

extern void Axis_Config(void);
extern void motion_control(void);

extern struct rtos_pipe *mc_Fifo;

#endif /* MOTION_CONTROL_H_ */
