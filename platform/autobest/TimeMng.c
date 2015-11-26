#include <TimeMng.h>
#include <sdds_types.h>
#include "Log.h"
#include <api_calls.h>

#define TIME_MNG_DIV_SIFT_TO_SEC 30


rc_t Time_getCurTime(pointInTime_t* curTime) {
	time_t systemTime = sys_gettime();

	*curTime = (uint16_t)(systemTime >> TIME_MNG_DIV_SIFT_TO_SEC);

	return SDDS_RT_OK;
}
