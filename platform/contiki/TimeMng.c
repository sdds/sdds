/*
 * =====================================================================================
 *
 *       Filename:  TimeMng.c
 *
 *    Description:  class to handle all aspects regarding time
 *
 *        Version:  1.0
 *        Created:  30.11.2015
 *
 *         Author:  Olga Dedi, olga.b.dedi@student.hs-rm.de
 *
 * =====================================================================================
 */

#include <os-ssal/TimeMng.h>
#include <sdds_types.h>
#include <sys/clock.h>

rc_t TimeMng_init() {
	clock_init ();
	return SDDS_RT_OK;
}

rc_t Time_getTime32(time32_t* time) {
	clock_time_t t = clock_time();
	t = t / (CLOCK_SECOND / 1000);
	*time = (time32_t *)t;
	return SDDS_RT_OK;
}

rc_t Time_getTime16(time16_t* time) {
	clock_time_t t = clock_time();
	t = t / (CLOCK_SECOND / 1000);
	*time = (time16_t *)t;
	return SDDS_RT_OK;
}

rc_t Time_remainMSec32(time32_t* refTime, msec32_t* remainingMSec) {
	time32_t time;
	int ret;
	ret = Time_getTime32(&time);

	if (ret != SDDS_RT_OK) {
		return ret;
	}

	*remainingMSec = *refTime - time;
	return SDDS_RT_OK;
}

rc_t Time_remainMSec16(time16_t* refTime, msec16_t* remainingMSec) {
	time16_t time;
	int ret;
	ret = Time_getTime16(&time);

	if (ret != SDDS_RT_OK) {
		return ret;
	}

	*remainingMSec = *refTime - time;
	return SDDS_RT_OK;
}

