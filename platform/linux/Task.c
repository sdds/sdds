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
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "Log.h"

struct Task_struct {

	timer_t timer;
	struct sigevent evp;

	void (*cb)(void* obj);
	void* data;
	SDDS_usec_t interval;
	SSW_TaskMode_t mode;
	bool active;
};

/**
 * Should be called at the init phase
 */
ssw_rc_t TaskMng_init(void) {

	return SDDS_SSW_RT_OK;
}

Task Task_create(void) {
	Task t = malloc(sizeof(struct Task_struct));
	return t;
}

void Task_callback(union sigval v) {
	Task t = (Task) v.sival_ptr;

	t->cb(t->data);
	t->active = false;
}

static void /* Thread notification function */
threadFunc(union sigval sv) {
	printf("hello task\n");
}

ssw_rc_t Task_setData(Task _this, void* data) {
	if (_this == NULL) {
		return SDDS_SSW_RT_FAIL;
	}

	_this->data = data;

	return SDDS_SSW_RT_OK;
}

/**
 * inits a task with a callback function etc
 */
ssw_rc_t Task_init(Task _this, void (*callback)(void* obj), void* data) {
	int rc;
	if (_this == NULL || callback == NULL) {
		return SDDS_SSW_RT_FAIL;
	}

	_this->cb = callback;

	// init the signal structure
	memset(&(_this->evp), 0, sizeof(struct sigevent));

	// call the callback fkt in a new thread
	_this->evp.sigev_notify = SIGEV_THREAD;
	// use this internal callback
	_this->evp.sigev_notify_function = Task_callback;
	// no thread attributes
	_this->evp.sigev_notify_attributes = NULL;

	// arg for the callback is the task structure
	_this->evp.sigev_value.sival_ptr = &(_this->timer);

	_this->active = false;

	rc = timer_create(CLOCK_REALTIME, &(_this->evp), &(_this->timer));
	if (rc != 0) {
		return SDDS_SSW_RT_FAIL;
	}

	return SDDS_SSW_RT_OK;
}

ssw_rc_t Task_start(Task _this, uint8_t sec, SDDS_msec_t msec,
		SSW_TaskMode_t mode) {
	if (_this == NULL || _this->cb == NULL) {
		return SDDS_SSW_RT_FAIL;
	}

	if (!_this->active) {
		struct timespec intv;
		struct timespec value;

		value.tv_nsec = msec * 1000000;
		value.tv_sec = sec;

		if (mode == SDDS_SSW_TaskMode_single) {
			intv.tv_nsec = 0;
			intv.tv_sec = 0;
		} else if (mode == SDDS_SSW_TaskMode_repeat) {
			intv.tv_nsec = msec * 1000000;
			intv.tv_sec = sec;
		} else {
			return SDDS_SSW_RT_FAIL;
		}
		struct itimerspec timer = { intv, value };

		int rc = timer_settime(_this->timer, 0, &timer, NULL);
		if (rc != 0) {
			Log_debug("errno: %s\n", strerror(errno));
			return SDDS_SSW_RT_FAIL;
		}
		_this->active = true;
	}
	return SDDS_SSW_RT_OK;
}
ssw_rc_t Task_stop(Task _this) {
	if (_this == NULL || _this->timer == 0) {
		return SDDS_SSW_RT_FAIL;
	}
	// stop the timer
	struct itimerspec timer;
	memset(&timer, 0, sizeof(struct itimerspec));
	int rc = timer_settime(_this->timer, 0, &timer, NULL);

	if (rc != 0) {
		return SDDS_SSW_RT_FAIL;
	}
	_this->active = false;
	return SDDS_SSW_RT_OK;
}

ssw_rc_t Task_delete(Task _this) {
	if (_this == NULL) {
		return SDDS_SSW_RT_FAIL;
	}
	Task_stop(_this);

	free(_this);

	return SDDS_SSW_RT_OK;

}

