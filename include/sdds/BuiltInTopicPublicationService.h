/*
 * BuiltInTopicPublicationService.h
 *
 *  Created on: Aug 1, 2016
 *      Author: o_dedi
 */

#ifndef SDDS_INCLUDE_SDDS_BUILTINTOPICPUBLICATIONSERVICE_H_
#define SDDS_INCLUDE_SDDS_BUILTINTOPICPUBLICATIONSERVICE_H_

#include "sDDS.h"

rc_t
BuiltInTopicPublicationService_init();

rc_t
BuiltInTopicPublicationService_publishDCPSParticipant();

rc_t
BuiltInTopicPublicationService_publishDCPSTopic(topicid_t id);

rc_t
BuiltInTopicPublicationService_publishDCPSPublication();

rc_t
BuiltInTopicPublicationService_publishDCPSSubscription(topicid_t id);

rc_t
BuiltInTopicPublicationService_publishDCPSLocation(DeviceLocation_t* dev);


#endif /* SDDS_INCLUDE_SDDS_BUILTINTOPICPUBLICATIONSERVICE_H_ */
