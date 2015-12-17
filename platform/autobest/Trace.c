#include <os-ssal/Trace.h>
#include <local_constants.h>

#include <api_calls.h>

#ifdef FEATURE_SDDS_TRACING_ENABLED

ssw_rc_t
Trace_init(void){
	return SDDS_RT_OK;
}

ssw_rc_t
Trace_point(Trace_event_t trace_event){
	sys_kldd_call(0, trace_event, 0, 0);
	return SDDS_RT_OK;
}

ssw_rc_t 
Trace_setSignal(Trace_signal_t trace_signal){
	sys_kldd_call(3, trace_signal, 0, 0);
	return SDDS_RT_OK;
}

ssw_rc_t 
Trace_resetSignal(Trace_signal_t trace_signal){
	sys_kldd_call(4, trace_signal, 0, 0);
	return SDDS_RT_OK;
}
	
#endif