#include "Wiedas_sensor_apps.h"

#include "sdds/sdds_types.h"
#include "sdds/Log.h"
#include <dds/DDS_DCPS.h>

#include <os-ssal/NodeConfig.h>


#include "atmega_sdds_impl.h"

#include "DimmableLight-ds.h"

#include "LED.h"
#include "ATMEGA_LED.h"

#include <avr/io.h>

extern SSW_NodeID_t nodeID;

static LED lampled;
static LED fan4led;

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

void sdds_DR_handler_LightRegulationFunctionality(DDS_DataReader reader);
void sdds_DR_handler_OnOffFunctionality(DDS_DataReader reader);
void sdds_DR_handler_ToggleFunctionality(DDS_DataReader reader);


rc_t Wiedas_SensorApp_DimmerLamp_init(){

	rc_t ret;

	// init led with dimming
	static struct LED_t lamp_stc = {
			.bank = LED_CONF_BANK_B,
			.pin = LED_CONF_PIN_5,
			.sourceing = true,
			.resolution = LED_CONF_DIM_RESOLUTION_10BIT,
			.mode = (LED_CONF_DIM_MODE_FAST_PWM | LED_CONF_DIM_ACTIVATE),
			.dimValue = 0

	};
	lampled = &lamp_stc;
	ret = LED_init(lampled);


	// switch of the lamp
	LED_switchOff(lampled);

	// init led fan (as an led ...)
	static struct LED_t fan_stc = {
			.bank = LED_CONF_BANK_B,
			.pin = LED_CONF_PIN_4,
			.sourceing = true,
			.mode = 0
	};
	fan4led = &fan_stc;
	ret = LED_init(fan4led);

	LED_switchOff(fan4led);


	// register callbacks for topics

	DDS_ReturnCode_t ddsret;


	struct DDS_DataReaderListener listStruct = {
			.on_data_available = &sdds_DR_handler_LightRegulationFunctionality
	};

	ddsret = DDS_DataReader_set_listener((DDS_DataReader) g_LightRegulationFunctionality_reader,
			 &listStruct, NULL);

	listStruct.on_data_available = &sdds_DR_handler_OnOffFunctionality;
	ddsret = DDS_DataReader_set_listener((DDS_DataReader) g_OnOffFunctionality_reader,
				 &listStruct, NULL);

	listStruct.on_data_available = &sdds_DR_handler_ToggleFunctionality;
	ddsret = DDS_DataReader_set_listener((DDS_DataReader) g_ToggleFunctionality_reader,
				 &listStruct, NULL);

	return SDDS_RT_OK;
}
rc_t Wiedas_SensorApp_DimmerLamp_start(){
	return SDDS_RT_OK;
}
rc_t Wiedas_SensorApp_DimmerLamp_dowork(){

	// get status of lamp an dimm value

	bool_t state;

	LED_getState(lampled, &state);

	uint8_t dimValue;

	LED_getDimValue(lampled, &dimValue);

	DimmableLight data;

	data.id = nodeID;

	data.lightIntensityState = dimValue;

	data.onOffState = state;

	DDS_ReturnCode_t ddsret;

	ddsret = DDS_DimmableLightDataWriter_write(g_DimmableLight_writer, &data, NULL);

	if (ddsret != DDS_RETCODE_OK) {
		return SDDS_RT_FAIL;
	}

	return SDDS_RT_OK;
}

void sdds_DR_handler_LightRegulationFunctionality(DDS_DataReader reader) {

	LightRegulationFunctionality data;
	LightRegulationFunctionality* data_ptr = &data;

	DDS_ReturnCode_t ddsret;

	ddsret = DDS_LightRegulationFunctionalityDataReader_take_next_sample(
			reader,	&data_ptr, NULL);


	if (ddsret == DDS_RETCODE_NO_DATA) {
		Log_info("No data available, but called by callback?\n ");
		return;
	}
	if (ddsret != DDS_RETCODE_OK) {
		Log_error("Error receiving data from LightRegulationFunctionality topic\n");
		return	;
	}

	// check if msg is for this node

	if (data.id != nodeID) {
		return ;
	}
	Log_debug("Got LightRegulationFunctionality command value: %d \n", data.setValue);

	uint8_t value = data.setValue;

	LED_dim(lampled, value);

	if (value > 0) {
		LED_switchOn(fan4led);
	} else {
		LED_switchOff(fan4led);
	}

	return;
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
		LED_switchOn(lampled);
		// switch on the fan as well
		LED_switchOn(fan4led);
	} else if (data.command == ONOFF_OFF) {
		LED_switchOff(lampled);
		LED_switchOff(fan4led);
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
		LED_toggle(lampled);
	}
	bool_t ledstate;
	LED_getState(lampled, &ledstate);
	if (ledstate == true) {
		// lamp is on, switch on the fan
		LED_switchOn(fan4led);
	} else {
		// switch off the fan
		LED_switchOff(fan4led);
	}

	return;
}

