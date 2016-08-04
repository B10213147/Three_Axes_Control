/*
 * rtos_sched.c
 *
 *  Created on: Jun 19, 2016
 *      Author: Harvard Tseng
 */

#include "rtos.h"

void rtos_sched_run(struct rtos_task *task){
	rtos_task_remove(&rtos_ready_tasks, task);
	rtos_running_task = task;

	enable_os();
	rtos_running_task->function(rtos_running_task->agr);
	disable_os();

	if(rtos_running_task->delete_flag){
		rtos_task_delete(rtos_running_task);
	}
	else{
		rtos_running_task->remain_ticks += rtos_running_task->interval;
		rtos_task_insert(&rtos_ready_tasks, rtos_running_task);
	}
}

struct rtos_task_list *rtos_sched_list_update(void){
	struct rtos_task *task;
	struct rtos_task_list *another, *prev, *cur, *list = 0;

	for(task = rtos_ready_tasks; task; task = task->next){
		task->remain_ticks--;
		if(task->remain_ticks > 0){	//task still sleep
//			task->remain_ticks--;
		}
		else{	//another ready to be scheduled task
			another = (struct rtos_task_list*)malloc(sizeof(struct rtos_task_list));
			another->task = task;
			another->next = 0;
			//insert at the end of list
			for(prev = 0, cur = list; cur; prev = cur, cur = cur->next);
			if(!cur){
				if(prev) prev->next = another;
				else list = another;
			}
		}
	}

	return list;
}

struct rtos_task *rtos_sched_remove(struct rtos_task_list **list){
	struct rtos_task *task;
	struct rtos_task_list *next;

	task = (*list)->task;
	next = (*list)->next;
	free(*list);
	*list = next;

	return task;
}

void rtos_sched(void){
	static struct rtos_task_list *list;
	list = rtos_sched_list_update();
	while(list){
		rtos_sched_run(rtos_sched_remove(&list));
		rtos_running_task = 0;
	}
}


