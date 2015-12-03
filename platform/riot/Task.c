#include <os-ssal/Task.h>
#include <os-ssal/Memory.h>

#include <stdlib.h>
#include <string.h>

#include "vtimer.h"
#include "timex.h"
#include "thread.h"
#include "msg.h"

struct Task_struct {

    vtimer_t timer;

    void (* cb)(void* obj);
    void* data;
    timex_t interval;
    SSW_TaskMode_t mode;
};

static int TaskMngPid;

/*void Task_callback(void* task){
    Task t = (Task) task;

    t->cb(t->data);
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
        t->cb(t->data);
    }
}

/**
 * Should be called at the init phase
 */
ssw_rc_t
TaskMng_init(void) {
    int taskMng_stack_size = KERNEL_CONF_STACKSIZE_PRINTF_FLOAT * sizeof(char);
    char* stack_thread = (char*) Memory_alloc(taskMng_stack_size);
    TaskMngPid = thread_create(stack_thread, taskMng_stack_size,
                               THREAD_PRIORITY_MAIN, 0,
                               TaskMngLoop, NULL, "TaskMng sDDS");
    if(TaskMngPid < 0) {
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

    _this->cb = callback;
    _this->data = data;

    return SDDS_SSW_RT_OK;
}

ssw_rc_t
Task_start(Task _this, uint8_t sec, SDDS_usec_t usec, SSW_TaskMode_t mode) {
    _this->mode = mode;
    _this->interval.seconds = sec;
    _this->interval.microseconds = usec;

    vtimer_set_msg(&_this->timer, _this->interval, TaskMngPid, 0, _this);
    return SDDS_SSW_RT_OK;
}

ssw_rc_t
Task_stop(Task _this) {
    vtimer_remove(&_this->timer);
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