#ifndef DRIVERS_H_INCLUDED
#define DRIVERS_H_INCLUDED

#include <stdint.h>

/*
 * initialize the wind vane sensor
 *
 * @return 0 on success, 1 on failure
 */
int wind_vane_init(void);

/* 
 * read the current position multiplier
 *
 * the wind vane has 16 possible positions, i.e. the value
 * that is read is between 0 and 15, multiply this with 22.5
 * and you have the final position in degrees
 *
 * @param value the current position multiplier [0,16)
 *
 * @return 0 on success, 1 on failure
 */
int wind_vane_read(uint8_t *value);



/* 
 * initialize the anemometer
 *
 * @return 0 on success, 1 on failure
 */
int anemometer_init(void);

/* 
 * read the current wind speed or for the last 60 or 3600 seconds in 0.01 km/h
 *
 * @return 0 on success, 1 on failure
 */
int anemometer_read_current(uint16_t *value);
int anemometer_read_minute(uint16_t *value);
int anemometer_read_hour(uint16_t *value);

/* 
 * the passed callback needs to be called every ANEMOMETER_CALLBACK_PERIOD (see config.h)
 * to keep track of the current speed
 */
void anemometer_register_callback(void(*callback)(void));

/* 
 * initialize the raingauge
 *
 * @return 0 on success, 1 on failure
 */
int raingauge_init(void);

/* 
 * read the current amount of rain or for the last 60 or 3600 seconds in 0.01 mm/h
 *
 * @return 0 on success, 1 on failure
 */
int raingauge_read_current(uint16_t *value);
int raingauge_read_minute(uint16_t *value);
int raingauge_read_hour(uint16_t *value);

/* 
 * the passed callback needs to be called every RAINGAUGE_CALLBACK_PERIOD (see config.h)
 * to keep track of the current speed
 */
void raingauge_register_callback(void(*callback)(void));

/* 
 * initialize the temperature and humidity sensor SHT15
 *
 * @return 0 on success, 1 on failure
 */
int sht15_init(void);

/* 
 * read the temperature in 0.01°C
 *
 * @param temperature the temperature if the function returns 0
 *
 * @return 0 on success, 1 on failure
 */
int sht15_read_temperature(int16_t *temperature);

/* 
 * read relative humidity in 0.01%
 *
 * @param humidity the relative humidity if the function returns 0
 *
 * @return 0 on success, 1 on failure
 */
int sht15_read_relative_humidity(uint16_t *humidity);

/* 
 * initialize the temperature and pressure sensor BMP085
 *
 * @return 0 on success, 1 on failure
 */
int bmp085_init(void);

/* 
 * read the temperature in 0.01°C
 *
 * @param temperature the temperature if the function returns 0
 *
 * @return 0 on success, 1 on failure
 */
int bmp085_read_temperature(int16_t *temperature);

/* 
 * read the pressure in 0.1 mbar (hPa) (which is 1 Pa)
 *
 * @param pressure the pressure if the function returns 0
 *
 * @return 0 on success, 1 on failure
 */
int bmp085_read_pressure(uint16_t *pressure);

#endif
