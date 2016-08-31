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
SubscriptionManagementService_registerFilter(SDDS_DCPSManagement* sample) {
    return SubscriptionManager_registerFilter(&s_subscriptionManager, sample);
}
