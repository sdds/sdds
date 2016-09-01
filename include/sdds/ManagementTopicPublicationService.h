/*
 * ManagementTopicPublicationService.h
 *
 *  Created on: Aug 29, 2016
 *      Author: o_dedi
 */

#ifndef SDDS_INCLUDE_SDDS_MANAGEMENTTOPICPUBLICATIONSERVICE_H_
#define SDDS_INCLUDE_SDDS_MANAGEMENTTOPICPUBLICATIONSERVICE_H_

#include "sDDS.h"
#include "ManagementTopic.h"

rc_t
ManagementTopicPublicationService_init();

rc_t
ManagementTopicPublicationService_publishManagementDirective(SDDS_DCPSManagement* data, Locator_t* addr);

#endif /* SDDS_INCLUDE_SDDS_MANAGEMENTTOPICPUBLICATIONSERVICE_H_ */
