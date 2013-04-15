#include "Wiedas_sensor_apps.h"

#include "sdds/sdds_types.h"
#include "sdds/Log.h"
#include <dds/DDS_DCPS.h>

#include <os-ssal/NodeConfig.h>


#include "atmega_sdds_impl.h"

#include "HumiditySensor-ds.h"

extern SSW_NodeID_t nodeID;

rc_t Wiedas_SensorApp_Humidity_init(){
	return SDDS_RT_OK;
}
rc_t Wiedas_SensorApp_Humidity_start(){
	return SDDS_RT_OK;
}
rc_t Wiedas_SensorApp_Humidity_dowork(){
	return SDDS_RT_OK;
}
rc_t Wiedas_SensorApp_Humidity_process(){
	return SDDS_RT_OK;
}
