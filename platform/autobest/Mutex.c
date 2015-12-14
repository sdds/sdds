/*
 * Mutex.h
 *
 *  Created on: Dec 14, 2015
 *      Author: andreas
 */

#include "os-ssal/SSW.h"
#include "os-ssal/Mutex.h"
#include "os-ssal/Memory.h"
#include "Log.h"

#include <api_calls.h>
#include <lwip/sys.h>

#define UNLOCK_STATE    0
#define LOCK_STATE      1

struct Mutex{
    uint8_t old_prio;
    uint8_t state:1;
};
typedef struct Mutex Mutex_t;

/**
 * -----------------------------------------------------------------------------
 * Creates a mutes object
 *
 * @return  Returns the new mutex object.
 */
Mutex_t*
Mutex_create(void) {
    Mutex_t* mutex = Memory_alloc(sizeof(Mutex_t));
    return (Mutex_t*) mutex;
}

/**
 * -----------------------------------------------------------------------------
 * Initializes the mutex.
 *
 * @param[in]   Mutex which should be initialized.
 * @return      Returns SDDS_SSW_RT_OK on success and SDDS_SSW_RT_FAIL on failure.
 */
ssw_rc_t
Mutex_init(Mutex_t* mutex) {
    mutex->state = UNLOCK_STATE;
    return SDDS_SSW_RT_OK;
}

/**
 * -----------------------------------------------------------------------------
 * Locks the mutex.
 *
 * @param[in]   Mutex which should be locked.
 * @return      Returns SDDS_SSW_RT_OK on success and SDDS_SSW_RT_FAIL on failure.
 */
ssw_rc_t
Mutex_lock(Mutex_t* mutex) {
    if(mutex == NULL){
        return SDDS_SSW_RT_FAIL;
    }
    if(mutex->state == LOCK_STATE){
        return SDDS_SSW_RT_OK;
    }
    mutex->old_prio =  sys_arch_protect();
    mutex->state = LOCK_STATE;
    return SDDS_SSW_RT_OK;
}

/**
 * -----------------------------------------------------------------------------
 * Unlocks the mutex.
 *
 * @param[in]   Mutex which should be unlocked.
 * @return     Returns SDDS_SSW_RT_OK on success and SDDS_SSW_RT_FAIL on failure.
 */
ssw_rc_t
Mutex_unlock(Mutex_t* mutex) {
    if(mutex == NULL){
        return SDDS_SSW_RT_FAIL;
    }
    if(mutex->state == UNLOCK_STATE){
        return SDDS_SSW_RT_OK;
    }
    sys_arch_unprotect(mutex->old_prio);
    mutex->state = UNLOCK_STATE;
    return SDDS_SSW_RT_OK;
}