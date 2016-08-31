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

static SubscriptionManager_t s_subscriptionManager;

rc_t
SubscriptionManagementService_init() {
    return SubscriptionManager_init(&s_subscriptionManager);
}

rc_t
SubscriptionManagementService_evalFilteredSubscription(DeviceLocation_t* sample) {
    return SubscriptionManager_evalFilteredSubscription(&s_subscriptionManager, sample);
}

rc_t
SubscriptionManagementService_handleParticipant(SDDS_DCPSParticipant* sample) {
    return SubscriptionManager_handleParticipant(&s_subscriptionManager, sample);
}

rc_t
SubscriptionManagementService_handlePupblication(DDS_DCPSPublication* sample) {
    return SubscriptionManager_handlePupblication(&s_subscriptionManager, sample);
}

rc_t
SubscriptionManagementService_handleSubscription(SDDS_DCPSSubscription* sample) {
    return SubscriptionManager_handleSubscription(&s_subscriptionManager, sample);
}


rc_t
SubscriptionManagementService_registerFilter(SDDS_DCPSManagement* sample) {
    return SubscriptionManager_registerFilter(&s_subscriptionManager, sample);
}
