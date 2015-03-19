/*
 * Task.c
 *
 *  Created on: Jul 22, 2010
 *      Author: kai
 */

#include "os-ssal/Task.h"

#include "pthread.h"
#include <time.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>

struct Task_struct{

    timer_t timer;
    struct sigevent evp;

    void(*cb)(void* obj);
    void* data;
    SDDS_usec_t interval;
    SSW_TaskMode_t mode;

};


/**
 * Should be called at the init phase
 */
ssw_rc_t TaskMng_init(void)
{

    return SDDS_SSW_RT_OK;
}

Task Task_create(void)
{
    Task t = malloc(sizeof(struct Task_struct));
    return t;
}

void Task_callback(union sigval v){
    Task t = (Task) v.sival_ptr;

    t->cb(t->data);
  // printf("hello task\n");
}

/**
 * inits a task with a callback function etc
 */
ssw_rc_t Task_init(Task _this, void(*callback)(void* obj), void* data)
{
    if (_this == NULL || callback == NULL){
	return SDDS_SSW_RT_FAIL;
    }
    int rc;

    _this->cb = callback;
    _this->data = data;

    // init the signal structure
    memset(&(_this->evp), 0, sizeof(struct sigevent));

    // call the callback fkt in a new thread
    _this->evp.sigev_notify = SIGEV_THREAD;
    // arg for the callback is the task structure
    _this->evp.sigev_value.sival_ptr = (void*)_this;

    // no thread attributes
    _this->evp.sigev_notify_attributes = NULL;
    // use this internal callback
    _this->evp.sigev_notify_function = Task_callback;

    rc = timer_create(CLOCK_REALTIME, &(_this->evp), &(_this->timer));
    if (rc != 0){
	return SDDS_SSW_RT_FAIL;
    }


    return SDDS_SSW_RT_OK;
}

ssw_rc_t Task_start(Task _this, uint8_t sec, SDDS_usec_t usec, SSW_TaskMode_t mode)
{
    if (_this == NULL){
	return SDDS_SSW_RT_FAIL;
    }
    struct timespec intv = {0,0};
    struct timespec value = {0,0};

    if (mode == SDDS_SSW_TaskMode_single){
	value.tv_nsec = usec *1000;
	value.tv_sec = sec;
    } else if (mode == SDDS_SSW_TaskMode_repeat){
	intv.tv_nsec = usec *1000;
	intv.tv_sec = sec;
    } else {
	return SDDS_SSW_RT_FAIL;
    }
    struct itimerspec timer = {intv, value};

    int rc = timer_settime(_this, 0, &timer, NULL);
    if (rc != 0){
	return SDDS_SSW_RT_FAIL;
    }

    return SDDS_SSW_RT_OK;
}
ssw_rc_t Task_stop(Task _this)
{

    if (_this == NULL){
	return SDDS_SSW_RT_FAIL;
    }
    // stop the timer
    struct itimerspec timer;
    memset(&timer, 0, sizeof(struct itimerspec));
    int rc = timer_settime(_this, 0, &timer, NULL);

    if (rc != 0){
	return SDDS_SSW_RT_FAIL;
    }

    return SDDS_SSW_RT_OK;
}
ssw_rc_t Task_delete(Task _this)
{
    if (_this == NULL){
	return SDDS_SSW_RT_FAIL;
    }
    Task_stop(_this);

    free(_this);

    return SDDS_SSW_RT_OK;

}


