/*
 * =====================================================================================
 *
 *       Filename:  TimeMng.c
 *
 *    Description:  class to handle all aspects regarding time
 *
 *        Version:  1.0
 *        Created:  27.11.2015
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Olga Dedi, olga.b.dedi@student.hs-rm.de
 *
 * =====================================================================================
 */

#include <os-ssal/TimeMng.h>
#include <sdds_types.h>
#include <sys/time.h>

rc_t
TimeMng_init() {
    return SDDS_RT_OK;
}

rc_t
Time_getTime32(time32_t* time) {
    struct timeval tv;
    uint64_t tmp;
    int ret = gettimeofday(&tv, NULL);

    if (ret < 0) {
        return SDDS_RT_FAIL;
    }

    // cut the leading 32 bits off
    tmp = (uint32_t) tv.tv_sec;
    // convert seconds to milliseconds
    tmp  = tmp * 1000;
    // add remaining milliseconds
    tmp = tmp + (tv.tv_usec / 1000);
    // cut the leading 32 bits off
    *time = (uint32_t) tmp;

    return SDDS_RT_OK;
}

rc_t
Time_getTime16(time16_t* time) {
    struct timeval tv;
    uint64_t tmp;
    int ret = gettimeofday(&tv, NULL);

    if (ret < 0) {
        return SDDS_RT_FAIL;
    }

    // cut the leading 32 bits off
    tmp = (uint16_t) tv.tv_sec;
    // convert seconds to milliseconds
    tmp  = tmp * 1000;
    // add remaining milliseconds
    tmp = tmp + (tv.tv_usec / 1000);
    // cut the leading 32 bits off
    *time = (uint16_t) tmp;

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