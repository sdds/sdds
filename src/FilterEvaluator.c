/*
 * FilterEvaluator.c
 *
 *  Created on: Aug 29, 2016
 *      Author: o_dedi
 */

#include "sDDS.h"
#include "FilterEvaluator.h"

#if defined(FEATURE_SDDS_LOCATION_FILTER_ENABLED) && defined(FEATURE_SDDS_SUBSCRIPTION_MANAGER_ENABLED)

rc_t
FilterEvaluator_evalSubscription(DeviceLocation_t* sample, DirectedEdge_t* edge) {
    assert(sample);
    assert(edge);

    if (edge->filteredSubscription == false) {
        return SDDS_RT_OK;
    }

    List_t* locTopics = edge->locTopics;
    LocationFilteredTopic_t* locTopic = locTopics->first_fn(locTopics);
    while (locTopic != NULL) {
        rc_t ret = LocationFilteredTopic_evalExpression(locTopic, sample);
        if (ret == SDDS_RT_OK) {
            return SDDS_RT_OK;
        }
        locTopic = locTopics->next_fn(locTopics);
    }

    return SDDS_RT_FAIL;
}

#endif
