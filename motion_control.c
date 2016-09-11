/*
 * motion_control.c
 *
 *  Created on: August 10, 2016
 *      Author: Harvard Tseng
 */

#include "motion_control.h"
#include "rtos.h"
#include "three_axes.h"

struct rtos_pipe *mc_Fifo;
float x_scale, y_scale, z_scale;		/* unit: pulse/mm */

void Axis_Config(void){
	AxisInitTypeDef Axis_x, Axis_y, Axis_z;

	Axis_x.MotorStepPerRev = 360/1.8;
	Axis_x.DriverMicrostepping = 1;
	Axis_x.MotorCoupleLeadscrew = 1;
	Axis_x.LeadscrewPitch = 1.5;
	x_scale = Axis_x.MotorStepPerRev * Axis_x.DriverMicrostepping *
			Axis_x.MotorCoupleLeadscrew / Axis_x.LeadscrewPitch;

	Axis_y.MotorStepPerRev = 360/1.8;
	Axis_y.DriverMicrostepping = 1;
	Axis_y.MotorCoupleLeadscrew = 1;
	Axis_y.LeadscrewPitch = 1.5;
	y_scale = Axis_y.MotorStepPerRev * Axis_y.DriverMicrostepping *
			Axis_y.MotorCoupleLeadscrew / Axis_y.LeadscrewPitch;

	Axis_z.MotorStepPerRev = 360/1.8;
	Axis_z.DriverMicrostepping = 1;
	Axis_z.MotorCoupleLeadscrew = 1;
	Axis_z.LeadscrewPitch = 1.5;
	z_scale = Axis_z.MotorStepPerRev * Axis_z.DriverMicrostepping *
			Axis_z.MotorCoupleLeadscrew / Axis_z.LeadscrewPitch;

	mc_Fifo = rtos_pipe_create(50);
	axes_init();
}

bool is_z_at_Home = true;
void calculate_pos(void){
	char temp;
	/*
	 * Get pipe character
	 */
	while(rtos_pipe_read(mc_Fifo, &temp, 1)){
		switch(temp){
		// Arrow up
		case 0x41:
			x_axis->next_move += 0.1;
			break;
			// Arrow down
		case 0x42:
			x_axis->next_move -= 0.1;
			break;
			// Arrow right
		case 0x43:
			y_axis->next_move += 0.1;
			break;
			// Arrow left
		case 0x44:
			y_axis->next_move -= 0.1;
			break;
			// Go Home
		case 'H':
			x_axis->next_move -= x_axis->current_pos;
			y_axis->next_move -= y_axis->current_pos;
			break;
			// z_axis goes up
		case 'u':
			z_axis->next_move += 0.1;
			break;
			// z_axis goes down
		case 'd':
			z_axis->next_move -= 0.1;
			break;
			// z_axis goes home
		case 'U':
			z_axis->next_move -= z_axis->current_pos;
			break;
		}
	}

	/*
	 * x_axis & y_axis move
	 */
	if(is_z_at_Home == true && z_axis->pulse_Gen->finished != false &&
			x_axis->pulse_Gen->finished != false &&
			y_axis->pulse_Gen->finished != false &&
			(x_axis->next_move*100 != 0 || y_axis->next_move*100 != 0)){
		point cur_pos = {x_axis->current_pos, y_axis->current_pos};
		point next_pos;
		next_pos.x = cur_pos.x + x_axis->next_move;
		next_pos.y = cur_pos.y + y_axis->next_move;

		move_P2P(cur_pos, next_pos);

		x_axis->current_pos = next_pos.x;
		x_axis->next_move = 0;
		y_axis->current_pos = next_pos.y;
		y_axis->next_move = 0;
	}

	/*
	 * z_axis move
	 */
	if(z_axis->pulse_Gen->finished != false &&
			x_axis->pulse_Gen->finished != false &&
			y_axis->pulse_Gen->finished != false &&
			z_axis->next_move*100 != 0){
		float cur_pos = z_axis->current_pos;
		float next_pos = cur_pos + z_axis->next_move;

		drop_lift(cur_pos, next_pos);

		z_axis->current_pos = next_pos;
		z_axis->next_move = 0;
	}
}

void move_P2P(point p1, point p2){
	int dx = 0, dy = 0;

	if(x_axis->pulse_Gen->finished != false &&
			y_axis->pulse_Gen->finished != false){
		dx = (p2.x - p1.x) * x_scale;
		dy = (p2.y - p1.y) * y_scale;

		if(dx < 0){
			x_axis->dir = 'n';
			dx = -dx;
		}
		else x_axis->dir = 'p';
		if(dy < 0){
			y_axis->dir = 'n';
			dy = -dy;
		}
		else y_axis->dir = 'p';

		rtos_task_create(move_P2P, 0, 1);
	}

	axis_move(x_axis->pulse_Gen, dx);
	axis_move(y_axis->pulse_Gen, dy);

	if(x_axis->pulse_Gen->finished != false &&
			y_axis->pulse_Gen->finished != false){
		rtos_running_task->delete_flag = true;
	}
}

//!	length unit: mm
void drop_lift(float l1, float l2){
	int dz = 0;

	if(z_axis->pulse_Gen->finished != false){
		dz = (l2 - l1) * z_scale;

		if(dz < 0){
			z_axis->dir = 'd';
			dz = -dz;
		}
		else z_axis->dir = 'u';

		rtos_task_create(drop_lift, 0, 1);
	}

	axis_move(z_axis->pulse_Gen, dz);

	if(z_axis->pulse_Gen->finished != false){
		rtos_running_task->delete_flag = true;
	}

	if(z_axis->current_pos*100 != 0){
		is_z_at_Home = false;
	}
	else is_z_at_Home = true;
}
