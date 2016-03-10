/*
 * =====================================================================================
 *
 *       Filename:  Mutex.c
 *
 *    Description:  Impl for the mutex interface for RIOT-OS
 *
 *        Version:  1.0
 *        Created:  25.02.2016 11:33:54
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Kai Beckmann (kai), kai-oliver.beckmann@hs-rm.de
 *        Company:  Hochschule RheinMain - DOPSY Labor für verteilte Systeme
 *
 * =====================================================================================
 */
#include "os-ssal/Mutex.h"
#include "Log.h"
#include "os-ssal/Memory.h"
#include "os-ssal/SSW.h"

// from RIOT-OS
#include "mutex.h"
//#include "sema.h"

//typedef struct sema_t struct mutex_t;
struct Mutex {
    struct mutex_t m;
};

Mutex_t*
Mutex_create(void) {
 //   struct sema_t* mutex = (struct sema_t*) Memory_alloc(sizeof(struct 
 //   sema_t));
    struct mutex_t* mutex = (struct mutex_t*) Memory_alloc(sizeof(struct Mutex));
    return (Mutex_t*) mutex;
}

ssw_rc_t
Mutex_init(Mutex_t* mutex) {
//    int ret = sema_create(mutex, 1);
    mutex_init((struct mutex_t*) mutex);
//    if (ret != 0) {
//        return SDDS_SSW_RT_FAIL;
//    }
    return SDDS_SSW_RT_OK;
}

ssw_rc_t
Mutex_lock(Mutex_t* mutex) {
    /*  
    int ret = sema_wait(mutex);
    if (ret == 0) {
        return SDDS_SSW_RT_OK;
    } else {
        return SDDS_SSW_RT_FAIL;
    }
    */
    mutex_lock((struct mutex_t*) mutex);

    return SDDS_SSW_RT_OK;
}

ssw_rc_t
Mutex_unlock(Mutex_t* mutex) {
/*  
    int ret = sema_post(mutex);
    if (ret == 0) {
        return SDDS_SSW_RT_OK
    } else {
        return SDDS_SSW_RT_FAIL;
    }
*/
    mutex_unlock((struct mutex_t*) mutex);

    return SDDS_SSW_RT_OK;
}

