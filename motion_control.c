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

	  mc_Fifo = rtos_pipe_create(10);
	  axes_init();
}

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
		}
	}

	if(x_axis->pulse_Gen->finished != false &&
		y_axis->pulse_Gen->finished != false &&
		(x_axis->next_move*10 != 0 || y_axis->next_move*10 != 0)){
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
}

void move_P2P(point p1, point p2){
	int dx = 0, dy = 0;

	if(x_axis->pulse_Gen->finished != false &&
		y_axis->pulse_Gen->finished != false){
		dx = (p2.x - p1.x) * x_scale;
		dy = (p2.y - p1.y) * y_scale;

		if(dx < 0){
			x_axis->dir = 'r';
			dx = -dx;
		}
		else x_axis->dir = 'l';
		if(dy < 0){
			y_axis->dir = 'u';
			dy = -dy;
		}
		else y_axis->dir = 'd';

		rtos_task_create(move_P2P, 0, 1);
	}

	if(z_axis->pulse_Gen->finished == true){
		axis_move(x_axis->pulse_Gen, dx);
		axis_move(y_axis->pulse_Gen, dy);
	}

	if(x_axis->pulse_Gen->finished != false &&
		y_axis->pulse_Gen->finished != false){
		rtos_running_task->delete_flag = true;
	}
}
