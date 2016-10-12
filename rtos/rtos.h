/*
 * rtos.h
 *
 *  Created on: May 13, 2016
 *      Author: Harvard Tseng
 */

#ifndef RTOS_H_
#define RTOS_H_

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

typedef void (* voidfuncptr)();

struct rtos_pipe{
  unsigned int begin;
  unsigned int end;
  unsigned int size;
  char *data;
};

struct rtos_task{
	struct rtos_task *next;
	voidfuncptr function;
	void *agr;
	int interval;
	int remain_ticks;
	bool delete_flag;
};

struct rtos_task_list{
	struct rtos_task *task;
	struct rtos_task_list *next;	// next task in the list
};

// Function & variable declarations
// rtos
extern void enable_os(void);
extern void disable_os(void);
extern void rtos_init(uint32_t slice);

// rtos_sched
extern void rtos_sched(void);

// rtos_task
extern struct rtos_task *rtos_running_task;		// Currently running task.
extern struct rtos_task *rtos_ready_tasks;		// List of ready to run tasks.
extern void rtos_task_create(voidfuncptr function, void *arg, int interval);
extern void rtos_task_delete(struct rtos_task *task);
extern void rtos_task_insert(struct rtos_task **list, struct rtos_task *task);
extern void rtos_task_remove(struct rtos_task **list, struct rtos_task *task);

// rtos_pipe
extern struct rtos_pipe *rtos_pipe_create(unsigned int size);
extern void rtos_pipe_delete(struct rtos_pipe *pipe);
extern unsigned int rtos_pipe_read(struct rtos_pipe *pipe, char *data, unsigned int num_bytes);
extern unsigned int rtos_pipe_write(struct rtos_pipe *pipe, char *data, unsigned int num_bytes);

#endif /* RTOS_H_ */
