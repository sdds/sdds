#ifndef TRACE_H
#define TRACE_H

#include <local_constants.h>
#include "SSW.h"
#include "sdds_types.h"


#ifdef FEATURE_SDDS_TRACING_ENABLED

/*
 *  Define sDDS posible trace events for sDDS.
 *  The Events are only in use when the traceing feature is defined and
 *  the feature for the events are defined.
 *
 * There are two types of events for recving and sending,
 * the isolated and normal events.
 * isolated events are for trace of a sdds version which is
 * splittet into sdds middleware and sdds applications.
 * For this version we need more events.
 * At this time we only have ja isolated version for the
 * AUTOBEST kernel, so the trace events are accommodated to this
 * version.
 */
#define SDDS_TRACE_EVENT_STOP				0u

#ifdef FEATURE_SDDS_TRACING_RECV_NORMAL

#define SDDS_TRACE_EVENT_DUMMY_1            1u
#define SDDS_TRACE_EVENT_DUMMY_2            3u
#define SDDS_TRACE_EVENT_DUMMY_3           	2u

#ifdef FEATURE_SDDS_TRACING_RECV_PAKET
#define SDDS_TRACE_EVENT_RECV_PAKET			6u
#endif

#ifdef FEATURE_SDDS_TRACING_PROCESS_FRAME
#define SDDS_TRACE_EVENT_PROCESS_FRAME		7u
#endif

#ifdef FEATURE_SDDS_TRACING_HISTORY_ENQUEUE
#define SDDS_TRACE_EVENT_HISTORY_ENQUEUE	5u
#endif

#ifdef FEATURE_SDDS_TRACING_CALL_LISTNER
#define SDDS_TRACE_EVENT_CALL_LISTNER       4u
#endif

#endif // FEATURE_SDDS_TRACEING_RECV_NORMAL


#ifdef FEATURE_SDDS_TRACING_RECV_ISOLATED

#ifdef FEATURE_SDDS_TRACING_RECV_PAKET
#define SDDS_TRACE_EVENT_RECV_PAKET			6u
#endif

#ifdef FEATURE_SDDS_TRACING_PROCESS_FRAME
#define SDDS_TRACE_EVENT_PROCESS_FRAME		7u
#endif

#ifdef FEATURE_SDDS_TRACING_HISTORY_ENQUEUE
#define SDDS_TRACE_EVENT_HISTORY_ENQUEUE	5u
#endif

#ifdef FEATURE_SDDS_TRACING_SERVER_LISTNER
#define SDDS_TRACE_EVENT_SERVER_LISTNER     4u
#endif

#ifdef FEATURE_SDDS_TRACING_IPEV_LISTNER
#define SDDS_TRACE_EVENT_IPEV_LISTNER       12u
#endif

#ifdef FEATURE_SDDS_TRACING_CALL_MNG
#define SDDS_TRACE_EVENT_CALL_MNG           13u
#endif

#ifdef FEATURE_SDDS_TRACING_CALL_LISTNER
#define SDDS_TRACE_EVENT_CALL_LISTNER       15u
#endif

#ifdef FEATURE_SDDS_TRACING_RPC_CALL
#define SDDS_TRACE_EVENT_RPC_CALL           14u
#endif

#ifdef FEATURE_SDDS_TRACING_RPC_TASK
#define SDDS_TRACE_EVENT_RPC_TASK           10u
#endif

#ifdef FEATURE_SDDS_TRACING_RPC_REPLY
#define SDDS_TRACE_EVENT_RPC_REPLY          11u
#endif


#define SDDS_TRACE_EVENT_DUMMY_1            9u
#define SDDS_TRACE_EVENT_DUMMY_2            8u

#endif // FEATURE_SDDS_TRACEING_RECV_ISOLATED

#ifdef FEATURE_SDDS_TRACING_SEND_NORMAL

#ifdef FEATURE_SDDS_TRACING_CALL_WRITE
#define SDDS_TRACE_EVENT_CALL_WRITE			1u
#endif

#ifdef FEATURE_SDDS_TRACING_PREPARE_SNPS
#define SDDS_TRACE_EVENT_PREPARE_SNPS       3u
#endif

#ifdef FEATURE_SDDS_TRACING_SEND_PAKET
#define SDDS_TRACE_EVENT_SEND_PAKET	        2u
#endif

 // lwip: 6u
 // ETH: 7u

#define SDDS_TRACE_EVENT_DUMMY_3            6u
#define SDDS_TRACE_EVENT_DUMMY_4            7u

#define SDDS_TRACE_EVENT_DUMMY_1            5u
#define SDDS_TRACE_EVENT_DUMMY_2            4u

#endif // FEATURE_SDDS_TRACEING_SEND_NORMAL

#ifdef FEATURE_SDDS_TRACING_SEND_ISOLATED

#ifdef FEATURE_SDDS_TRACING_RPC_CALL
#define SDDS_TRACE_EVENT_RPC_CALL           1u
#endif

#ifdef FEATURE_SDDS_TRACING_RPC_TASK
#define SDDS_TRACE_EVENT_RPC_TASK           3u
#endif

#ifdef FEATURE_SDDS_TRACING_CALL_WRITE
#define SDDS_TRACE_EVENT_CALL_WRITE			2u
#endif

#ifdef FEATURE_SDDS_TRACING_PREPARE_SNPS
#define SDDS_TRACE_EVENT_PREPARE_SNPS       6u
#endif

#ifdef FEATURE_SDDS_TRACING_SEND_PAKET
#define SDDS_TRACE_EVENT_SEND_PAKET	        7u
#endif

// lwip: 5u

// eth:  4u

#ifdef FEATURE_SDDS_TRACING_RPC_REPLY
#define SDDS_TRACE_EVENT_RPC_REPLY          12u
#endif

#define SDDS_TRACE_EVENT_DUMMY_1            13u
#define SDDS_TRACE_EVENT_DUMMY_2            15u
#define SDDS_TRACE_EVENT_DUMMY_3            14u
#define SDDS_TRACE_EVENT_DUMMY_4            10u
#define SDDS_TRACE_EVENT_DUMMY_5            11u
#define SDDS_TRACE_EVENT_DUMMY_6            9u
#define SDDS_TRACE_EVENT_DUMMY_7            8u

#endif //FEATURE_SDDS_TRACEING_SEND_ISOLATED

typedef uint8_t Trace_event_t;
typedef uint8_t Trace_signal_t;

// Init function for the trace module
ssw_rc_t
Trace_init(void);

/*
 * function set a trace point in the code.
 * Trace point are places in the code to
 * show the external world what is happening in the system.
 * So to can interpret them as start point of phaseses in your
 * software.
 */
ssw_rc_t
Trace_point(Trace_event_t trace_event);

/*
 * functions set and reset trace signals.
 * Unlike trace events, trace signals are
 * signals to the external world of your system.
 * So you can give additinal infomations to phases
 * of your software e. g. when did start rescheduling
 * of a process or so on,
 */
ssw_rc_t
Trace_setSignal(Trace_signal_t trace_signal);

ssw_rc_t
Trace_resetSignal(Trace_signal_t trace_signal);

#endif

#endif
