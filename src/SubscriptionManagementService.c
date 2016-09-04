/*
 * SubscriptionManagementService.c
 *
 *  Created on: Aug 29, 2016
 *      Author: o_dedi
 */

#include "sDDS.h"
#include "SubscriptionManagementService.h"
#include "SubscriptionManager.h"
#include "ManagementTopic.h"

#ifdef FEATURE_SDDS_SUBSCRIPION_MANAGER_ENABLED

static SubscriptionManager_t s_subscriptionManager;

rc_t
SubscriptionManagementService_init() {
    return SubscriptionManager_init(&s_subscriptionManager);

    // TODO regelmäßig pause senden
}

#ifdef FEATURE_SDDS_LOCATION_ENABLED
rc_t
SubscriptionManagementService_evalFilteredSubscription(DeviceLocation_t* sample) {
    return SubscriptionManager_evalFilteredSubscription(&s_subscriptionManager, sample);
}
#endif

rc_t
SubscriptionManagementService_handleParticipant(SDDS_DCPSParticipant* sample) {
    return SubscriptionManager_handleParticipant(&s_subscriptionManager, sample);
}

rc_t
SubscriptionManagementService_handlePublication(DDS_DCPSPublication* sample) {
    return SubscriptionManager_handlePublication(&s_subscriptionManager, sample);
}

rc_t
SubscriptionManagementService_handleSubscription(SDDS_DCPSSubscription* sample) {
    return SubscriptionManager_handleSubscription(&s_subscriptionManager, sample);
}


rc_t
SubscriptionManagementService_registerFilter(SDDS_DCPSManagement* sample) {
    return SubscriptionManager_registerFilter(&s_subscriptionManager, sample);
}

#endif
