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

void move_unknow_distance(struct axis *axis);
void move_know_distance(struct axis *axis);
void pipe_character_Get(void);

void Axis_Config(void){
	AxisInitTypeDef Axis_x, Axis_y, Axis_z;

	axes_init();

	Axis_x.MotorStepPerRev = 360/1.8;
	Axis_x.DriverMicrostepping = 1;
	Axis_x.MotorCoupleLeadscrew = 1;
	Axis_x.LeadscrewPitch = 1.5;
	x_axis->scale = Axis_x.MotorStepPerRev * Axis_x.DriverMicrostepping *
			Axis_x.MotorCoupleLeadscrew / Axis_x.LeadscrewPitch;

	Axis_y.MotorStepPerRev = 360/1.8;
	Axis_y.DriverMicrostepping = 1;
	Axis_y.MotorCoupleLeadscrew = 1;
	Axis_y.LeadscrewPitch = 1.5;
	y_axis->scale = Axis_y.MotorStepPerRev * Axis_y.DriverMicrostepping *
			Axis_y.MotorCoupleLeadscrew / Axis_y.LeadscrewPitch;

	Axis_z.MotorStepPerRev = 360/1.8;
	Axis_z.DriverMicrostepping = 1;
	Axis_z.MotorCoupleLeadscrew = 1;
	Axis_z.LeadscrewPitch = 1.5;
	z_axis->scale = Axis_z.MotorStepPerRev * Axis_z.DriverMicrostepping *
			Axis_z.MotorCoupleLeadscrew / Axis_z.LeadscrewPitch;

	mc_Fifo = rtos_pipe_create(20);
}

void motion_control(void){
	pipe_character_Get();

	if(/*z_axis->current_pos*1000 == 0 &&*/
			x_axis->onoff == true &&
			x_axis->pulse_Gen->finished == true){
		if(x_axis->next_move == 0){
			axis_move(x_axis, true);
			// interval means acceleration
			rtos_task_create(move_unknow_distance, x_axis, 10);
		}
		else{
			axis_move(x_axis, true);
			rtos_task_create(move_know_distance, x_axis, 10);
		}
	}
	if(/*z_axis->current_pos*1000 == 0 &&*/
			y_axis->onoff == true &&
			y_axis->pulse_Gen->finished == true){
		if(y_axis->next_move == 0){
			// interval means acceleration
			rtos_task_create(move_unknow_distance, y_axis, 10);
		}
		else{
			axis_move(y_axis, true);
			rtos_task_create(move_know_distance, y_axis, 10);
		}
	}
	if(z_axis->onoff == true && z_axis->pulse_Gen->finished == true &&
			x_axis->pulse_Gen->finished != false &&
			y_axis->pulse_Gen->finished != false){
		if(z_axis->next_move == 0){
			// interval means acceleration
			rtos_task_create(move_unknow_distance, z_axis, 10);
		}
		else{
			axis_move(z_axis, true);
			rtos_task_create(move_know_distance, z_axis, 10);
		}
	}
}

void move_unknow_distance(struct axis *axis){
	if(axis->onoff != false){
		axis_move(axis, true);
	}
	else{
		axis_move(axis, false);
		if(axis->pulse_Gen->speed == 0){
			rtos_task_create(axis_modify, axis, 1);
			rtos_running_task->delete_flag = true;
		}
	}
}

void move_know_distance(struct axis *axis){
	// Check if reach move value
	if(fabs(axis->next_move)*1000 > fabs(pulse2position(axis))*1000){
		axis->onoff = true;
	}
	else axis->onoff = false;

	if(axis->onoff != false){
		axis_move(axis, true);
	}
	else{
		axis_move(axis, false);
		axis->pulse_Gen->total = fabs(axis->next_move * axis->scale);
		if(axis->pulse_Gen->speed == 0){
			rtos_task_create(axis_modify, axis, 1);
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
				x_axis->next_move = -x_axis->current_pos;
				x_axis->onoff = true;
				if(x_axis->next_move*1000 < 0) x_axis->dir = 'n';
				else x_axis->dir = 'p';

				y_axis->next_move = -y_axis->current_pos;
				y_axis->onoff = true;
				if(y_axis->next_move*1000 < 0) y_axis->dir = 'n';
				else y_axis->dir = 'p';

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
				z_axis->next_move = -z_axis->current_pos;
				z_axis->onoff = true;
				if(z_axis->next_move*1000 < 0) z_axis->dir = 'd';
				else z_axis->dir = 'u';
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

