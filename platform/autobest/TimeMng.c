#include <os-ssal/TimeMng.h>
#include <sdds_types.h>
#include "Log.h"
#include <api_calls.h>

#define TIME_MNG_DIV_SIFT_TO_SEC 20

rc_t TimeMng_init() {
	return SDDS_RT_OK;
}

rc_t Time_getTime32(time32_t* time) {
	time_t systemTime = sys_gettime();

	*time = (time32_t)(systemTime >> TIME_MNG_DIV_SIFT_TO_SEC);

	return SDDS_RT_OK;
}

rc_t Time_getTime16(time16_t* time) {
	time_t systemTime = sys_gettime();

	*time = (time16_t)(systemTime >> TIME_MNG_DIV_SIFT_TO_SEC);

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
