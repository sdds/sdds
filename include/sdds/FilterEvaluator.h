/*
 * FilterEvaluator.h
 *
 *  Created on: Aug 29, 2016
 *      Author: o_dedi
 */

#ifndef SDDS_INCLUDE_SDDS_FILTEREVALUATOR_H_
#define SDDS_INCLUDE_SDDS_FILTEREVALUATOR_H_

#include "sDDS.h"
#include "SubscriptionGraph.h"

rc_t
FilterEvaluator_evalSubscription(DeviceLocation_t* sample, DirectedEdge_t* edge);


#endif /* SDDS_INCLUDE_SDDS_FILTEREVALUATOR_H_ */
