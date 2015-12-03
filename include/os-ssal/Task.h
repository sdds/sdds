/*
 * Task.h
 *
 *  Created on: Jul 22, 2010
 *      Author: kai
 */

#ifndef TASK_H_
#define TASK_H_

#include "SSW.h"
#include "sdds_types.h"

typedef uint8_t SSW_TaskMode_t;
#define SDDS_SSW_TaskMode_single 0
#define SDDS_SSW_TaskMode_repeat 1

struct Task_struct;
typedef struct Task_struct* Task;

/**
 * Should be called at the init phase
 */
ssw_rc_t TaskMng_init(void);

Task Task_create(void);

/**
 * inits a task with a callback function etc
 */
ssw_rc_t Task_init(Task _this, void(*callback)(void* obj), void* data);
ssw_rc_t Task_setData(Task _this, void* data);

ssw_rc_t Task_start(Task _this, uint8_t sec, SDDS_msec_t msec, SSW_TaskMode_t mode);
ssw_rc_t Task_stop(Task _this);
ssw_rc_t Task_delete(Task _this);


#endif /* TASK_H_ */
