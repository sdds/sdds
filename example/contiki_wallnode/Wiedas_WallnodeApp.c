#include "SDDS_Application.h"
#include "sdds/sdds_types.h"

#include <os-ssal/NodeConfig.h>

extern SSW_NodeID_t nodeID;

rc_t SDDS_Application_init() {

	rc_t ret;

	// init the drivers

	// init light sensor

	ret = Wiedas_SensorApp_Light_init();

	// imit temperature sensor

	return SDDS_RT_OK;
}

rc_t SDDS_Application_doWork(){

	rc_t ret;
	//durchreichen der process loop

	ret = Wiedas_SensorApp_Light_dowork();

	return SDDS_RT_OK;
}

rc_t SDDS_Application_start(){
	return SDDS_RT_OK;
}

rc_t SDDS_Application_stop(){
	return SDDS_RT_OK;
}
