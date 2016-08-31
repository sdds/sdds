/*
 * ManagementTopicPublicationService.c
 *
 *  Created on: Aug 29, 2016
 *      Author: o_dedi
 */

#include "sDDS.h"
#include "ManagementTopicPublicationService.h"
#include "ManagementTopic.h"

rc_t
ManagementTopicPublicationService_init() {
    return SDDS_RT_OK;
}

rc_t
ManagementTopicPublicationService_publishManagementDirective(Data data, Locator_t* addr) {
    DataWriter_t* dw = ((DataWriter_t*) g_DCPSManagement_writer);

    if (SDDS_DCPSManagementDataWriter_writeToParticipant(g_DCPSParticipant_writer, data, NULL, addr) == DDS_RETCODE_ERROR) {
        // handle error
        Log_error("Send participant topic failed.\n");
        return SDDS_RT_FAIL;
    }
    return SDDS_RT_OK;
}
