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

void rtos_SysTick_irq(void);

// System variables
uint32_t slice_quantum;
int rtos_recursive_counter;

void enable_os(void){
	rtos_recursive_counter++;
	if(rtos_recursive_counter > 0){
		IntEnable(FAULT_SYSTICK);
	}
}

void disable_os(void){
	IntMasterDisable();
	IntDisable(FAULT_SYSTICK);
	rtos_recursive_counter--;
	IntMasterEnable();
}

// Real time operating system core
void rtos_SysTick_irq(void){
	//	SCB->ICSR = SCB_ICSR_PENDSTCLR_Msk;
	rtos_recursive_counter--;
	//	IntPriorityMaskSet(0x00);	//0 means no effect

	rtos_sched();

	//	IntPriorityMaskSet(0x20);	//only looks at the upper 3 bits
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
	//	Configure the 24-bit SysTick.
	//
	IntRegister(FAULT_SYSTICK, rtos_SysTick_irq);
	while(SysTick_Config(slice_quantum));
}
