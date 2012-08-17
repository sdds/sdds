#include "atmega_sdds_impl.h"

//#include "weather-drivers.h"

#include "anemometer.h"
#include "bmp085.h"
#include "raingauge.h"
#include "sht15.h"
#include "wind_vane.h"

#include "twi.h"

#include <sdds/DataSink.h>
#include <sdds/DataSource.h>

#include <contiki.h>
#include <contiki-net.h>

PROCESS(write_process, "sDDS");
PROCESS(anemometer_process, "anemometer");
PROCESS(raingauge_process, "raingauge");

AUTOSTART_PROCESSES(&write_process);

static struct etimer g_wait_timer;
static struct etimer g_anemometer_timer;
static void (*g_anemometer_callback)(void);
static struct etimer g_raingauge_timer;
static void (*g_raingauge_callback)(void);

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

	sDDS_init();

	for (;;)
	{
		do
		{
			Rainfall rainfall_data_used;

			if (raingauge_read_current(&rainfall_data_used.amount_current) != 0)
				break;

			if (raingauge_read_minute(&rainfall_data_used.amount_minute) != 0)
				break;

			if (raingauge_read_hour(&rainfall_data_used.amount_hour) != 0)
				break;

			if (DDS_RainfallDataWriter_write(g_Rainfall_writer, &rainfall_data_used, NULL) != DDS_RETCODE_OK)
			{
				// handle error
			}
		} while (0);

		do
		{
			Wind_speed wind_speed_data_used;

			if (anemometer_read_current(&wind_speed_data_used.speed_current) != 0)
				break;

			if (anemometer_read_minute(&wind_speed_data_used.speed_minute) != 0)
				break;

			if (anemometer_read_hour(&wind_speed_data_used.speed_hour) != 0)
				break;

			if (DDS_Wind_speedDataWriter_write(g_Wind_speed_writer, &wind_speed_data_used, NULL) != DDS_RETCODE_OK)
			{
				// handle error
			}
		} while (0);

		do
		{
			Wind_direction wind_direction_data_used;

			if (wind_vane_read(&wind_direction_data_used.degrees_multiplier) != 0)
				break;

			
			if (DDS_Wind_directionDataWriter_write(g_Wind_direction_writer, &wind_direction_data_used, NULL) != DDS_RETCODE_OK)
			{
				// handle error
			}
		} while (0);

		do
		{
			Temperature temperature_data_used;

			if (bmp085_read_temperature(&temperature_data_used.temperature) != 0)
			{
				// fallback to sht15 sensor
				if (sht15_read_temperature(&temperature_data_used.temperature) != 0)
					break;
			}

			if (DDS_TemperatureDataWriter_write(g_Temperature_writer, &temperature_data_used, NULL) != DDS_RETCODE_OK)
			{
				// handle error
			}
		} while(0);

		do
		{
			Humidity humidity_data_used;

			if (sht15_read_relative_humidity(&humidity_data_used.relative_humidity) != 0)
				break;

			if (DDS_HumidityDataWriter_write(g_Humidity_writer, &humidity_data_used, NULL) != DDS_RETCODE_OK)
			{
				// handle error
			}
		} while(0);

		do
		{
			Pressure pressure_data_used;

			if (bmp085_read_pressure(&pressure_data_used.pressure) != 0)
				break;

			if (DDS_PressureDataWriter_write(g_Pressure_writer, &pressure_data_used, NULL) != DDS_RETCODE_OK)
			{
				// handle error
			}
		} while(0);

		etimer_set(&g_wait_timer, CLOCK_SECOND);
		PROCESS_YIELD_UNTIL(etimer_expired(&g_wait_timer));
	}

	PROCESS_END();
}
