/*
 * Task.c
 *
 *  Created on: Jul 22, 2010
 *      Author: kai
 */

#include "os-ssal/Task.h"

/**
 * Should be called at the init phase
 */
ssw_rc_t
TaskMng_init(void) {

    return SDDS_SSW_RT_OK;
}

Task
Task_create(void) {
    return NULL;
}

/**
 * inits a task with a callback function etc
 */
ssw_rc_t
Task_init(Task _this, void (* callback)(void* obj), void* data) {
    return SDDS_SSW_RT_OK;
}

ssw_rc_t
Task_start(Task _this, uint8_t sec, SDDS_usec_t usec, SSW_TaskMode_t mode) {
    return SDDS_SSW_RT_OK;
}
ssw_rc_t
Task_stop(Task _this) {
    return SDDS_SSW_RT_OK;
}
ssw_rc_t
Task_delete(Task _this) {
    return SDDS_SSW_RT_OK;

}