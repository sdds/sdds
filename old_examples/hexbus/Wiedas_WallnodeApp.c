#include "SDDS_Application.h"
#include "sdds/sdds_types.h"

#include <os-ssal/NodeConfig.h>

#include "Wiedas_sensor_apps.h"

extern SSW_NodeID_t nodeID;

rc_t SDDS_Application_init() {

	rc_t ret;

	// init the drivers

	// init onoffoutput
	ret = Wiedas_SensorApp_OnOffOutput_init();

	// init simplelamp
	//ret = Wiedas_SensorApp_SimpleLamp_init();

	// init powermeter
	ret = Wiedas_SensorApp_PowerMeter_init();

	// init light sensor

	//ret = Wiedas_SensorApp_Light_init();

	// init temperature sensor

	//ret = Wiedas_SensorApp_Temperature_init();

	// init movement sensor

	//ret = Wiedas_SensorApp_Movement_init();

	// init humidity sensor

	//ret = Wiedas_SensorApp_Humidity_init();

	// init dimmer lamp
	//ret = Wiedas_SensorApp_DimmerLamp_init();

	// init door sensors
	// door on first port
	//ret = Wiedas_SensorApp_Door_init(0);

	// door on second port
	//ret = Wiedas_SensorApp_Door_init(1);

	// door on third port
	//ret = Wiedas_SensorApp_Door_init(2);

	// switch on first port
	//ret = Wiedas_SensorApp_Switch_init(0);

	// switch on second port
	//ret = Wiedas_SensorApp_Switch_init(1);

	// switch on third port
	//ret = Wiedas_SensorApp_Switch_init(2);

	// window on first port
	//ret = Wiedas_SensorApp_Window_init(0);

	// window on second port
	//ret = Wiedas_SensorApp_Window_init(1);

	// window on third port
	//ret = Wiedas_SensorApp_Window_init(2);

	return SDDS_RT_OK;
}

rc_t SDDS_Application_doWork(){

	rc_t ret;
	//durchreichen der process loop

	ret = Wiedas_SensorApp_OnOffOutput_dowork();

	//ret = Wiedas_SensorApp_SimpleLamp_dowork();

	ret = Wiedas_SensorApp_PowerMeter_dowork();

	//ret = Wiedas_SensorApp_Light_dowork();

	//ret = Wiedas_SensorApp_Temperature_dowork();

	//ret = Wiedas_SensorApp_Movement_dowork();

	//ret = Wiedas_SensorApp_Humidity_dowork();

	//ret = Wiedas_SensorApp_DimmerLamp_dowork();

	//ret = Wiedas_SensorApp_Door_dowork(0);
	//ret = Wiedas_SensorApp_Door_dowork(1);
	//ret = Wiedas_SensorApp_Door_dowork(2);

	//ret = Wiedas_SensorApp_Switch_dowork(0);
	//ret = Wiedas_SensorApp_Switch_dowork(1);
	//ret = Wiedas_SensorApp_Switch_dowork(2);

	//ret = Wiedas_SensorApp_Window_dowork(0);
	//ret = Wiedas_SensorApp_Window_dowork(1);
	//ret = Wiedas_SensorApp_Window_dowork(2);

	return SDDS_RT_OK;
}

rc_t SDDS_Application_start(){

	rc_t ret;

	//ret = Wiedas_SensorApp_Movement_start();

	return SDDS_RT_OK;
}

rc_t SDDS_Application_stop(){
	return SDDS_RT_OK;
}
