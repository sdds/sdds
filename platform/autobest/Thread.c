/*
 * Thread.c
 *
 *  Created on: Aug 11, 2015
 *      Author: z_zoor
 */
#include <stdio.h>
#include "os-ssal/Thread.h"
#include "os-ssal/SSW.h"
#include "os-ssal/Memory.h"

#include <api_calls.h>
#include <error.h>

#define SDDS_AUTOBEST_NUM_TASKS 8
#define SDDS_AUTOBEST_STACK_SIZE 1024
#ifndef SDDS_AUTOBEST_FIRST_FREE_TASK_ID
#define SDDS_AUTOBEST_FIRST_FREE_TASK_ID 4
#endif

#ifndef SDDS_AUTOBEST_TASK_PRIO
#define SDDS_AUTOBEST_TASK_PRIO 101
#endif

#define AUTOBEST_MULT_TO_NANO_SEC 1000000000ULL

static unsigned char stack[SDDS_AUTOBEST_NUM_TASKS][SDDS_AUTOBEST_STACK_SIZE];
static uint8_t used_tasks = 0;

//const int thread_stack_size =  KERNEL_CONF_STACKSIZE_PRINTF_FLOAT * sizeof(char);
Thread Thread_create()
{
	int* pid = Memory_alloc(sizeof(int));
	if (pid == NULL){
		return NULL;
	}
	*pid = SDDS_AUTOBEST_FIRST_FREE_TASK_ID + used_tasks;
	return (Thread) pid;
}

// interface for the to run methode attachable to a thread
// Second parameter is the assorizated object, that the uses the thread
//ssw_rt_t Thread_function(Thread _this, void* object);

ssw_rc_t Thread_init(Thread _this, void* (*function)(void* object), void* obj)
{
	unsigned int err;
	int pid = *((int*)_this);
	if(used_tasks >= SDDS_AUTOBEST_NUM_TASKS){
		return SDDS_RT_FAIL;
	}

	err = sys_task_create(pid, SDDS_AUTOBEST_TASK_PRIO, function,
	                      &stack[used_tasks][SDDS_AUTOBEST_STACK_SIZE], obj, NULL);
	if(err != E_OK){
		return SDDS_RT_FAIL;
	}
	used_tasks++;

	return SDDS_RT_OK;
}

ssw_rc_t Thread_run(Thread _this);
ssw_rc_t Thread_stop(Thread _this){
	unsigned int pid = *((int*)_this);
	sys_task_terminate_other(pid);
}
ssw_rc_t Thread_sleep(Thread _this, uint16_t secSleep)
{
	unsigned int err;
	err = sys_sleep(secSleep * AUTOBEST_MULT_TO_NANO_SEC);
	if(err != E_OS_TIMEOUT){
		return SDDS_RT_FAIL;
	}
	return SDDS_RT_OK;
}




