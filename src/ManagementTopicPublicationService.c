/*
 * ManagementTopicPublicationService.c
 *
 *  Created on: Aug 29, 2016
 *      Author: o_dedi
 */

#include "sDDS.h"
#include "ManagementTopicPublicationService.h"
#include "ManagementTopic.h"

#ifdef FEATURE_SDDS_MANAGEMENT_TOPIC_ENABLED

rc_t
ManagementTopicPublicationService_init() {
    return SDDS_RT_OK;
}

rc_t
ManagementTopicPublicationService_publishManagementDirective(SDDS_DCPSManagement* data, Locator_t* addr) {
    assert(data);
    assert(addr);

    DataWriter_t* dw = ((DataWriter_t*) g_DCPSManagement_writer);

    if (SDDS_DCPSManagementDataWriter_writeToParticipant(g_DCPSManagement_writer, data, NULL, addr) == DDS_RETCODE_ERROR) {
        // handle error
        Log_error("Send participant topic failed.\n");
        return SDDS_RT_FAIL;
    }

    return SDDS_RT_OK;
}

#endif
