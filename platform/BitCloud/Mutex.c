/*
 * Mutex.c
 *
 *  Created on: 21 Feb 2011
 *      Author: k_beckma
 *
 *   Mutex impl for the avr bitcloud stack
 */

#include "os-ssal/Mutex.h"

#include <atomic.h>

struct Mutex_t {
    volatile atomic_t m;
};

Mutex Mutex_create(void)
{
    return NULL;
}

ssw_rc_t Mutex_init(Mutex mutex)
{
    mutex = mutex;
    return SDDS_SSW_RT_OK;
}

ssw_rc_t Mutex_up(Mutex mutex)
{
    if (mutex != NULL) {
	mutex->m = halStartAtomic();
	return SDDS_SSW_RT_OK;
    } else {
	return SDDS_SSW_RT_FAIL;
    }
}
ssw_rc_t Mutex_down(Mutex mutex){

    if (mutex != NULL){
	halEndAtomic(mutex->m);
	return SDDS_SSW_RT_OK;
    } else {
	return SDDS_SSW_RT_FAIL;
    }
}
