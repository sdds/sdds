/*
 * SubscriptionManager.h
 *
 *  Created on: Aug 29, 2016
 *      Author: o_dedi
 */

#ifndef SDDS_INCLUDE_SDDS_SUBSCRIPTIONMANAGER_H_
#define SDDS_INCLUDE_SDDS_SUBSCRIPTIONMANAGER_H_

#include "sDDS.h"
#include "SubscriptionGraph.h"
#include "FilterEvaluator.h"
#include "ManagementTopic.h"

struct SubscriptionManager {
    SubscriptionGraph_t subscriptionGraph;
};
typedef struct SubscriptionManager SubscriptionManager_t;

rc_t
SubscriptionManager_init(SubscriptionManager_t* self);

rc_t
SubscriptionManager_evalFilteredSubscription(SubscriptionManager_t* self, DeviceLocation_t* sample);

rc_t
SubscriptionManager_handleParticipant(SubscriptionManager_t* self, SDDS_DCPSParticipant* sample);

rc_t
SubscriptionManager_handlePublication(SubscriptionManager_t* self, DDS_DCPSPublication* sample);

rc_t
SubscriptionManager_handleSubscription(SubscriptionManager_t* self, SDDS_DCPSSubscription* sample);

rc_t
SubscriptionManager_registerFilter(SubscriptionManager_t* self, SDDS_DCPSManagement* sample);

#endif /* SDDS_INCLUDE_SDDS_SUBSCRIPTIONMANAGER_H_ */
