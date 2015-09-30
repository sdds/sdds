/*
 * Task.c
 *
 *  Created on: Jul 22, 2010
 *      Author: kai
 */

#include "os-ssal/Task.h"
#include "os-ssal/Memory.h"

#include "contiki.h"

static clock_time_t Task_checkTimer();

struct Task_struct{
	struct etimer timer;
    void(*cb)(void* obj);
    void* data;
    SSW_TaskMode_t mode;
    SDDS_usec_t usec_interval;
    clock_time_t sec_interval;
    Task next;
};

static Task Task_list = NULL;
static struct etimer task_timer;

PROCESS(task_process, "task execution process");

/**
 * Should be called at the init phase
 */
ssw_rc_t TaskMng_init(void) {
	process_start(&task_process, NULL);
	return SDDS_SSW_RT_OK;
}

Task Task_create(void) {
	Task it = Task_list;
	if (it == NULL) {
		Task_list = Memory_alloc(sizeof(struct Task_struct));
		Task_list->next = NULL;

		return Task_list;
	} else {

		while (it->next != NULL) {
			it = it->next;
		}

		Task task = Memory_alloc(sizeof(struct Task_struct));
		task->next = NULL;

		it->next = task;

		return task;
	}
}

/**
 * inits a task with a callback function etc
 */
ssw_rc_t Task_init(Task _this, void(*callback)(void* obj), void* data) {
	_this->cb = callback;
	_this->data = data;

	return SDDS_SSW_RT_OK;
}

ssw_rc_t Task_start(Task _this, uint8_t sec, SDDS_usec_t usec, SSW_TaskMode_t mode) {
	_this->usec_interval = usec;
	_this->sec_interval = sec * CLOCK_SECOND;
	_this->mode = mode;

	etimer_set(&(_this->timer), _this->sec_interval);

	return SDDS_SSW_RT_OK;
}

ssw_rc_t Task_stop(Task _this) {
	etimer_stop(&(_this->timer));

	return SDDS_SSW_RT_OK;
}

ssw_rc_t Task_delete(Task _this) {
	Task it = Task_list;

	if (it == NULL) {
		// task list i empty...something went wrong
		return SDDS_SSW_RT_FAIL;
	}
	else if (it == _this) {
		// delete first element
		Task_list = _this->next;
		Memory_free(_this);
		return SDDS_SSW_RT_OK;
	}
	else {
		// look up _this on task_list
		while ((it != NULL) && (it->next != _this)) {
			it = it->next;
		}

		if (it != NULL) {
			// found _this, delete it
			it->next = _this->next;
			Memory_free(_this);
			return SDDS_SSW_RT_OK;
		}
	}
	// didn't found _this---something went wrong
	return SDDS_SSW_RT_FAIL;
}

static clock_time_t Task_checkTimer() {
	Task it = Task_list;

	clock_time_t nextTimer = 0;
	clock_time_t tmp = 0;

	while (it != NULL) {
		if (etimer_expired(&(it->timer))) {
			it->cb(it->data);
			if (it->mode == SDDS_SSW_TaskMode_repeat) {
				etimer_reset(&(it->timer));
			}
		}

		tmp = etimer_expiration_time(&(it->timer)) - clock_time();
		if (nextTimer == 0) {
			nextTimer = tmp;
		}
		else if (tmp < nextTimer) {
			nextTimer = tmp;
		}

		it = it->next;
	}

	if (nextTimer <= 0) {
		nextTimer = CLOCK_SECOND;
	}
	return nextTimer;
}

PROCESS_THREAD(task_process, ev, data)
{
	  PROCESS_BEGIN();
	  clock_time_t nextTime;

	  nextTime = (etimer_next_expiration_time() - clock_time());
	  nextTime = (nextTime < 0) ? nextTime : CLOCK_SECOND;
	  etimer_set(&task_timer, nextTime);
	  while(1) {
			nextTime = Task_checkTimer();

			etimer_set(&task_timer, nextTime);

			PROCESS_YIELD_UNTIL(etimer_expired(&task_timer));
	  }

	  PROCESS_END();
}
