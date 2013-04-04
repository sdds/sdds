#include "SDDS_Application.h"
#include "sdds/sdds_types.h"

#include <os-ssal/NodeConfig.h>

#include "Wiedas_sensor_apps.h"

extern SSW_NodeID_t nodeID;

rc_t SDDS_Application_init() {

	rc_t ret;

	// init the drivers

	// init light sensor

	//ret = Wiedas_SensorApp_Light_init();

	// init temperature sensor

	//ret = Wiedas_SensorApp_Temperature_init();

	// init movement sensor

	//ret = Wiedas_SensorApp_Movement_init();

	// init humidity sensor

	//ret = Wiedas_SensorApp_Humidity_init();

	// init dimmer lamp
	ret = Wiedas_SensorApp_DimmerLamp_init();

	return SDDS_RT_OK;
}

rc_t SDDS_Application_doWork(){

	rc_t ret;
	//durchreichen der process loop

	/*
	ret = Wiedas_SensorApp_Light_dowork();

	ret = Wiedas_SensorApp_Temperature_dowork();

	ret = Wiedas_SensorApp_Movement_dowork();

	ret = Wiedas_SensorApp_Humidity_dowork();
*/
	ret = Wiedas_SensorApp_DimmerLamp_dowork();

	return SDDS_RT_OK;
}

rc_t SDDS_Application_start(){
	return SDDS_RT_OK;
}

rc_t SDDS_Application_stop(){
	return SDDS_RT_OK;
}
