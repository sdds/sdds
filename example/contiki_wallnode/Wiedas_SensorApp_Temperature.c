#include "Wiedas_sensor_apps.h"

#include "sdds/sdds_types.h"
#include "sdds/Log.h"
#include <dds/DDS_DCPS.h>
#include <os-ssal/NodeConfig.h>
#include "atmega_sdds_impl.h"

#include "TemperatureSensor-ds.h"

#include "ds18x20.h"

extern SSW_NodeID_t nodeID;

rc_t Wiedas_SensorApp_Temperature_init() {

	rc_t ret;

	ret = DS18X20_init();

	return SDDS_RT_OK;
}
rc_t Wiedas_SensorApp_Temperature_start() {
	return SDDS_RT_OK;
}
rc_t Wiedas_SensorApp_Temperature_dowork() {


	int16_t tem;
	rc_t ret;

	ret = DS18X20_read_celsius(&tem);

	if (ret != SDDS_RT_OK) {
		Log_error("Can't read temperatur form ds18s20 %d \n", ret);
		return ret;
	}
	Log_debug("temperature %d \n", tem);


	TemperatureSensor data;
	data.id = nodeID;
	data.temperature = tem;

	DDS_ReturnCode_t ddsret;

	ddsret = DDS_TemperatureSensorDataWriter_write(g_TemperatureSensor_writer, &data, NULL);

	if (ddsret != DDS_RETCODE_OK) {
		return SDDS_RT_FAIL;
	}

	return SDDS_RT_OK;
}
rc_t Wiedas_SensorApp_Temperature_process() {
	return SDDS_RT_OK;
}
