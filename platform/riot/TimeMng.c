/*
 * =====================================================================================
 *
 *       Filename:  TimeMng.c
 *
 *    Description:  Timemanagement interface impl. for RIOT-OS
 *
 *        Version:  1.0
 *        Created:  25.02.2016 13:22:20
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Kai Beckmann (kai), kai-oliver.beckmann@hs-rm.de
 *        Company:  Hochschule RheinMain - DOPSY Labor für verteilte Systeme
 *
 * =====================================================================================
 */

#include <os-ssal/TimeMng.h>
#include <sdds_types.h>
#include "Log.h"

// virtual timer interface if RIOT-OS
#include "vtimer.h"

rc_t
TimeMng_init(void) {

    // shoud be called only once ...
	// normaly it is called during auto_init?
//    vtimer_init();
    return SDDS_RT_OK;
}

rc_t
Time_getTime16(time16_t* time) {
    timex_t now;
    // get current system time
    xtimer_now_timex(&now);
    // seconds to milliseconds
    *time = (time16_t) now.seconds * 1000;
    // micorseconds to milliseconds
    *time += (time16_t) (now.microseconds * 1000);

    return SDDS_RT_OK;
}

rc_t
Time_remainMSec16(time16_t* refTime, msec16_t* remainingMSec) {
    time16_t time;
    int ret;
    ret = Time_getTime16(&time);

    if (ret != SDDS_RT_OK) {
        return ret;
    }

    *remainingMSec = *refTime - time;
            
    return SDDS_RT_OK;
}

rc_t
Time_getTime32(time32_t* time) {
    timex_t now;
    // get current system time
    xtimer_now_timex(&now);
	// seconds to milliseconds
    *time = (time32_t) now.seconds * 1000;
    // micorseconds to milliseconds
    *time += (time32_t) (now.microseconds * 1000);

    return SDDS_RT_OK;
}
rc_t
Time_remainMSec32(time32_t* refTime, msec32_t* remainingMSec) {
    time32_t time;
    int ret;
    ret = Time_getTime32(&time);

    if (ret != SDDS_RT_OK) {
        return ret;
    }

   *remainingMSec = *refTime - time;
   return SDDS_RT_OK;
}
