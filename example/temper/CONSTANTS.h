/*
 * =====================================================================================
 *
 *       Filename:  CONSTANTS.h
 *
 *    Description:  generated constants for the project
 *
 *        Version:  1.0
 *        Created:  22.02.2010 19:08:39
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Kai Beckmann (kai), kai-oliver.beckmann@hs-rm.de
 *        Company:  Hochschule RheinMain - DOPSY Labor für verteilte Systeme
 *
 * =====================================================================================
 */


#ifndef  CONSTANTS_GEN_H_INC
#define  CONSTANTS_GEN_H_INC

#ifndef __AVR_ATmega128RFA1__
#define __AVR_ATmega128RFA1__
#endif

// todo temp for eclipse
#ifndef SDDS_ARCH_x86
#define SDDS_ARCH_x86
#endif

#define SDDS_LOCAL_BYTEORDER_LITTLE_ENDIAN
//#define SDDS_LOCAL_BYTEORDER_BIG_ENDIAN
#define SDDS_NET_VERSION 0x01

#define SDDS_MAX_DATA_READERS 7
#define SDDS_MAX_DATA_WRITERS 7

#define SDDS_NET_MAX_OUT_QUEUE 2
#define SDDS_NET_MAX_BUF_SIZE 30
#define SDDS_NET_MAX_LOCATOR_COUNT 10
#define SDDS_QOS_DW1_LATBUD 100
#define SDDS_QOS_DW2_LATBUD 500
#define SDDS_TOPIC_HAS_SUB
#define SDDS_TOPIC_HAS_PUB
#define SDDS_TOPIC_APP_MSG_COUNT 2
#define SDDS_TOPIC_MAX_COUNT 2
#define SDDS_MNG_WORKER_CYCLE_TIME 10000
#define SDDS_MNG_BUILTINT_PUBCYCLE_PRESCALER 2


/* TWI bus speed in Hz */
#define DRIVER_TWI_BUS_SPEED 100000UL

/* TWI data and clock pin and port */
#define DRIVER_TWI_CLOCK_PORT D
#define DRIVER_TWI_DATA_PORT D
#define DRIVER_TWI_CLOCK_PIN 0
#define DRIVER_TWI_DATA_PIN 1


/* adc reference voltage, see adc.h */
#define DRIVER_WIND_VANE_ADC_REFERENCE ADC_USE_AVCC

/* adc channel for the wind vane */
#define DRIVER_WIND_VANE_ADC_CHANNEL 0

/* resistor used as voltage divider */
#define DRIVER_WIND_VANE_DIVIDER_RESISTOR 5.4f

/* internal = 10bit resoluti0on, pcf8591 = 8bit */
#define DRIVER_WIND_VANE_POSSIBLE_VALUES (1 << 10)

/* define port and pin for the anemometer interrupt */
#define DRIVER_ANEMOMETER_PORT E
#define DRIVER_ANEMOMETER_PIN 5

/* which interrupt is used for the anemometer (5 == PE5) */
#define DRIVER_ANEMOMETER_INTERRUPT 5

/* the external interrupt control register for this interrupt */
#define DRIVER_ANEMOMETER_EICR EICRB

/* how often is the callback called (in seconds) */
#define DRIVER_ANEMOMETER_CALLBACK_PERIOD 1

/* define port and pin for the rain gauge interrupt */
#define DRIVER_RAINGAUGE_PORT E
#define DRIVER_RAINGAUGE_PIN 4

/* which interrupt is used for the rain gauge (4 == PE4) */
#define DRIVER_RAINGAUGE_INTERRUPT 4

/* the external interrupt control register for this interrupt */
#define DRIVER_RAINGAUGE_EICR EICRB

/* how often is the callback called (in seconds) */
#define DRIVER_RAINGAUGE_CALLBACK_PERIOD 1

/* SHT15 bus speed in Hz */
#define DRIVER_SHT15_BUS_SPEED 100000

/* SHT15 data and clock pin and port */
#define DRIVER_SHT15_CLOCK_PORT D
#define DRIVER_SHT15_DATA_PORT D
#define DRIVER_SHT15_CLOCK_PIN 7
#define DRIVER_SHT15_DATA_PIN 5

/*
 * SHT15 temperature coefficients
 * 5V = -4010, 4V = -3980, 3.5V = -3970, 3V = -3960, 2.5V = -3940
 */
#define DRIVER_SHT15_TEMPERATURE_COEFFICIENT -3960

/*
 * SHT15 measurement resolution
 * can be 14bit temperature/12bit humidity
 * or 12bit temperature/8bit humidity
 */
#define DRIVER_SHT15_HIGH_RESOLUTION

/* settings for the AMN31112 PIR Sensor
 */

#define DRIVER_AMN31112_PORT B
#define DRIVER_AMN31112_PIN 5

/* settings for the TSL2561 light sensor
*/

#define DRIVER_TSL2561_TWI_ADDRESS (0x39<<1)

/*
 * Settings for the one wire bus
 */
#define DRIVER_ONEWIRE_PORT B
#define DRIVER_ONEWIRE_PIN 6


#endif   /* ----- #ifndef CONSTANTS_GEN_H_INC  ----- */
