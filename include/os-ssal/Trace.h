#ifndef TRACE_H
#define TRACE_H

#include <local_constants.h>
#include "SSW.h"
#include "sdds_types.h"


#ifdef FEATURE_SDDS_TRACING_ENABLED

/*
 *  Define sDDS posible trace events for sDDS.
 *  The Events are only in use when the traceing feature is defined and
 *  the feature for the ebents are defined.
 */
#define SDDS_TRACE_EVENT_STOP				0u

#ifdef FEATURE_SDDS_TRACING_RECV_PAKET
#define SDDS_TRACE_EVENT_RECV_PAKET			3u
#endif

#ifdef FEATURE_SDDS_TRACING_PROCESS_FRAME
#define SDDS_TRACE_EVENT_PROCESS_FRAME		4u
#endif

#ifdef FEATURE_SDDS_TRACING_HISTORY_ENQUEUE
#define SDDS_TRACE_EVENT_HISTORY_ENQUEUE	5u
#endif

#ifdef FEATURE_SDDS_TRACING_DATA_WRITE
#define SDDS_TRACE_EVENT_DATA_WRITE			6u
#endif

#ifdef FEATURE_SDDS_TRACING_HISTORY_DEQUEUE
#define SDDS_TRACE_EVENT_HISTORY_DEQUEUE	7U
#endif

#ifdef FEATURE_SDDS_TRACING_SEND_PAKET
#define SDDS_TRACE_EVENT_SEND_PAKET			8u
#endif

#ifdef FEATURE_SDDS_TRACING_SIGNAL_RECV_PAKET
#define SDDS_TRACE_SIGNAL_RECV_PAKET		6u
#endif

#ifdef FEATURE_SDDS_TRACING_SIGNAL_SEND_PAKET
#define SDDS_TRACE_SIGNAL_SEND_PAKET		7u
#endif


typedef uint8_t Trace_event_t;
typedef uint8_t Trace_signal_t;

ssw_rc_t
Trace_init(void);

ssw_rc_t
Trace_point(Trace_event_t trace_event);

ssw_rc_t 
Trace_setSignal(Trace_signal_t trace_signal);

ssw_rc_t 
Trace_resetSignal(Trace_signal_t trace_signal);

#endif

#endif