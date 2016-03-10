/*
 * Thread.c
 *
 *  Created on: Aug 11, 2015
 *      Author: z_zoor
 */

#include "os-ssal/Thread.h"
#include "os-ssal/SSW.h"
#include "os-ssal/Memory.h"
#include "Log.h"
#include "vtimer.h"
#include <thread.h>

const int thread_stack_size =  THREAD_STACKSIZE_DEFAULT * sizeof(char);
Thread
Thread_create() {
    int* pid = Memory_alloc(sizeof(int));
    if (pid == NULL) {
        return NULL;
    }
    return (Thread) pid;
}

// interface for the to run methode attachable to a thread
// Second parameter is the assorizated object, that the uses the thread
//ssw_rt_t Thread_function(Thread _this, void* object);

ssw_rc_t
Thread_init(Thread _this, void* (*function)(void* object), void* obj) {
    char* thread_stack = NULL;
    thread_stack = Memory_alloc(thread_stack_size);
    if (_this == NULL) {
        return SDDS_SSW_RT_FAIL;
    }

    int* pid = (int*)_this;

    *pid = thread_create(thread_stack, thread_stack_size,
                         THREAD_PRIORITY_MAIN-1, 0,
                         function, obj, "sDDS User-Thread");
    if (*pid < 0) {
        return SDDS_SSW_RT_FAIL;
    }

    return SDDS_RT_OK;
}

ssw_rc_t
Thread_run(Thread _this);
ssw_rc_t
Thread_stop(Thread _this);
ssw_rc_t
Thread_sleep(Thread _this, uint16_t usecSleep) {
    (void)_this;
    vtimer_usleep(usecSleep);
    return SDDS_RT_OK;
}
