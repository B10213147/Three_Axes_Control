/*
 * motion_control.c
 *
 *  Created on: August 10, 2016
 *      Author: Harvard Tseng
 */

#include "math.h"
#include "motion_control.h"
#include "rtos.h"
#include "three_axes.h"

struct rtos_pipe *mc_Fifo;
double x_scale, y_scale, z_scale;		/* unit: pulse/mm */

void move_unknow_distance(struct axis *n_axis);
void move_know_distance(struct axis *n_axis);
void pipe_character_Get(void);

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

void motion_control(void){
	pipe_character_Get();

	if(x_axis->onoff == true && x_axis->pulse_Gen->finished == true){
		if(x_axis->next_move == 0)
			// interval means acceleration
			rtos_task_create(move_unknow_distance, x_axis, 10);
		else
			axis_move(x_axis->pulse_Gen, true);
			rtos_task_create(move_know_distance, x_axis, 10);
	}
	if(y_axis->onoff == true && y_axis->pulse_Gen->finished == true){
		if(y_axis->next_move == 0)
			// interval means acceleration
			rtos_task_create(move_unknow_distance, y_axis, 10);
		else
			axis_move(y_axis->pulse_Gen, true);
			move_know_distance(y_axis);
	}
	if(z_axis->onoff == true && z_axis->pulse_Gen->finished == true){
		if(z_axis->next_move == 0)
			// interval means acceleration
			rtos_task_create(move_unknow_distance, z_axis, 10);
		else
			axis_move(z_axis->pulse_Gen, true);
			move_know_distance(z_axis);
	}
}

bool is_z_at_Home = true;
void calculate_pos(void){
	pipe_character_Get();

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

void move_unknow_distance(struct axis *n_axis){
	if(n_axis->onoff != false){
		axis_move(n_axis->pulse_Gen, true);
	}
	else{
		axis_move(n_axis->pulse_Gen, false);
		if(n_axis->pulse_Gen->speed == 0){
			rtos_task_create(axis_modify, n_axis->pulse_Gen, 1);
			rtos_running_task->delete_flag = true;
		}
	}
}

void move_know_distance(struct axis *n_axis){
	// Check if reach move value
	if(fabs(n_axis->next_move)*1000 >= fabs(pulse2position(n_axis->pulse_Gen))*1000){
		n_axis->onoff = true;
	}
	else n_axis->onoff = false;

	if(n_axis->onoff != false){
		axis_move(n_axis->pulse_Gen, true);
	}
	else{
		axis_move(n_axis->pulse_Gen, false);
		if(n_axis->pulse_Gen->speed == 0){
			rtos_task_create(axis_modify, n_axis->pulse_Gen, 1);
			rtos_running_task->delete_flag = true;
		}
	}
}

void pipe_character_Get(void){
	char temp;
	/*
	 * Get pipe character
	 */
	if(rtos_pipe_read(mc_Fifo, &temp, 1)){
		do{
			switch(temp){
			// Arrow up
			case 0x41:
				x_axis->onoff = true;
				x_axis->dir = 'p';
				break;
				// Arrow down
			case 0x42:
				x_axis->onoff = true;
				x_axis->dir = 'n';
				break;
				// Arrow right
			case 0x43:
				y_axis->onoff = true;
				y_axis->dir = 'p';
				break;
				// Arrow left
			case 0x44:
				y_axis->onoff = true;
				y_axis->dir = 'n';
				break;
				// Go Home
			case 'H':
				x_axis->next_move -= x_axis->current_pos;
				x_axis->onoff = true;
				y_axis->next_move -= y_axis->current_pos;
				y_axis->onoff = true;
				break;
				// z_axis goes up
			case 'u':
				z_axis->onoff = true;
				z_axis->dir = 'u';
				break;
				// z_axis goes down
			case 'd':
				z_axis->onoff = true;
				z_axis->dir = 'd';
				break;
				// z_axis goes home
			case 'U':
				z_axis->next_move -= z_axis->current_pos;
				z_axis->onoff = true;
				break;
			}
		}while(rtos_pipe_read(mc_Fifo, &temp, 1));
	}
	else{
		// No character in pipe
		x_axis->onoff = false;
		y_axis->onoff = false;
		z_axis->onoff = false;
	}
}

