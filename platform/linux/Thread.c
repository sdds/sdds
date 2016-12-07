/*
 * Thread.c
 *
 *  Created on: Jul 19, 2010
 *      Author: kai
 */

#include "os-ssal/Thread.h"
#include "os-ssal/SSW.h"
#include "os-ssal/Memory.h"
#include "Log.h"
#include <pthread.h>
#include <unistd.h>



Thread
Thread_create() {

    pthread_t* thread = Memory_alloc(sizeof(pthread_t));
    if (thread == NULL) {

        Log_error("No memory\n");
        return NULL;
    }
    return (Thread) thread;
}

// interface for the to run methode attachable to a thread
// Second parameter is the assorizated object, that the uses the thread
//ssw_rt_t Thread_function(Thread _this, void* object);

ssw_rc_t
Thread_init(Thread _this, void* (*function)(void* object), void* obj) {
    if (_this == NULL) {
        Log_error("no thread arg given\n");
        return SDDS_SSW_RT_FAIL;
    }

    int rc = pthread_create((pthread_t*)_this, NULL, function, obj);
    if (rc != 0) {
        Log_error("can not create thread\n");
        return SDDS_SSW_RT_FAIL;
    }

    return SDDS_RT_OK;
}

ssw_rc_t
Thread_run(Thread _this);
ssw_rc_t
Thread_stop(Thread _this);

ssw_rc_t
Thread_usleep(Thread _this, uint16_t usecSleep) {
    usleep(usecSleep);
    return SDDS_RT_OK;
}

ssw_rc_t
Thread_sleep(Thread _this, uint16_t secSleep) {
    sleep(secSleep);
    return SDDS_RT_OK;
}

ssw_rc_t
Thread_yield(void) {
    // TODO
    return SDDS_RT_OK;
}
