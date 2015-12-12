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

#include <pthread.h>
#include <errno.h>

struct Mutex;
typedef struct Mutex Mutex_t;

/**
 * -----------------------------------------------------------------------------
 * Creates a mutes object
 *
 * @return  Returns the new mutex object.
 */
Mutex_t*
Mutex_create(void) {
    pthread_mutex_t* mutex = Memory_alloc(sizeof(pthread_mutex_t));
    if (mutex == NULL) {
        Log_error("No memory\n");
        return NULL;
    }
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
    int ret = pthread_mutex_init((pthread_mutex_t*)mutex, NULL);
    if (ret != 0) {
        Log_error("(%d) %s\n", __LINE__, strerror(errno));
        return SDDS_SSW_RT_FAIL;
    }
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
    int ret = pthread_mutex_lock((pthread_mutex_t*)mutex);
    if (ret != 0) {
        Log_error("(%d) %s\n", __LINE__, strerror(errno));
        return SDDS_SSW_RT_FAIL;
    }
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
    int ret = pthread_mutex_unlock((pthread_mutex_t*)mutex);
    if (ret != 0) {
        Log_error("(%d) %s\n", __LINE__, strerror(errno));
        return SDDS_SSW_RT_FAIL;
    }
    return SDDS_SSW_RT_OK;
}
