/*
 * Wiedas_SensorApp_SimpleLamp.c
 *
 *  Created on: Apr 16, 2013
 *      Author: k_beckma
 */


#include "Wiedas_sensor_apps.h"

#include "sdds/sdds_types.h"
#include "sdds/Log.h"
#include <dds/DDS_DCPS.h>

#include <os-ssal/NodeConfig.h>

#include <relay.h>

#include "atmega_sdds_impl.h"
#include "OnOffFunctionality-ds.h"
#include "ToggleFunctionality-ds.h"
#include "SimpleLamp-ds.h"

extern SSW_NodeID_t nodeID;

#define ONOFF_ON 1
#define ONOFF_OFF 0
#define TOGGLE_TOGGLE 0

/*
struct DDS_DataReaderListener
{
void *listener_data;
DDS_DataReaderListener_RequestedDeadlineMissedListener on_requested_deadline_missed;
DDS_DataReaderListener_RequestedIncompatibleQosListener on_requested_incompatible_qos;
DDS_DataReaderListener_SampleRejectedListener on_sample_rejected;
DDS_DataReaderListener_LivelinessChangedListener on_liveliness_changed;
DDS_DataReaderListener_DataAvailableListener on_data_available;
DDS_DataReaderListener_SubscriptionMatchListener on_subscription_matched;
DDS_DataReaderListener_SampleLostListener on_sample_lost;
};
typedef void (*DDS_DataReaderListener_DataAvailableListener)(DDS_DataReader reader);
*/

void sdds_DR_handler_OnOffFunctionality(DDS_DataReader reader);
void sdds_DR_handler_ToggleFunctionality(DDS_DataReader reader);

rc_t Wiedas_SensorApp_SimpleLamp_init() {

	// default : off
	relay_on();

	// register callbacks for topics

	DDS_ReturnCode_t ddsret;

	struct DDS_DataReaderListener listStruct = { .on_data_available =
			&sdds_DR_handler_OnOffFunctionality };

	ddsret = DDS_DataReader_set_listener(
			(DDS_DataReader) g_OnOffFunctionality_reader, &listStruct, NULL );

	listStruct.on_data_available = &sdds_DR_handler_ToggleFunctionality;
	ddsret = DDS_DataReader_set_listener(
			(DDS_DataReader) g_ToggleFunctionality_reader, &listStruct, NULL );

	return SDDS_RT_OK;
}
rc_t Wiedas_SensorApp_SimpleLamp_start() {

	return SDDS_RT_OK;
}
rc_t Wiedas_SensorApp_SimpleLamp_dowork() {

	static waitCount = 0;

	if (waitCount < WIEDAS_SENSORAPP_SIMPLELAMP_INTERVALL) {
		waitCount++;
		return SDDS_RT_OK;
	} else {
		waitCount = 0;
	}


	bool_t state = relay_get_state();

	SimpleLamp sw;
	sw.id = nodeID;

	if (state == 0) {
		sw.state = ONOFF_ON;
	} else {
		sw.state = ONOFF_OFF;
	}


	Log_debug("Publish SimpleLamp state: %d\n", sw.state);

	DDS_ReturnCode_t ret = DDS_SimpleLampDataWriter_write(g_SimpleLamp_writer,
			&sw, NULL );
	if (ret != DDS_RETCODE_OK) {
		Log_error("Can't publish SimpleLamp \n");
		return SDDS_RT_FAIL;
	}


	return SDDS_RT_OK;
}

void sdds_DR_handler_OnOffFunctionality(DDS_DataReader reader) {

	OnOffFunctionality data;
	OnOffFunctionality* data_ptr = &data;

	DDS_ReturnCode_t ddsret;

	ddsret = DDS_OnOffFunctionalityDataReader_take_next_sample(reader, &data_ptr, NULL);
	if (ddsret == DDS_RETCODE_NO_DATA) {
		Log_info("No data available, but called by callback?\n ");
		return;
	}
	if (ddsret != DDS_RETCODE_OK) {
		Log_error(
				"Error receiving data from OnOffFunctionality topic\n");
		return;
	}

	// check if this data sample is for this instance
	if (data.id != nodeID) {
		return;
	}

	Log_debug("Got OnOffFunctionality command: %s \n", (data.command ? "ON" : "OFF"));

	if (data.command == ONOFF_ON) {
		relay_off();
	} else if (data.command == ONOFF_OFF) {
		relay_on();
	}

	return;

}
void sdds_DR_handler_ToggleFunctionality(DDS_DataReader reader) {

	ToggleFunctionality data;
	ToggleFunctionality* data_ptr = &data;

	DDS_ReturnCode_t ddsret;

	ddsret = DDS_ToggleFunctionalityDataReader_take_next_sample(reader, &data_ptr, NULL);

	if (ddsret == DDS_RETCODE_NO_DATA) {
	//	Log_info("No data available, but called by callback?\n ");
		return;
	}
	if (ddsret != DDS_RETCODE_OK) {
		Log_error( "Error receiving data from ToggleFunctionality topic\n");
		return;
	}

	Log_debug("Got ToggleFunctionality command \n");

	// chekc if this data sample is for this instance
	if (data.id != nodeID) {
		return;
	}

	if (data.command == TOGGLE_TOGGLE) {
		relay_toggle();
	}

	return;
}

