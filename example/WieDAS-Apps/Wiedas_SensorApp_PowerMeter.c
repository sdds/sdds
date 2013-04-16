/*
 * Wiedas_SensorApp_PowerMeter.c
 *
 *  Created on: Apr 16, 2013
 *      Author: k_beckma
 */


#include "Wiedas_sensor_apps.h"

#include "sdds/sdds_types.h"
#include "sdds/Log.h"
#include <dds/DDS_DCPS.h>

#include <os-ssal/NodeConfig.h>

#include "atmega_sdds_impl.h"

#include "PowerMeter-ds.h"

extern SSW_NodeID_t nodeID;

#define ONOFF_ON 1
#define ONOFF_OFF 0
#define TOGGLE_TOGGLE 0

rc_t Wiedas_SensorApp_PowerMeter_init() {


	return SDDS_RT_OK;
}
rc_t Wiedas_SensorApp_PowerMeter_start() {

	return SDDS_RT_OK;
}
rc_t Wiedas_SensorApp_PowerMeter_dowork() {

	uint16_t power;
	PowerMeter p;

	power = metering_get_power();
	p.id = nodeID;
	p.watt = power;
	Log_debug("Publish PowerMeter power %d \n", power);

	DDS_ReturnCode_t ret = DDS_PowerMeterDataWriter_write(g_PowerMeter_writer,
			&p, NULL );

	if (ret != DDS_RETCODE_OK) {
		Log_error("Can't publish PowerMeter \n");
		return SDDS_RT_FAIL;
	}


	return SDDS_RT_OK;
}
