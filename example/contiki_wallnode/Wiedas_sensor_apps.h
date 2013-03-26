/*
 * Wiedas_sensor_apps.h
 *
 *  Created on: 22.03.2013
 *      Author: kai
 */

#ifndef WIEDAS_SENSOR_APPS_H_
#define WIEDAS_SENSOR_APPS_H_

#include <sdds/sdds_types.h>

rc_t Wiedas_SensorApp_Temperature_init();
rc_t Wiedas_SensorApp_Temperature_start();
rc_t Wiedas_SensorApp_Temperature_dowork();
rc_t Wiedas_SensorApp_Temperature_process();

rc_t Wiedas_SensorApp_Light_init();
rc_t Wiedas_SensorApp_Light_start();
rc_t Wiedas_SensorApp_Light_dowork();
rc_t Wiedas_SensorApp_Light_process();

rc_t Wiedas_SensorApp_Movement_init();
rc_t Wiedas_SensorApp_Movement_start();
rc_t Wiedas_SensorApp_Movement_dowork();
rc_t Wiedas_SensorApp_Movement_process();


rc_t Wiedas_SensorApp_Humidity_init();
rc_t Wiedas_SensorApp_Humidity_start();
rc_t Wiedas_SensorApp_Humidity_dowork();
rc_t Wiedas_SensorApp_Humidity_process();

rc_t Wiedas_SensorApp_Door_init(uint8_t number);
rc_t Wiedas_SensorApp_Door_start(uint8_t number);
rc_t Wiedas_SensorApp_Door_dowork(uint8_t number);

rc_t Wiedas_SensorApp_Window_init(uint8_t number);
rc_t Wiedas_SensorApp_Window_start(uint8_t number);
rc_t Wiedas_SensorApp_Window_dowork(uint8_t number);

rc_t Wiedas_SensorApp_Switch_init(uint8_t number);
rc_t Wiedas_SensorApp_Switch_start(uint8_t number);
rc_t Wiedas_SensorApp_Switch_dowork(uint8_t number);

rc_t Wiedas_SensorApp_Battery_init();
rc_t Wiedas_SensorApp_Battery_start();
rc_t Wiedas_SensorApp_Battery_dowork();

rc_t Wiedas_SensorApp_SimpleLight_init();
rc_t Wiedas_SensorApp_SimplteLight_start();
rc_t Wiedas_SensorApp_SimplteLight_dowork();

rc_t Wiedas_SensorApp_OnOffOutput_init();
rc_t Wiedas_SensorApp_OnOffOutput_start();
rc_t Wiedas_SensorApp_OnOffOutput_dowork();

rc_t Wiedas_SensorApp_PowerMeter_init();
rc_t Wiedas_SensorApp_PowerMeter_start();
rc_t Wiedas_SensorApp_PowerMeter_dowork();

rc_t Wiedas_SensorApp_DimmerLamp_init();
rc_t Wiedas_SensorApp_DimmerLamp_start();
rc_t Wiedas_SensorApp_DimmerLamp_dowork();

#endif /* WIEDAS_SENSOR_APPS_H_ */
