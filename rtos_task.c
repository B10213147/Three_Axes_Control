/*
 * rtos_task.c
 *
 *  Created on: May 13, 2016
 *      Author: Harvard Tseng
 */

#include "rtos.h"

struct rtos_task *rtos_running_task;		// Currently running task.
struct rtos_task *rtos_ready_tasks;			// List of ready to run tasks.

void rtos_task_create(voidfuncptr func, void *arg, int interval){
	struct rtos_task *task;
	disable_os();

	task = (struct rtos_task *)malloc(sizeof(struct rtos_task));
	rtos_task_insert(&rtos_ready_tasks, task);
	task->function = func;
	task->agr = arg;
	task->interval = interval;
	task->remain_ticks = interval;
	task->delete_flag = 0;

	enable_os();
}

void rtos_task_delete(struct rtos_task *task){
	disable_os();
	free(task);
	enable_os();
}

//insert a task at the back of list
void rtos_task_insert(struct rtos_task **list, struct rtos_task *task){
	task->next = *list;
	*list = task;
}

void rtos_task_remove(struct rtos_task **list, struct rtos_task *task){
	struct rtos_task *prev, *current;

	if(*list == task) *list = (*list)->next;	//first in the list
	else{
		//serch task in list
		for(prev = *list, current = (*list)->next; current != task;
				prev = current, current = current->next);
		prev->next = current->next;
	}
	task->next = 0;
}
