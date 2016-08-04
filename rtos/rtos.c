/*
 * rtos.c
 *
 *  Created on: May 13, 2016
 *      Author: Harvard Tseng
 */

#include "rtos.h"
#include "TM4C123GH6PM.h"
#include "inc/hw_ints.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"
#include "driverlib/interrupt.h"

// System variables
uint32_t slice_quantum;
int rtos_recursive_counter;

void enable_os(void){
	rtos_recursive_counter++;
	if(rtos_recursive_counter > 0) TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
}

void disable_os(void){
	IntMasterDisable();
	TimerIntDisable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
	rtos_recursive_counter--;
	IntMasterEnable();
}

// Real time operating system core
void rtos_Timer0_irq(void){
	TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
	rtos_recursive_counter--;
	IntPriorityMaskSet(0x00);	//0 means no effect

	rtos_sched();

	IntPriorityMaskSet(0x20);	//only looks at the upper 3 bits
	enable_os();
}

// Start real time operating system
// slice ... timeslice in microseconds
void rtos_init(uint32_t slice){
	rtos_recursive_counter = 0;
	rtos_running_task = 0;
	rtos_ready_tasks = 0;
	slice_quantum = slice * (SysCtlClockGet() / 1000000);

	//
	// Enable the peripherals used by this example.
	//
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);

	//
	// Check if the peripheral access is enabled.
	//
	while(!SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER0))
	{
	}

	//
	// Configure the 32-bit periodic timer.
	//
	TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
	TimerIntRegister(TIMER0_BASE, TIMER_A, rtos_Timer0_irq);
	IntPrioritySet(INT_TIMER0A, 0x00);	//set Timer0A to the highest priority
	TimerLoadSet(TIMER0_BASE, TIMER_A, slice_quantum);

	//
	// Enable the timer.
	//
	TimerEnable(TIMER0_BASE, TIMER_A);
}
