/*
 * SubscriptionManagementService.h
 *
 *  Created on: Aug 29, 2016
 *      Author: o_dedi
 */

#ifndef SDDS_INCLUDE_SDDS_SUBSCRIPTIONMANAGEMENTSERVICE_H_
#define SDDS_INCLUDE_SDDS_SUBSCRIPTIONMANAGEMENTSERVICE_H_

#include "sDDS.h"
#include "ManagementTopic.h"

rc_t
SubscriptionManagementService_init();

rc_t
SubscriptionManagementService_evalFilteredSubscription(DeviceLocation_t* sample);

rc_t
SubscriptionManagementService_handleParticipant(SDDS_DCPSParticipant* sample);

rc_t
SubscriptionManagementService_handlePupblication(DDS_DCPSPublication* sample);

rc_t
SubscriptionManagementService_handleSubscription(SDDS_DCPSSubscription* sample);

rc_t
SubscriptionManagementService_registerFilter(SDDS_DCPSManagement* sample);


#endif /* SDDS_INCLUDE_SDDS_SUBSCRIPTIONMANAGEMENTSERVICE_H_ */
