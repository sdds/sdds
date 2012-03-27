extern "C"
{
	#include <stdbool.h>

	#include "linux_sdds_impl.h"

	#include <sdds/data_sink.h>
	#include <sdds/data_source.h>
}

#include <algorithm>
#include <cmath>
#include <cstring>
#include <iostream>
#include <sstream>
#include <string>

using std::cerr;
using std::cin;
using std::cout;
using std::endl;

using std::string;

using std::getline;

int main(void)
{
	init_sdds(3);

	for (;;)
	{
		cout << "Please enter a command\n";

		string s;
		getline(cin, s);

		if (s.find("read_temperature") == 0)
		{
			temperature_data_t temperature_data_used;
			temperature_data_t *temperature_data_used_ptr = &temperature_data_used;

			if (temperature_data_reader_take_next_sample(g_temperature_reader, &temperature_data_used_ptr, NULL) != DDS_RETCODE_NO_DATA)
			{
				cout << "temperature: " << (temperature_data_used.temperature / 100.0) << " °C" << endl;
			}
		}
		else if (s.find("read_humidity") == 0)
		{
			humidity_data_t humidity_data_used;
			humidity_data_t *humidity_data_used_ptr = &humidity_data_used;

			if (humidity_data_reader_take_next_sample(g_humidity_reader, &humidity_data_used_ptr, NULL) != DDS_RETCODE_NO_DATA)
			{
				cout << "relative humidity: " << (humidity_data_used.relative_humidity / 100.0) << " %" << endl;
			}
		}
		else if (s.find("read_pressure") == 0)
		{
			pressure_data_t pressure_data_used;
			pressure_data_t *pressure_data_used_ptr = &pressure_data_used;

			if (pressure_data_reader_take_next_sample(g_pressure_reader, &pressure_data_used_ptr, NULL) != DDS_RETCODE_NO_DATA)
			{
				double pressure = pressure_data_used.pressure / 10.0;
				double altitude = 44300 * (1 - std::pow(pressure / 1013.25, 1.0 / 5.255));

				cout
					<< "pressure: " << pressure << " hPa\n"
					<< "altitude: " << altitude << " m\n";
			}
		}
		else if (s.find("read_wind_speed") == 0)
		{
			wind_speed_data_t wind_speed_data_used;
			wind_speed_data_t *wind_speed_data_used_ptr = &wind_speed_data_used;

			if (wind_speed_data_reader_take_next_sample(g_wind_speed_reader, &wind_speed_data_used_ptr, NULL) != DDS_RETCODE_NO_DATA)
			{
				cout
					<< "wind speed\n"
					<< "current:     " << (wind_speed_data_used.speed_current / 100.0) << " km/h\n"
					<< "last minute: " << (wind_speed_data_used.speed_minute / 100.0) << " km/h\n"
					<< "last hour:   " << (wind_speed_data_used.speed_hour / 100.0) << " km/h\n";
			}
		}
		else if (s.find("read_rainfall") == 0)
		{
			rainfall_data_t rainfall_data_used;
			rainfall_data_t *rainfall_data_used_ptr = &rainfall_data_used;

			if (rainfall_data_reader_take_next_sample(g_rainfall_reader, &rainfall_data_used_ptr, NULL) != DDS_RETCODE_NO_DATA)
			{
				cout
					<< "rain amount\n"
					<< "current:     " << (rainfall_data_used.amount_current / 100.0) << " mm/h\n"
					<< "last minute: " << (rainfall_data_used.amount_minute / 100.0) << " mm/h\n"
					<< "last hour:   " << (rainfall_data_used.amount_hour / 100.0) << " mm/h\n";
			}
		}
		else if (s.find("read_wind_direction") == 0)
		{
			wind_direction_data_t wind_direction_data_used;
			wind_direction_data_t *wind_direction_data_used_ptr = &wind_direction_data_used;

			if (wind_direction_data_reader_take_next_sample(g_wind_direction_reader, &wind_direction_data_used_ptr, NULL) != DDS_RETCODE_NO_DATA)
			{
				cout << "wind direction: " << (wind_direction_data_used.degrees_multiplier * 22.5) << " degrees\n";
			}
		}
		else if (s == "quit")
		{
			break;
		}
		else
		{
			cerr << "unknown command\n";
		}
	}

	return 0;
}
