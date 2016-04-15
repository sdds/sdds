#include <os-ssal/Trace.h>
#include <avr/io.h>

#ifdef FEATURE_SDDS_TRACING_ENABLED

ssw_rc_t
Trace_init(void){
	DDRB |= (1 << PB4);
	DDRB |= (1 << PB5);
	DDRB |= (1 << PB6);
	DDRB |= (1 << PB7);

	return SDDS_RT_OK;
}

ssw_rc_t
Trace_point(Trace_event_t trace_event){
	PORTB = (trace_event << 4);
	return SDDS_RT_OK;
}

ssw_rc_t
Trace_setSignal(Trace_signal_t trace_signal){
	return SDDS_RT_OK;
}

ssw_rc_t
Trace_resetSignal(Trace_signal_t trace_signal){
	return SDDS_RT_OK;
}

#endif
