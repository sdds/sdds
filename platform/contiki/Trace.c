#include <os-ssal/Trace.h>
#include <avr/io.h>

#ifdef FEATURE_SDDS_TRACING_ENABLED

ssw_rc_t
Trace_init(void){
    DDRE |= (1 << PE4);
    DDRE |= (1 << PE5);
    DDRE |= (1 << PE7);
    DDRF |= (1 << PF0);

	return SDDS_RT_OK;
}

ssw_rc_t
Trace_point(Trace_event_t trace_event){
    bool_t pin0 = (trace_event & 1);
    bool_t pin1 = (trace_event & 2);
    bool_t pin2 = (trace_event & 4);
    bool_t pin3 = (trace_event & 8);

    PORTE &= 0x00;
    PORTF &= 0x00;

    PORTE |= (pin0 << PE4);
    PORTF |= (pin1 << PF0);
    PORTE |= (pin2 << PE5);
    PORTE |= (pin3 << PE7);

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
