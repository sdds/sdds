/*
 * Mutex.h
 *
 *  Created on: Dec 10, 2015
 *      Author: olga
 */

#include "os-ssal/SSW.h"
#include "os-ssal/Mutex.h"
#include "os-ssal/Memory.h"
#include "Log.h"

#include "contiki.h"

struct Mutex {
    uint8_t value;
};

/**
 * -----------------------------------------------------------------------------
 * Creates a mutex object
 *
 * @return  Returns the new mutex object.
 */
Mutex_t*
Mutex_create(void) {
    return (Mutex_t*) Memory_alloc(sizeof(Mutex_t));
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
    mutex->value = 1;
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
    if (mutex->value != 0) {
        cli();
        mutex->value = 0;
        return SDDS_SSW_RT_OK;
    }
    return SDDS_SSW_RT_FAIL;
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
    mutex->value = 1;
    sei();
    return SDDS_SSW_RT_OK;
}
