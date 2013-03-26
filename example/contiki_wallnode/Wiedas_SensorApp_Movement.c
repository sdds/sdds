#include "Wiedas_sensor_apps.h"

#include "sdds/sdds_types.h"
#include "sdds/Log.h"
#include <dds/DDS_DCPS.h>
#include <os-ssal/NodeConfig.h>
#include "atmega_sdds_impl.h"

#include "MovementSensor-ds.h"

#include "AMN31112.h"

#define MOVEMENT_isMoving 0
#define MOVEMENT_notMoving 1

extern SSW_NodeID_t nodeID;

rc_t Wiedas_SensorApp_Movement_init() {

	rc_t ret;

	ret = AMN31112_init();

	if (ret != SDDS_RT_OK) {
		Log_error("Can't init PIR movement sensor\n");
		return ret;
	}
	return SDDS_RT_OK;
}
rc_t Wiedas_SensorApp_Movement_start() {
	return SDDS_RT_OK;
}
rc_t Wiedas_SensorApp_Movement_dowork() {


	// read sensor
	bool_t movement;

	AMN31112_readMovement(&movement);

	Log_debug("checked movement: %d \n", movement);


	// publish regular data
	MovementSensor data;

	data.id = nodeID;
	if (movement == true) {
		data.state = MOVEMENT_isMoving;
	} else {
		data.state = MOVEMENT_notMoving;
	}

	DDS_ReturnCode_t ddsret;

	ddsret = DDS_MovementSensorDataWriter_write(g_MovementSensor_writer, &data, NULL);

	if (ddsret != DDS_RETCODE_OK) {
		return SDDS_RT_FAIL;
	}

	return SDDS_RT_OK;
}
rc_t Wiedas_SensorApp_Movement_process() {
	return SDDS_RT_OK;
}



