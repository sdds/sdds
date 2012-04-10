
#include <stdbool.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include "linux_sdds_impl.h"

#include <sdds/DataSink.h>
#include <sdds/DataSource.h>


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
			Temperature temperature_data_used;
			Temperature *temperature_data_used_ptr = &temperature_data_used;

			if (DDS_TemperatureDataReader_take_next_sample(g_Temperature_reader, &temperature_data_used_ptr, NULL) != DDS_RETCODE_NO_DATA)
			{
				printf("temperature: %f °C \n", (temperature_data_used.temperature / 100.0) );
			}
		}
		else if (strcmp(s, "read_humidity\n") == 0)
		{
			Humidity humidity_data_used;
			Humidity *humidity_data_used_ptr = &humidity_data_used;

			if (DDS_HumidityDataReader_take_next_sample(g_Humidity_reader, &humidity_data_used_ptr, NULL) != DDS_RETCODE_NO_DATA)
			{
				printf("relative humidity: %f %%\n", (humidity_data_used.relative_humidity / 100.0) );
			}
		}
		else if (strcmp(s, "read_pressure\n") == 0)
		{
			Pressure pressure_data_used;
			Pressure *pressure_data_used_ptr = &pressure_data_used;

			if (DDS_PressureDataReader_take_next_sample(g_Pressure_reader, &pressure_data_used_ptr, NULL) != DDS_RETCODE_NO_DATA)
			{
				double pressure = pressure_data_used.pressure / 10.0;
				double altitude = 44300 * (1 - pow(pressure / 1013.25, 1.0 / 5.255));

				printf("pressure: %g hPa\n", pressure);
				printf("altitude: %g m\n",altitude);
			}
		}
		else if (strcmp(s, "read_wind_speed\n") == 0)
		{
			Wind_speed wind_speed_data_used;
			Wind_speed *wind_speed_data_used_ptr = &wind_speed_data_used;

			if (DDS_Wind_speedDataReader_take_next_sample(g_Wind_speed_reader, &wind_speed_data_used_ptr, NULL) != DDS_RETCODE_NO_DATA)
			{
				printf( "wind speed\n");
				printf("current:     %f km/h\n", (wind_speed_data_used.speed_current / 100.0) );
				printf("last minute: %f km/h\n", (wind_speed_data_used.speed_minute / 100.0) );
				printf("last hour:   %f km/h\n", (wind_speed_data_used.speed_hour / 100.0) );
			}
		}
		else if (strcmp(s, "read_rainfall\n") == 0)
		{
			Rainfall rainfall_data_used;
			Rainfall *rainfall_data_used_ptr = &rainfall_data_used;

			if (DDS_RainfallDataReader_take_next_sample(g_Rainfall_reader, &rainfall_data_used_ptr, NULL) != DDS_RETCODE_NO_DATA)
			{
				printf("rain amount\n");
				printf("current:     %f mm/h\n", (rainfall_data_used.amount_current / 100.0) );
				printf("last minute: %f mm/h\n", (rainfall_data_used.amount_minute / 100.0) );
				printf("last hour:   %f mm/h\n", (rainfall_data_used.amount_hour / 100.0) );
			}
		}
		else if (strcmp(s, "read_wind_direction\n") == 0)
		{
			Wind_direction wind_direction_data_used;
			Wind_direction *wind_direction_data_used_ptr = &wind_direction_data_used;

			if (DDS_Wind_directionDataReader_take_next_sample(g_Wind_direction_reader, &wind_direction_data_used_ptr, NULL) != DDS_RETCODE_NO_DATA)
			{
				printf("wind direction: %f degrees\n", (wind_direction_data_used.degrees_multiplier * 22.5) );
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
