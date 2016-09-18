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
	float LeadscrewPitch;           /* unit: mm/rev */
}AxisInitTypeDef;

typedef struct{
	float x;	/* unit: mm */
	float y;	/* unit: mm */
}point;

extern void Axis_Config(void);
extern void move_P2P(point p1, point p2);
extern void drop_lift(float l1, float l2);	/* length unit: mm */
extern void calculate_pos(void);
extern void motion_control(void);

extern struct rtos_pipe *mc_Fifo;

#endif /* MOTION_CONTROL_H_ */
