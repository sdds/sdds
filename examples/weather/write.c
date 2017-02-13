#include "atmega_sdds_impl.h"

//#include "weather-drivers.h"

#include "anemometer.h"
#include "bmp085.h"
#include "raingauge.h"
#include "sht15.h"
#include "wind_vane.h"
#include "TSL2561.h"

#include "twi.h"

#include <sdds/DataSink.h>
#include <sdds/DataSource.h>
#include <os-ssal/NodeConfig.h>
#include <sdds/Log.h>

#include <contiki.h>
#include <contiki-net.h>
#include <avr/eeprom.h>

// workaround to prevent contiki to overwrite the mac adress in eeprom
char atmega128rfa1_macadress[8]       EEMEM;

PROCESS(write_process, "sDDS");
PROCESS(anemometer_process, "anemometer");
PROCESS(raingauge_process, "raingauge");

AUTOSTART_PROCESSES(&write_process);

static struct etimer g_wait_timer;
static struct etimer g_anemometer_timer;
static void (*g_anemometer_callback)(void);
static struct etimer g_raingauge_timer;
static void (*g_raingauge_callback)(void);

static SSW_NodeID_t nodeID = 0;

void anemometer_register_callback(void(*callback)(void))
{
	g_anemometer_callback = callback;

	process_start(&anemometer_process, NULL);
}

void raingauge_register_callback(void(*callback)(void))
{
	g_raingauge_callback = callback;

	process_start(&raingauge_process, NULL);
}

PROCESS_THREAD(anemometer_process, ev, data)
{
	PROCESS_BEGIN();

	// make gcc happy
	(void)ev;
	(void)data;

	for (;;)
	{
		etimer_set(&g_anemometer_timer, CLOCK_SECOND);
		PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&g_anemometer_timer));
		g_anemometer_callback();
	}

	PROCESS_END();
}

PROCESS_THREAD(raingauge_process, ev, data)
{
	PROCESS_BEGIN();

	// make gcc happy
	(void)ev;
	(void)data;

	for (;;)
	{
		etimer_set(&g_raingauge_timer, CLOCK_SECOND);
		PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&g_raingauge_timer));
		g_raingauge_callback();
	}

	PROCESS_END();
}

PROCESS_THREAD(write_process, ev, data)
{
	PROCESS_BEGIN();

	// make gcc happy
	(void)ev;
	(void)data;

	anemometer_init();
	raingauge_init();
	wind_vane_init();
	sht15_init();
	bmp085_init();
	TSL2561_init();

	sDDS_init();
	nodeID = NodeConfig_getNodeID();

	for (;;)
	{
		do
		{
			RainfallSensor rainfall_data_used;
			rainfall_data_used.id = nodeID;

			if (raingauge_read_current(&rainfall_data_used.amount_current) != 0)
				break;

			if (raingauge_read_minute(&rainfall_data_used.amount_minute) != 0)
				break;

			if (raingauge_read_hour(&rainfall_data_used.amount_hour) != 0)
				break;

			if (DDS_RainfallSensorDataWriter_write(g_RainfallSensor_writer, &rainfall_data_used, NULL) != DDS_RETCODE_OK)
			{
				// handle error
			}
		} while (0);

		do
		{
			WindSpeedSensor wind_speed_data_used;
			wind_speed_data_used.id = nodeID;

			if (anemometer_read_current(&wind_speed_data_used.speed_current) != 0)
				break;

			if (anemometer_read_minute(&wind_speed_data_used.speed_minute) != 0)
				break;

			if (anemometer_read_hour(&wind_speed_data_used.speed_hour) != 0)
				break;

			if (DDS_WindSpeedSensorDataWriter_write(g_WindSpeedSensor_writer, &wind_speed_data_used, NULL) != DDS_RETCODE_OK)
			{
				// handle error
			}
		} while (0);

		do
		{
			WindDirectionSensor wind_direction_data_used;
			wind_direction_data_used.id = nodeID;

			if (wind_vane_read(&wind_direction_data_used.degrees_multiplier) != 0)
				break;

			
			if (DDS_WindDirectionSensorDataWriter_write(g_WindDirectionSensor_writer, &wind_direction_data_used, NULL) != DDS_RETCODE_OK)
			{
				// handle error
			}
		} while (0);

		do
		{
			TemperatureSensor temperature_data_used;
			temperature_data_used.id = nodeID;

			if (bmp085_read_temperature(&temperature_data_used.temperature) != 0)
			{
				// fallback to sht15 sensor
				if (sht15_read_temperature(&temperature_data_used.temperature) != 0)
					break;
			}

			if (DDS_TemperatureSensorDataWriter_write(g_TemperatureSensor_writer, &temperature_data_used, NULL) != DDS_RETCODE_OK)
			{
				// handle error
			}
		} while(0);

		do
		{
			TemperatureAndHumiditySensor humidity_data_used;
			humidity_data_used.id = nodeID;

			if (sht15_read_relative_humidity(&humidity_data_used.relative_humidity) != 0)
				break;
			if (sht15_read_temperature(&humidity_data_used.temperature) != 0)
				break;

			if (DDS_TemperatureAndHumiditySensorDataWriter_write(g_TemperatureAndHumiditySensor_writer, &humidity_data_used, NULL) != DDS_RETCODE_OK)
			{
				// handle error
			}
		} while(0);

		do
		{
			AirPressureSensor pressure_data_used;
			pressure_data_used.id = nodeID;

			if (bmp085_read_pressure(&pressure_data_used.pressure) != 0)
				break;

			if (DDS_AirPressureSensorDataWriter_write(g_AirPressureSensor_writer, &pressure_data_used, NULL) != DDS_RETCODE_OK)
			{
				// handle error
			}
		} while(0);

		do
		{
			LightSensor ldata;
			ldata.id = nodeID;

			uint16_t ch0, ch1;
			rc_t ret;
			ret = TSL2561_getChannels(&ch0, &ch1);

			if (ret != SDDS_RT_OK) {
				Log_error("cant read channels of tsl2561 ret %d\n", ret);
				break;
			}
			uint32_t lux = 0;
			ret = TSL2561_calculateLux(ch0, ch1, &lux);

			ldata.lux = (uint16_t) lux;

			if (DDS_LightSensorDataWriter_write(g_LightSensor_writer, &ldata, NULL) != DDS_RETCODE_OK)
			{

			}

		} while (0);

		etimer_set(&g_wait_timer, CLOCK_SECOND);
		PROCESS_YIELD_UNTIL(etimer_expired(&g_wait_timer));
	}

	PROCESS_END();
}
