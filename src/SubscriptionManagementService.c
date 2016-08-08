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

#define SDDS_SUBSCRIPTION_PAUSE_TIMER_SEC         1

#ifndef SDDS_SUBSCRIPTION_PAUSE_TIMER_MSEC
#define SDDS_SUBSCRIPTION_PAUSE_TIMER_MSEC        0
#endif


static SubscriptionManager_t s_subscriptionManager;

static Task pasueSubscriptionTask;

static void
s_pauseSubscription();

rc_t
SubscriptionManagementService_init() {
#   if (SDDS_SUBSCRIPTION_PAUSE_TIMER_SEC != 0) || (SDDS_SUBSCRIPTION_PAUSE_TIMER_MSEC != 0)
    pasueSubscriptionTask = Task_create();
    Task_init(pasueSubscriptionTask, s_pauseSubscription, NULL);
    if (Task_start(pasueSubscriptionTask, SDDS_SUBSCRIPTION_PAUSE_TIMER_SEC, SDDS_SUBSCRIPTION_PAUSE_TIMER_MSEC, SDDS_SSW_TaskMode_repeat) != SDDS_RT_OK) {
        Log_error("Task_start failed\n");
    }
#   endif

    return SubscriptionManager_init(&s_subscriptionManager);
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

static void
s_pauseSubscription() {
    List_t* edges = s_subscriptionManager.subscriptionGraph.edges;
    DirectedEdge_t* edge = (DirectedEdge_t*) edges->first_fn(edges);
    while (edge != NULL) {
        if (edge->sate == PAUSED) {
            rc_t ret = SubscriptionManager_publishSubscriptionState(edge);
            if (ret != SDDS_RT_OK) {
                Log_error("Unable to publish Subscription state.\n");
            }
        }
        edge = (DirectedEdge_t*) edges->next_fn(edges);
    }
}

#endif
