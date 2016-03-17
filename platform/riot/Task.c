#include <os-ssal/Task.h>
#include <os-ssal/Memory.h>
#include <sdds/Log.h>

#include <stdlib.h>
#include <string.h>

#include "vtimer.h"
#include "timex.h"
#include "thread.h"
#include "msg.h"

struct Task_struct {

    vtimer_t timer;

    void (* callback)(void* obj);
    void* data;
    timex_t interval;
    SSW_TaskMode_t mode;
    bool active;
};

static int TaskMngPid;

/*void Task_callback(void* task){
    Task t = (Task) task;

    t->callback(t->data);
   }*/

void*
TaskMngLoop(void* foo) {
    (void)foo;

    msg_t m;
    Task t;
    for(;; )
    {
        msg_receive(&m);
        t = (Task) m.content.value;
        if(t->mode == SDDS_SSW_TaskMode_repeat) {
            vtimer_set_msg(&t->timer, t->interval, TaskMngPid, 0, t);
        }
        t->callback(t->data);
        if (t->mode == SDDS_SSW_TaskMode_single)  {
            t->active = false;
        }
    }
}

/**
 * Should be called at the init phase
 */
ssw_rc_t
TaskMng_init(void) {
    int taskMng_stack_size = THREAD_STACKSIZE_DEFAULT * sizeof(char);
    char* stack_thread = (char*) Memory_alloc(taskMng_stack_size);
	if (stack_thread == NULL) {
		assert(false);
		Log_error("No memory");
        return SDDS_SSW_RT_FAIL;
	}
    TaskMngPid = thread_create(stack_thread, taskMng_stack_size,
                               THREAD_PRIORITY_MAIN, 0,
                               TaskMngLoop, NULL, "TaskMng sDDS");
    if(TaskMngPid < 0) {
		Log_error("Can't start task management thread. retcode %d", TaskMngPid);
        return SDDS_SSW_RT_FAIL;
    }
    return SDDS_SSW_RT_OK;
}

Task
Task_create(void) {
    Task t = Memory_alloc(sizeof(struct Task_struct));
    return t;
}

/**
 * inits a task with a callback function etc
 */
ssw_rc_t
Task_init(Task _this, void (* callback)(void* obj), void* data) {
    if (_this == NULL || callback == NULL) {
        return SDDS_SSW_RT_FAIL;
    }

    _this->callback = callback;
    _this->data = data;
    _this->active = false;

    return SDDS_SSW_RT_OK;
}

bool
Task_isRunning(Task _this)
{
    assert (_this);
    return _this->active;
}

ssw_rc_t
Task_start(Task _this, uint8_t sec, SDDS_usec_t usec, SSW_TaskMode_t mode) {
    _this->mode = mode;
    _this->interval.seconds = sec;
    _this->interval.microseconds = usec;

    vtimer_set_msg(&_this->timer, _this->interval, TaskMngPid, 0, _this);
    _this->active = true;
    return SDDS_SSW_RT_OK;
}

ssw_rc_t
Task_stop(Task _this) {
    vtimer_remove(&_this->timer);
    _this->active = false;
    return SDDS_SSW_RT_OK;
}

ssw_rc_t
Task_setData(Task _this, void* data) {
    if (_this == NULL) {
        return SDDS_SSW_RT_FAIL;
    }

    _this->data = data;

    return SDDS_SSW_RT_OK;
}

ssw_rc_t
Task_delete(Task _this) {
    if (_this == NULL) {
        return SDDS_SSW_RT_FAIL;
    }
    Task_stop(_this);

    Memory_free(_this);

    return SDDS_SSW_RT_OK;

}
