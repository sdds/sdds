/*
 * ManagementTopicDataWriter.h
 *
 *  Created on: Aug 29, 2016
 *      Author: o_dedi
 */

#ifndef SDDS_INCLUDE_SDDS_MANAGEMENTTOPICDATAWRITER_H_
#define SDDS_INCLUDE_SDDS_MANAGEMENTTOPICDATAWRITER_H_

#include "sDDS.h"
#include "DataWriter.h"
#include "Locator.h"

rc_t
ManagementTopicDataWriter_writeToParticipant(DataWriter_t* self,
                                             Data data,
                                             void* handle,
                                             Locator_t* addr
                                             );


#endif /* SDDS_INCLUDE_SDDS_MANAGEMENTTOPICDATAWRITER_H_ */
