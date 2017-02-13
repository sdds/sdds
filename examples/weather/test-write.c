
#include <stdbool.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include "linux_sdds_impl.h"

#include <sdds/DataSink.h>
#include <sdds/DataSource.h>
#include <dds/DDS_DCPS.h>


int main(void)
{
	sDDS_init();

	for (;;)
	{
		printf("Please enter a command\n");


		size_t siz = 100;
		char* s = malloc(siz +1);
		ssize_t byte_read = getline(&s, &siz, stdin);

		if (byte_read < 0) {
			printf("Error: cant read from stdin\n");
			return -1;
		}

		if (strcmp(s, "read_temperature\n") == 0)
		{
			TemperatureSensor temperature_data_used;
			TemperatureSensor *temperature_data_used_ptr = &temperature_data_used;

			if (DDS_TemperatureSensorDataReader_take_next_sample(g_TemperatureSensor_reader, &temperature_data_used_ptr, NULL) != DDS_RETCODE_NO_DATA)
			{
				printf("temperature: %f °C \n", (temperature_data_used.temperature / 100.0) );
			}
		}
		else if (strcmp(s, "read_humidity\n") == 0)
		{
			TemperatureAndHumiditySensor humidity_data_used;
			TemperatureAndHumiditySensor *humidity_data_used_ptr = &humidity_data_used;

			if (DDS_TemperatureAndHumiditySensorDataReader_take_next_sample(g_TemperatureAndHumiditySensor_reader, &humidity_data_used_ptr, NULL) != DDS_RETCODE_NO_DATA)
			{
				printf("relative humidity: %f temperature %f %%\n", (humidity_data_used.relative_humidity / 100.0), (humidity_data_used.temperature / 100.0));
			}
		}
		else if (strcmp(s, "read_pressure\n") == 0)
		{
			AirPressureSensor pressure_data_used;
			AirPressureSensor *pressure_data_used_ptr = &pressure_data_used;

			if (DDS_AirPressureSensorDataReader_take_next_sample(g_AirPressureSensor_reader, &pressure_data_used_ptr, NULL) != DDS_RETCODE_NO_DATA)
			{
				double pressure = pressure_data_used.pressure / 10.0;
				double altitude = 44300 * (1 - pow(pressure / 1013.25, 1.0 / 5.255));

				printf("pressure: %g hPa\n", pressure);
				printf("altitude: %g m\n",altitude);
			}
		}
		else if (strcmp(s, "read_wind_speed\n") == 0)
		{
			WindSpeedSensor wind_speed_data_used;
			WindSpeedSensor *wind_speed_data_used_ptr = &wind_speed_data_used;

			if (DDS_WindSpeedSensorDataReader_take_next_sample(g_WindSpeedSensor_reader, &wind_speed_data_used_ptr, NULL) != DDS_RETCODE_NO_DATA)
			{
				printf( "wind speed\n");
				printf("current:     %f km/h\n", (wind_speed_data_used.speed_current / 100.0) );
				printf("last minute: %f km/h\n", (wind_speed_data_used.speed_minute / 100.0) );
				printf("last hour:   %f km/h\n", (wind_speed_data_used.speed_hour / 100.0) );
			}
		}
		else if (strcmp(s, "read_rainfall\n") == 0)
		{
			RainfallSensor rainfall_data_used;
			RainfallSensor *rainfall_data_used_ptr = &rainfall_data_used;

			if (DDS_RainfallSensorDataReader_take_next_sample(g_RainfallSensor_reader, &rainfall_data_used_ptr, NULL) != DDS_RETCODE_NO_DATA)
			{
				printf("rain amount\n");
				printf("current:     %f mm/h\n", (rainfall_data_used.amount_current / 100.0) );
				printf("last minute: %f mm/h\n", (rainfall_data_used.amount_minute / 100.0) );
				printf("last hour:   %f mm/h\n", (rainfall_data_used.amount_hour / 100.0) );
			}
		}
		else if (strcmp(s, "read_wind_direction\n") == 0)
		{
			WindDirectionSensor wind_direction_data_used;
			WindDirectionSensor *wind_direction_data_used_ptr = &wind_direction_data_used;

			if (DDS_WindDirectionSensorDataReader_take_next_sample(g_WindDirectionSensor_reader, &wind_direction_data_used_ptr, NULL) != DDS_RETCODE_NO_DATA)
			{
				printf("wind direction: %f degrees\n", (wind_direction_data_used.degrees_multiplier * 22.5) );
			}
		}
		else if (strcmp(s, "read_light\n") == 0) {
			LightSensor ldata;
			LightSensor* ldata_ptr = &ldata;

			if (DDS_LightSensorDataReader_take_next_sample(g_LightSensor_reader, &ldata_ptr, NULL) != DDS_RETCODE_NO_DATA)
			{
				printf("light : %d lux\n", ldata.lux);
			}
		}
		else if (strcmp(s, "quit\n") == 0)
		{
			break;
		}
		else
		{
			printf("unknown command: %s \n", s);
		}

		free(s);

	}

	return 0;
}
