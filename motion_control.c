/*
 * motion_control.c
 *
 *  Created on: August 10, 2016
 *      Author: Harvard Tseng
 */

#include "motion_control.h"
#include "rtos.h"
#include "three_axes.h"

AxisInitTypeDef Axis_x, Axis_y;
float x_scale, y_scale;			/* unit: pulse/mm */

void Axis_Config(void){
	  Axis_x.MotorStepPerRev = 360/1.8;
	  Axis_x.DriverMicrostepping = 8;
	  Axis_x.MotorCoupleLeadscrew = 1;
	  Axis_x.LeadscrewPitch = 1.5;
	  x_scale = Axis_x.MotorStepPerRev * Axis_x.DriverMicrostepping *
	            Axis_x.MotorCoupleLeadscrew / Axis_x.LeadscrewPitch;

	  Axis_y.MotorStepPerRev = 360/1.8;
	  Axis_y.DriverMicrostepping = 8;
	  Axis_y.MotorCoupleLeadscrew = 1;
	  Axis_y.LeadscrewPitch = 1.5;
	  y_scale = Axis_y.MotorStepPerRev * Axis_y.DriverMicrostepping *
	            Axis_y.MotorCoupleLeadscrew / Axis_y.LeadscrewPitch;

	  axes_init();
}

void move_P2P(point p1, point p2){
	int dx = (p2.x - p1.x) * x_scale;
	int dy = (p2.y - p1.y) * y_scale;
	axis_move(x_axis->pulse_Gen, dx);
	axis_move(y_axis->pulse_Gen, dy);
}
