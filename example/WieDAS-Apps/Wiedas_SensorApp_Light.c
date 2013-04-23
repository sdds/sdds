#include "Wiedas_sensor_apps.h"

#include "sdds/sdds_types.h"
#include "sdds/Log.h"
#include <dds/DDS_DCPS.h>

#include "TSL2561.h"
#include "twi.h"

#include <os-ssal/NodeConfig.h>


#include "atmega_sdds_impl.h"

#include "LightSensor-ds.h"

extern SSW_NodeID_t nodeID;


rc_t Wiedas_SensorApp_Light_init() {

	// init light sensor driver
	rc_t ret;
//	twi_activateInternalPullUp();
	ret = TSL2561_init();
	if (ret != SDDS_RT_OK) {
		Log_error("can't init tsl2561 ret %d\n", ret);
		return ret;
	}
	uint8_t id = 0;
	ret = TSL2561_readID(&id);
	Log_debug("TSL2561: id 0x%x, ret %d\n", id, ret);

	// init dds?

	return SDDS_RT_OK;
}
rc_t Wiedas_SensorApp_Light_start(){

	// kein eigener thread hier
	return SDDS_RT_OK;
}
rc_t Wiedas_SensorApp_Light_dowork(){

	// get light data

	rc_t ret;

		uint16_t ch0, ch1;
		ret = TSL2561_getChannels(&ch0, &ch1);

		if (ret != SDDS_RT_OK) {
			Log_error("cant read channels of tsl2561 ret %d\n", ret);
			return ret;
		}
		Log_debug("tsl2561: channel 0 %u channel 1 %u \n", ch0, ch1);

		uint32_t lux = 0;
		ret = TSL2561_calculateLux(ch0, ch1, &lux);
		// cant go wrong

		Log_debug("tsl2561: lux %u 0x%x \n", lux, lux);


	// publish light data
		LightSensor data;

		data.id = nodeID;
		data.lux = (uint16_t) lux;

		DDS_ReturnCode_t ddsret;

		ddsret = DDS_LightSensorDataWriter_write(g_LightSensor_writer, &data, NULL);

		if (ddsret != DDS_RETCODE_OK) {
			return SDDS_RT_FAIL;
		}

	return SDDS_RT_OK;
}
rc_t Wiedas_SensorApp_Light_process(){
	return SDDS_RT_OK;
}
