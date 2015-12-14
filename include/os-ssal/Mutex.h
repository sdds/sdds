/*
 * Mutex.h
 *
 *  Created on: 21 Feb 2011
 *      Author: k_beckma
 */

#ifndef MUTEX_H_
#define MUTEX_H_

#include "os-ssal/SSW.h"
struct Mutex;
typedef struct Mutex Mutex_t;

/**
 * -----------------------------------------------------------------------------
 * Creates a mutes object
 *
 * @return  Returns the new mutex object.
 */
Mutex_t*
Mutex_create(void);

/**
 * -----------------------------------------------------------------------------
 * Initializes the mutex.
 *
 * @param[in]   Mutex which should be initialized.
 * @return      Returns SDDS_SSW_RT_OK on success and SDDS_SSW_RT_FAIL on failure.
 */
ssw_rc_t
Mutex_init(Mutex_t* mutex);

/**
 * -----------------------------------------------------------------------------
 * Locks the mutex.
 *
 * @param[in]   Mutex which should be locked.
 * @return      Returns SDDS_SSW_RT_OK on success and SDDS_SSW_RT_FAIL on failure.
 */
ssw_rc_t
Mutex_lock(Mutex_t* mutex);

/**
 * -----------------------------------------------------------------------------
 * Unlocks the mutex.
 *
 * @param[in]   Mutex which should be unlocked.
 * @return     Returns SDDS_SSW_RT_OK on success and SDDS_SSW_RT_FAIL on failure.
 */
ssw_rc_t
Mutex_unlock(Mutex_t* mutex);

#endif /* MUTEX_H_ */
