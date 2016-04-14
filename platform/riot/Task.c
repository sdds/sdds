#include <os-ssal/Task.h>
#include <os-ssal/Memory.h>
#include <sdds/Log.h>

#include <stdlib.h>
#include <string.h>

#include <xtimer.h>
#include <timex.h>
#include <thread.h>
#include <msg.h>
#include <ps.h>


#define MSG_QUEUE_SIZE  (8)

#define SDDS_TASKMNG_STACK_SIZE (THREAD_STACKSIZE_DEFAULT)

#define SDDS_TASKMNG_WORKER_TIMEOUT (60000000U)
static char msg_taskMng_stack[SDDS_TASKMNG_STACK_SIZE];
static msg_t msg_taskMng_queue[MSG_QUEUE_SIZE];

struct Task_struct {

    xtimer_t timer;
    msg_t msg;

    void (* callback)(void* obj);
    void* data;
    uint32_t interval_usecs;

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

    // init msg queue
     msg_init_queue(msg_taskMng_queue, MSG_QUEUE_SIZE);

    for(;; )
    {
        int ret = xtimer_msg_receive_timeout(&m, SDDS_TASKMNG_WORKER_TIMEOUT);
        if (ret < 0 ) {
            // no data
            Log_debug("No Task sceduled the last %d seconds interval\n",
                    SDDS_TASKMNG_WORKER_TIMEOUT/1000000);

            //ps();
            continue;
        }

        t = (Task) m.content.ptr;

        if(t->mode == SDDS_SSW_TaskMode_repeat) {
            xtimer_set_msg(&t->timer, t->interval_usecs, &t->msg, TaskMngPid);
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
    TaskMngPid = thread_create(msg_taskMng_stack, SDDS_TASKMNG_STACK_SIZE,
                               THREAD_PRIORITY_MAIN, THREAD_CREATE_STACKTEST,
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
    _this->msg.content.ptr = (char*) _this;

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
    _this->interval_usecs = (((uint32_t) sec) * 1000000) + usec;

    xtimer_set_msg(&_this->timer, _this->interval_usecs, &_this->msg, TaskMngPid);

    _this->active = true;

    return SDDS_SSW_RT_OK;
}

ssw_rc_t
Task_stop(Task _this) {
    xtimer_remove(&_this->timer);
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
