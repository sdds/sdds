#ifndef TRACE_H
#define TRACE_H

#include <local_constants.h>
#include "SSW.h"
#include "sdds_types.h"


#ifdef FEATURE_SDDS_TRACEING_ENABLED

typedef uint8_t Trace_events_t;

ssw_rc_t
Trace_init(void);

ssw_rc_t
Trace_point(Trace_events_t event);

#endif

#endif