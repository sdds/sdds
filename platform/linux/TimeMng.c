#include <TimeMng.h>
#include <sdds_types.h>
#include <sys/time.h>
#include "Log.h"

rc_t Time_getCurTime(pointInTime_t* curTime) {
	struct timeval tv;
	int ret = gettimeofday(&tv, NULL);

	*curTime = (uint16_t)tv.tv_sec;

	return SDDS_RT_OK;
}
