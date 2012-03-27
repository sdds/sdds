#include "weather-drivers.h"
#include "weather-drivers-config.h"

#include "adc.h"
#include "twi.h"

#include <avr/interrupt.h>
#if defined(__OPTIMIZE__) && defined(F_CPU)
#include <util/delay.h>
#endif

#include <stdlib.h>

#define INTERNAL_CONCAT2(x, y) x ## y
#define INTERNAL_CONCAT(x, y) INTERNAL_CONCAT2(x, y)
#define INTERNAL_MAX(x, y) ((x) > (y) ? (x) : (y))
#define INTERNAL_MIN(x, y) ((x) < (y) ? (x) : (y))
#define ANEMOMETER_ISC(x) INTERNAL_CONCAT(ISC, INTERNAL_CONCAT(ANEMOMETER_INTERRUPT, x))
#define ANEMOMETER_INT INTERNAL_CONCAT(INT, ANEMOMETER_INTERRUPT)
#define ANEMOMETER_VECTOR INTERNAL_CONCAT(INT, INTERNAL_CONCAT(ANEMOMETER_INTERRUPT, _vect))
#define ANEMOMETER_INTF INTERNAL_CONCAT(INTF, ANEMOMETER_INTERRUPT)
#define ANEMOMETER_PORTR INTERNAL_CONCAT(PORT, ANEMOMETER_PORT)
#define ANEMOMETER_PORTVAL INTERNAL_CONCAT(P, INTERNAL_CONCAT(ANEMOMETER_PORT, ANEMOMETER_PIN))
#define RAINGAUGE_ISC(x) INTERNAL_CONCAT(ISC, INTERNAL_CONCAT(RAINGAUGE_INTERRUPT, x))
#define RAINGAUGE_INT INTERNAL_CONCAT(INT, RAINGAUGE_INTERRUPT)
#define RAINGAUGE_VECTOR INTERNAL_CONCAT(INT, INTERNAL_CONCAT(RAINGAUGE_INTERRUPT, _vect))
#define RAINGAUGE_INTF INTERNAL_CONCAT(INTF, RAINGAUGE_INTERRUPT)
#define RAINGAUGE_PORTR INTERNAL_CONCAT(PORT, RAINGAUGE_PORT)
#define RAINGAUGE_PORTVAL INTERNAL_CONCAT(P, INTERNAL_CONCAT(RAINGAUGE_PORT, RAINGAUGE_PIN))
#define SHT15_SDA_PINR INTERNAL_CONCAT(PIN, SHT15_DATA_PORT)
#define SHT15_SDA_PIN INTERNAL_CONCAT(PIN, INTERNAL_CONCAT(SHT15_DATA_PORT, SHT15_DATA_PIN))
#define SHT15_SDA_DDR INTERNAL_CONCAT(DDR, SHT15_DATA_PORT)
#define SHT15_SDA_DD INTERNAL_CONCAT(DD, INTERNAL_CONCAT(SHT15_DATA_PORT, SHT15_DATA_PIN))
#define SHT15_SDA_PORTR INTERNAL_CONCAT(PORT, SHT15_DATA_PORT)
#define SHT15_SDA_PORT INTERNAL_CONCAT(P, INTERNAL_CONCAT(SHT15_DATA_PORT, SHT15_DATA_PIN))
#define SHT15_SCL_PINR INTERNAL_CONCAT(PIN, SHT15_CLOCK_PORT)
#define SHT15_SCL_PIN INTERNAL_CONCAT(SHT15_SCL_PINR, SHT15_CLOCK_PIN)
#define SHT15_SCL_DDR INTERNAL_CONCAT(DDR, SHT15_CLOCK_PORT)
#define SHT15_SCL_DD INTERNAL_CONCAT(DD, INTERNAL_CONCAT(SHT15_CLOCK_PORT, SHT15_CLOCK_PIN))
#define SHT15_SCL_PORTR INTERNAL_CONCAT(PORT, SHT15_CLOCK_PORT)
#define SHT15_SCL_PORT INTERNAL_CONCAT(P, INTERNAL_CONCAT(SHT15_CLOCK_PORT, SHT15_CLOCK_PIN))

#ifndef __OPTIMIZE__
#ifndef F_CPU
#error "Please set F_CPU to the frequency of the target system in Hz."
#endif

// 8 cycles on 8MHz == 1µs
#define custom_delay_8cyc(x) \
	do \
	{ \
		asm volatile( \
			"nop\n"                    /* [1 cycle] */ \
			"nop\n"                    /* [1 cycle] */ \
			"cp  %A0, __zero_reg__ \n" /* count & 0xFF == 0     [1 cycle] */ \
			"cpc %B0, __zero_reg__ \n" /* (count & 0xFF00 >> 8) [1 cycle] */ \
			"breq 2f\n"                /* jump to end [1 cycle] */ \
			"1:\n" \
			"sbiw %0, 1\n"             /* count-- [count * 2 cycles] */ \
			"brne 1b\n"                /* loop if != 0 [(count - 1) * 2 cycles + 1 cycle] */ \
			"2:" \
			                           /* 4 * (count + 1) if count > 0, else 6 */ \
		::"w"((x)) \
		); \
	} while(0);

#define _delay_ms(x) custom_delay_8cyc(((x) * F_CPU) / 1000 / 8)
#define _delay_us(x) custom_delay_8cyc(((x) * F_CPU) / 1000 / 1000 / 8)

#endif

/* resistor used as voltage divider */
#define WIND_VANE_DIVIDER_RESISTOR 5.4f

/* maximum amount of switch positions */
#define WIND_VANE_MAX_SWITCHES 16

/* lookup table for wind vane conversions, calculated on initialization */
static uint16_t wind_vane_conversions[WIND_VANE_MAX_SWITCHES];

/* 
 * anemometer:
 * using factor 100, 2.4 * 100 = 240
 * max current: (255 ticks / 1 s) * 240 = 61200 (612 km/h)
 * max hour 8bit:    (255 ticks /    1 s) * 240 = 17 (0.17 km/h)
 * max hour 16bit: (65535 ticks / 3600 s) * 240 = 436900 (4369 km/h)
 */
static uint16_t g_anemometer_callback_calls;
static uint16_t g_anemometer_ticks[3];
static uint16_t g_anemometer_values[3]; // hour, minute, seconds

/* 
 * raingauge
 * using factor 10000, 0.2794 * 10000 = 2794
 * max current 8bit:    (255 ticks / 1 s) * 2794 = 712400 (71,24 mm/h)
 * max current 16bit: (65535 ticks / 1 s) * 2794 = 183104790 (18310,479 mm/h) (record 401 mm/h)
 * max hour 16bit:      (65535 ticks / 3600 s) * 2794 = 50862,4416667 (5,08624416667 mm/h)
 * max hour 32bit: (4294967295 ticks / 3600 s) * 2794 = 3333371839.51 (333337,183951 mm/h)
 */
static uint16_t g_raingauge_callback_calls;
static uint32_t g_raingauge_ticks[3];
static uint16_t g_raingauge_values[3]; // hour, minute, seconds

/* eeprom data for the bmp085 sensor */
struct
{
	int16_t ac1;
	int16_t ac2;
	int16_t ac3;
	uint16_t ac4;
	uint16_t ac5;
	uint16_t ac6;
	int16_t b1;
	int16_t b2;
	int16_t mb;
	int16_t mc;
	int16_t md;
} g_bmp085_eeprom;

int wind_vane_init(void)
{
	uint8_t i;

	/* lookup table for wind vane resistors, recalculated on initialization */
	float const wind_vane_resistors[WIND_VANE_MAX_SWITCHES] =
	{
		33,
		(33 * 8.2) / (33 + 8.2),
		8.2,
		(8.2 * 1) / (8.2 + 1),
		1,
		(1 * 2.2) / (1 + 2.2),
		2.2,
		(2.2 * 3.9) / (2.2 + 3.9),
		3.9,
		(3.9 * 16) / (3.9 + 16),
		16,
		(16 * 120) / (16 + 120),
		120,
		(120 * 64.9) / (120 + 64.9),
		64.9,
		(64.9 * 33) / (64.9 + 33)
	};

#if 1
	// for connection to the adc
	adc_init_mux(WIND_VANE_ADC_REFERENCE);
	adc_init();
#else
	// for connection to the twi
	twi_init();
#endif

	for (i = 0; i < WIND_VANE_MAX_SWITCHES; i++)
	{
		float const divider = wind_vane_resistors[i] / (wind_vane_resistors[i] + WIND_VANE_DIVIDER_RESISTOR);
		wind_vane_conversions[i] = WIND_VANE_POSSIBLE_VALUES * divider;
	}

	return 0;
}

/* find the closest possible match from the lookup table */
static uint8_t closest_wind_vane_direction_index(uint16_t value)
{
	uint8_t current_index;
	uint8_t min_index;
	uint16_t current_min;

	/* initialize first set, then loop */
	current_index = 0;
	current_min = abs(wind_vane_conversions[current_index] - value);
	min_index = 0;

	for (current_index = 1; current_index < WIND_VANE_MAX_SWITCHES; current_index++)
	{
		uint16_t distance;

		/* calculate the current distance to this lookup entry */
		distance = abs(wind_vane_conversions[current_index] - value);

		if (distance < current_min)
		{
			min_index = current_index;
			current_min = distance;
		}
	}

	return min_index;
}

int wind_vane_read(uint8_t *value)
{
	uint16_t read_value;

	// for connection to the adc input 1
	read_value = adc_read(WIND_VANE_ADC_CHANNEL);

	/* find the closest match from the lookup table */
	*value = closest_wind_vane_direction_index(read_value);

	return 0;
}

ISR(ANEMOMETER_VECTOR)
{
	g_anemometer_ticks[0]++;
	g_anemometer_ticks[1]++;
	g_anemometer_ticks[2]++;
}

static void anemometer_callback(void)
{
	uint16_t time_passed;

	g_anemometer_callback_calls++;

	time_passed = g_anemometer_callback_calls * ANEMOMETER_CALLBACK_PERIOD;

	if (time_passed % 60 == 0)
	{
		uint32_t temp_value;

		temp_value = (uint32_t)g_anemometer_ticks[0] * 240 / 60;
		temp_value /= 2;

		g_anemometer_values[0] = INTERNAL_MIN(UINT16_MAX, temp_value);
		g_anemometer_ticks[0] = 0;
	}

	if (time_passed % 3600 == 0)
	{
		uint32_t temp_value;

		temp_value = (uint32_t)g_anemometer_ticks[1] * 240 / 3600;
		temp_value /= 2;

		g_anemometer_values[1] = INTERNAL_MIN(UINT16_MAX, temp_value);
		g_anemometer_ticks[1] = 0;

		// reset every hour
		g_anemometer_callback_calls = 0;
	}

	g_anemometer_values[2] = INTERNAL_MIN(UINT16_MAX, (g_anemometer_ticks[2] * 240) / ANEMOMETER_CALLBACK_PERIOD);
	g_anemometer_ticks[2] = 0;
}

int anemometer_init(void)
{
	ANEMOMETER_PORTR |= _BV(ANEMOMETER_PORTVAL);

	// prevent an interrupt to occur when changing ISC bits
	EIMSK &= ~_BV(ANEMOMETER_INT);

#if 1
	// positive edge triggered
	ANEMOMETER_EICR |= _BV(ANEMOMETER_ISC(1)) | _BV(ANEMOMETER_ISC(0));
#else
	// negative edge triggered
	ANEMOMETER_EICR |= _BV(ANEMOMETER_ISC(1));
	ANEMOMETER_EICR &= ~_BV(ANEMOMETER_ISC(0));
#endif

	// clear interrupt flag, so it can't get fired until re-enabling
	EIFR |= _BV(ANEMOMETER_INTF);

	// enable the interrupt again
	EIMSK |= _BV(ANEMOMETER_INT);

	anemometer_register_callback(anemometer_callback);

	return 0;
}

int anemometer_read_current(uint16_t *value)
{
	*value = g_anemometer_values[2];

	return 0;
}

int anemometer_read_minute(uint16_t *value)
{
	*value = g_anemometer_values[0];

	return 0;
}

int anemometer_read_hour(uint16_t *value)
{
	*value = g_anemometer_values[1];

	return 0;
}

ISR(RAINGAUGE_VECTOR)
{
	g_raingauge_ticks[0]++;
	g_raingauge_ticks[1]++;
	g_raingauge_ticks[2]++;
}

static void raingauge_callback(void)
{
	uint16_t time_passed;

	g_raingauge_callback_calls++;

	time_passed = g_raingauge_callback_calls * RAINGAUGE_CALLBACK_PERIOD;

	if (time_passed % 60 == 0)
	{
		uint64_t temp_value;

		temp_value = (uint64_t)g_raingauge_ticks[0] * 2749 / 60;
		temp_value /= 2;

		/* temp_value has factor 10000, repr has 100 */
		g_raingauge_values[0] = INTERNAL_MIN(UINT16_MAX, temp_value / 100);
		g_raingauge_ticks[0] = 0;
	}

	if (time_passed % 3600 == 0)
	{
		uint64_t temp_value;

		temp_value = (uint64_t)g_raingauge_ticks[1] * 2749 / 3600;
		temp_value /= 2;

		/* temp_value has factor 10000, repr has 100 */
		g_raingauge_values[1] = INTERNAL_MIN(UINT16_MAX, temp_value / 100);
		g_raingauge_ticks[1] = 0;

		// reset every hour
		g_raingauge_callback_calls = 0;
	}

	/* temp_value has factor 10000, repr has 100 */
	g_raingauge_values[2] = INTERNAL_MIN(UINT16_MAX, (g_raingauge_ticks[2] * 2749) / 100 / RAINGAUGE_CALLBACK_PERIOD);
	g_raingauge_ticks[2] = 0;
}

int raingauge_init(void)
{
	RAINGAUGE_PORTR |= _BV(RAINGAUGE_PORTVAL);

	// prevent an interrupt to occur when changing ISC bits
	EIMSK &= ~_BV(RAINGAUGE_INT);

#if 1
	// positive edge triggered
	RAINGAUGE_EICR |= _BV(RAINGAUGE_ISC(1)) | _BV(RAINGAUGE_ISC(0));
#else
	// negative edge triggered
	RAINGAUGE_EICR |= _BV(RAINGAUGE_ISC(1));
	RAINGAUGE_EICR &= ~_BV(RAINGAUGE_ISC(0));
#endif

	// clear interrupt flag, so it can't get fired until re-enabling
	EIFR |= _BV(RAINGAUGE_INTF);

	// enable the interrupt again
	EIMSK |= _BV(RAINGAUGE_INT);

	raingauge_register_callback(raingauge_callback);

	return 0;
}

int raingauge_read_current(uint16_t *value)
{
	*value = g_raingauge_values[2];

	return 0;
}

int raingauge_read_minute(uint16_t *value)
{
	*value = g_raingauge_values[0];

	return 0;
}

int raingauge_read_hour(uint16_t *value)
{
	*value = g_raingauge_values[1];

	return 0;
}

static inline uint8_t sht15_sda_bit_is_set(void)
{
	return (SHT15_SDA_PINR & _BV(SHT15_SDA_PIN)) != 0;
}

static inline void sht15_scl_high(void)
{
	SHT15_SCL_PORTR |= _BV(SHT15_SCL_PORT);
}

static inline void sht15_scl_low(void)
{
	SHT15_SCL_PORTR &= ~_BV(SHT15_SCL_PORT);
}

static inline void sht15_sda_high(void)
{
	// due to the use of pull-ups, just release pin
	SHT15_SDA_DDR &= ~_BV(SHT15_SDA_DD);
}

static inline void sht15_sda_low(void)
{
	// due to the use of pull-ups, setting pin = pulling down
	SHT15_SDA_DDR |= _BV(SHT15_SDA_DD);
}

static inline void sht15_delay(void)
{
	// half period wait
	_delay_us((int32_t)1.0 * 1000 * 1000 / (SHT15_BUS_SPEED * 2));
}

static inline void sht15_pulse(void)
{
	sht15_scl_high();
	sht15_delay();
	sht15_scl_low();
}

static void sht15_start(void)
{
	sht15_scl_high();
	sht15_delay();
	sht15_sda_low();
	sht15_delay();
	// now low pulse
	sht15_scl_low();
	sht15_delay();
	sht15_scl_high();
	sht15_delay();
	// back to initial state
	sht15_sda_high();
	sht15_delay();
	sht15_scl_low();
	sht15_delay();
}

static int sht15_write(uint8_t byte)
{
	uint8_t i;
	uint8_t ack_received;

	for (i = 0; i < 8; i++)
	{
		if (byte & (1 << 7))
			sht15_sda_high();
		else
			sht15_sda_low();

		byte <<= 1;

		sht15_delay();
		sht15_pulse();
	}

	sht15_sda_high();
	sht15_delay();

	// slave pulls down data if ACKed
	ack_received = sht15_sda_bit_is_set() == 0;

	sht15_pulse();

	return ack_received;
}

static uint8_t sht15_read(uint8_t ack)
{
	uint8_t i;
	uint8_t byte;

	byte = 0;

	for (i = 0; i < 8; i++)
	{
		if (sht15_sda_bit_is_set())
			byte |= 1;

		if (i != 7)
			byte <<= 1;

		sht15_pulse();
		sht15_delay();
	}

	// master pulls down data if ACKed
	if (ack)
		sht15_sda_low();
	else
		sht15_sda_high();

	sht15_delay();
	sht15_pulse();

	// only need to reset pin if ACKed
	if (ack)
		sht15_sda_high();

	sht15_delay();

	return byte;
}

static void sht15_reset_communication(void)
{
	uint8_t i;

	// at least 9 pulses
	for (i = 0; i < 9; i++)
	{
		sht15_pulse();
		sht15_delay();
	}
}

static int sht15_read_16bit(uint8_t command, int16_t *word)
{
	int16_t value;

	sht15_reset_communication();
	sht15_start();

	if (sht15_write(command) == 0)
		return 1;

	// watchdog will prevent timing out... hopefully :)
	while (sht15_sda_bit_is_set())
		;

	value = sht15_read(1) << 8 | sht15_read(0);

	*word = value;

	return 0;
}

int sht15_init(void)
{
	// set sda tri-state
	SHT15_SDA_PORTR &= ~_BV(SHT15_SDA_PORT);
	SHT15_SDA_DDR &= ~_BV(SHT15_SDA_DD);

	// set scl low
	SHT15_SCL_PORTR &= ~_BV(SHT15_SCL_PORT);
	SHT15_SCL_DDR |= _BV(SHT15_SCL_DD);

	_delay_ms(11);

	// initial software reset
	sht15_reset_communication();
	sht15_start();
	sht15_write(0x1E);

	_delay_ms(11);

#ifndef SHT15_HIGH_RESOLUTION
	// setup resolution if required
	sht15_reset_communication();
	sht15_start();

	// write status register
	if (sht15_write(0x6) == 0)
		return 1;

	// write low resolution bit
	if (sht15_write(0x1) == 0)
		return 1;
#endif

	return 0;
}

int sht15_read_temperature(int16_t *temperature)
{
	int16_t sensor_temp;

	enum
	{
		/* 
		 * coefficients specified in the datasheet
		 * adjusted to give a non-floating integer
		 */

#ifdef SHT15_HIGH_RESOLUTION
		d1 = SHT15_TEMPERATURE_COEFFICIENT, // factor 100
		d2 = 1                              // factor 100
#else
		d1 = SHT15_TEMPERATURE_COEFFICIENT, // factor 100
		d2 = 4                              // factor 100
#endif
	};

	// read sensor temperature
	if (sht15_read_16bit(0x03, &sensor_temp) != 0)
		return 1;

	// result has factor 100
	*temperature = (int32_t)d1 + (int32_t)d2 * sensor_temp;

	return 0;
}

int sht15_read_relative_humidity(uint16_t *humidity)
{
	enum
	{
		/*
		 * coefficients specified in the datasheet
		 * adjusted to give a non-floating integer
		 */
#ifdef SHT15_HIGH_RESOLUTION
		c1 = -20468,   // factor = 10000
		c2 = 367,      // factor = 10000
		c3 = -15955,   // factor = 10000000000
		t1 = 1,        // factor = 100
		t2 = 8         // factor = 100000
#else
		c1 = -20468,   // factor = 10000
		c2 = 5872,     // factor = 10000
		c3 = -4085400, // factor = 10000000000
		t1 = 1,        // factor = 100
		t2 = 128       // factor = 100000
#endif
	};

	/*
	 * linear_hum = c1 + c2 * sensor_hum + c3 * sensor_hum * sensor_hum
	 * c3 has the highest factor, so adjust to that factor
	 * c1 -> c3 = 1000000
	 * c2 -> c3 = 1000000
	 *
	 * as the highest factor is beyond the range of a 32bit
	 * integer, use 64bit
	 */
	int16_t sensor_hum;
	int64_t linear_hum;

	if (sht15_read_16bit(0x05, &sensor_hum) != 0)
		return 1;

	linear_hum = (int64_t)c1 * 1000000;
	linear_hum += (int64_t)c2 * sensor_hum * 1000000;
	linear_hum += (int64_t)c3 * sensor_hum * sensor_hum;

	/*
	 * if the temperature is significantly different from 25°C
	 * it needs a correction
	 */
	{
		int16_t sensor_temp;;
		int32_t comp_temp;

		/*
		 * the forumla is hum_true = (sensor_temp - 25) * (t1 + t2 * sensor_hum) +
		 *                           linear_hum
		 * t2 has the highest factor, so adjust to that factor
		 * t1 -> t2 = 1000
		 * sensor_temp -> t2 = 1000
		 */
		if (sht15_read_temperature(&sensor_temp) != 0)
			return 1;

		// sensor_temp is factor 100, 25 * 100 = 2500
		comp_temp = sensor_temp - 2500;
		// 16bit * 1000 could overflow, so expand
		comp_temp *= (int32_t)t1 * 1000 + (int32_t)t2 * sensor_hum;

		/*
		 * linear_hum has factor 10000000000
		 * comp_temp has factor 100000 (t2) * 100 (left side) = 10000000
		 * comp_temp -> linear_hum = 1000
		 * result needs to be factor 100, hence / 100000000
		 */
		*humidity = (linear_hum + (int64_t)comp_temp * 1000) / 100000000;
	}

	return 0;
}

int bmp085_init(void)
{
	uint8_t i;
	twi_data_t data[27];
	uint8_t failed_command;

	twi_init();

	data[0].type = TWI_TYPE_START;

	data[1].type = TWI_TYPE_TRANSMIT_ADDRESS;
	data[1].byte = 0xEE;

	data[2].type = TWI_TYPE_TRANSMIT_DATA;
	data[2].byte = 0xAA;

	data[3].type = TWI_TYPE_START;

	data[4].type = TWI_TYPE_RECEIVE_ADDRESS;
	data[4].byte = 0xEF;

	for (i = 0; i < 22; i++)
		data[4 + 1 + i].type = TWI_TYPE_RECEIVE_DATA;

	if (twi_communicate(data, sizeof data / sizeof *data, &failed_command) != 0)
		return 1;

	g_bmp085_eeprom.ac1 =  (data[5].byte << 8) | data[6].byte;
	g_bmp085_eeprom.ac2 =  (data[7].byte << 8) | data[8].byte;
	g_bmp085_eeprom.ac3 =  (data[9].byte << 8) | data[10].byte;
	g_bmp085_eeprom.ac4 = (data[11].byte << 8) | data[12].byte;
	g_bmp085_eeprom.ac5 = (data[13].byte << 8) | data[14].byte;
	g_bmp085_eeprom.ac6 = (data[15].byte << 8) | data[16].byte;
	g_bmp085_eeprom.b1 =  (data[17].byte << 8) | data[18].byte;
	g_bmp085_eeprom.b2 =  (data[19].byte << 8) | data[20].byte;
	g_bmp085_eeprom.mb =  (data[21].byte << 8) | data[22].byte;
	g_bmp085_eeprom.mc =  (data[23].byte << 8) | data[24].byte;
	g_bmp085_eeprom.md =  (data[25].byte << 8) | data[26].byte;

	return 0;
}

static int bmp085_read_temperature_and_b5(int16_t *temperature, int32_t *b5_store)
{
	twi_data_t data[7];
	uint8_t failed_command;
	int32_t x1;
	int32_t x2;
	int32_t b5;
	int32_t raw_temperature;

	twi_init();

	data[0].type = TWI_TYPE_START;

	data[1].type = TWI_TYPE_TRANSMIT_ADDRESS;
	data[1].byte = 0xEE;

	data[2].type = TWI_TYPE_TRANSMIT_DATA;
	data[2].byte = 0xF4;

	data[3].type = TWI_TYPE_TRANSMIT_DATA;
	data[3].byte = 0x2E;

	if (twi_communicate(data, 4, &failed_command) != 0)
		return 1;

	/* datasheet says 4.5, be on the safe side here */
	_delay_ms(10);

	data[1].type = TWI_TYPE_TRANSMIT_ADDRESS;
	data[1].byte = 0xEE;

	data[2].type = TWI_TYPE_TRANSMIT_DATA;
	data[2].byte = 0xF6;

	data[3].type = TWI_TYPE_START;

	data[4].type = TWI_TYPE_RECEIVE_ADDRESS;
	data[4].byte = 0xEF;

	data[5].type = TWI_TYPE_RECEIVE_DATA;

	data[6].type = TWI_TYPE_RECEIVE_DATA;

	if (twi_communicate(data, sizeof data / sizeof *data, &failed_command) != 0)
		return 1;

	raw_temperature = ((data[5].byte << 8) | data[6].byte);
	x1 = (raw_temperature - g_bmp085_eeprom.ac6) * g_bmp085_eeprom.ac5 / ((int32_t)1 << 15);
	x2 = (int32_t)g_bmp085_eeprom.mc * (1 << 11) / (x1 + g_bmp085_eeprom.md);
	b5 = x1 + x2;

	if (temperature)
		*temperature = (b5 + 8) / (1 << 4) * 10;

	if (b5_store)
		*b5_store = b5;

	return 0;
}

int bmp085_read_temperature(int16_t *temperature)
{
	if (bmp085_read_temperature_and_b5(temperature, NULL) != 0)
		return 1;

	return 0;
}

int bmp085_read_pressure(uint16_t *pressure)
{
	twi_data_t data[7];
	uint8_t failed_command;
	int32_t x1;
	int32_t x2;
	int32_t x3;
	int32_t b3;
	uint32_t b4;
	int32_t b5;
	int32_t b6;
	uint32_t b7;
	int32_t temp_pressure;
	enum { oss = 0 };

	twi_init();

	data[0].type = TWI_TYPE_START;

	data[1].type = TWI_TYPE_TRANSMIT_ADDRESS;
	data[1].byte = 0xEE;

	data[2].type = TWI_TYPE_TRANSMIT_DATA;
	data[2].byte = 0xF4;

	data[3].type = TWI_TYPE_TRANSMIT_DATA;
	data[3].byte = 0x34;

	if (twi_communicate(data, 4, &failed_command) != 0)
		return 1;

	/* datasheet says 4.5, be on the safe side here */
	_delay_ms(10);

	data[1].type = TWI_TYPE_TRANSMIT_ADDRESS;
	data[1].byte = 0xEE;

	data[2].type = TWI_TYPE_TRANSMIT_DATA;
	data[2].byte = 0xF6;

	data[3].type = TWI_TYPE_START;

	data[4].type = TWI_TYPE_RECEIVE_ADDRESS;
	data[4].byte = 0xEF;

	data[5].type = TWI_TYPE_RECEIVE_DATA;

	data[6].type = TWI_TYPE_RECEIVE_DATA;

	if (twi_communicate(data, sizeof data / sizeof *data, &failed_command) != 0)
		return 1;

	if (bmp085_read_temperature_and_b5(NULL, &b5) != 0)
		return 1;

	temp_pressure = ((int32_t)data[5].byte << 8) | data[6].byte;

	b6 = b5 - 4000;
	x1 = ((int32_t)g_bmp085_eeprom.b2 * (b6 * b6 / (1 << 12))) / (1 << 11);
	x2 = (int32_t)g_bmp085_eeprom.ac2 * b6 / (1 << 11);
	x3 = x1 + x2;
	b3 = ((((int32_t)g_bmp085_eeprom.ac1 * 4 + x3) << oss) + 2) / 4;
	x1 = (int32_t)g_bmp085_eeprom.ac3 * b6 / (1 << 13);
	x2 = ((int32_t)g_bmp085_eeprom.b1 * (b6 * b6 / (1 << 12))) / ((int32_t)1 << 16);
	x3 = ((x1 + x2) + 2) / (1 << 2);
	b4 = (int32_t)g_bmp085_eeprom.ac4 * (uint32_t)(x3 + 32768) / ((int32_t)1 << 15);
	b7 = ((uint32_t)temp_pressure - b3) * (50000 >> oss);
	b4 = (int32_t)g_bmp085_eeprom.ac4 * (uint32_t)(x3 + 32768) / ((int32_t)1 << 15);

	if (b7 < (uint32_t)0x80000000)
		temp_pressure = (b7 * 2) / b4;
	else
		temp_pressure = (b7 / b4) * 2;

	x1 = (temp_pressure / (1 << 8)) * (temp_pressure / (1 << 8));
	x1 = (x1 * 3038) / ((int32_t)1 << 16);
	x2 = (-7357 * temp_pressure) / ((int32_t)1 << 16);

	temp_pressure += (x1 + x2 + 3791) / (1 << 4);

	// 1 pA => 10 hPa
	*pressure = temp_pressure / 10;

	return 0;
}
