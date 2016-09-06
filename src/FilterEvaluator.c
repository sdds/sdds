/*
 * FilterEvaluator.c
 *
 *  Created on: Aug 29, 2016
 *      Author: o_dedi
 */

#include "sDDS.h"
#include "FilterEvaluator.h"

#ifdef FEATURE_SDDS_LOCATION_FILTER_ENABLED

rc_t
FilterEvaluator_evalSubscription(DeviceLocation_t* sample, DirectedEdge_t* edge) {
    assert(sample);
    assert(edge);

    if (edge->filteredSubscription == false) {
        return SDDS_RT_OK;
    }

    return LocationFilteredTopic_evalExpression(edge->locTopic, sample);
}

#endif
