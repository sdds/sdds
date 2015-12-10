
#ifdef FEATURE_SDDS_TRACEING_ENABLED

#ifndef TRACE_H
#define TRACE_H

enum Trace_events {
	EVENT_0,
	EVENT_1,
	EVENT_2,
	EVENT_3,
	EVENT_4,
	EVENT_5,
	EVENT_6,
	EVENT_7,
	EVENT_8,
	EVENT_9,
	EVENT_10,
	EVENT_11,
	EVENT_12,
	EVENT_13,
	EVENT_14,
	EVENT_15
 };

typedef enum Trace_events Trace_events_t;

ssw_rc_t
Trace_init(void);

ssw_rc_t
Trace_point(Trace_events_t event);

#endif

#endif