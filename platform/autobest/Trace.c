#include <os-ssal/Trace.h>

#include <api_calls.h>

#ifdef FEATURE_SDDS_TRACEING_ENABLED

ssw_rc_t
Trace_init(void){
	return SDDS_RT_OK;
}

ssw_rc_t
Trace_point(Trace_events_t event){
	sys_kldd_call(0, event, 0, 0);
	return SDDS_RT_OK;
}
	
#endif