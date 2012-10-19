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

#define sDDS_LOCAL_BYTEORDER_LITTLE_ENDIAN
//#define sDDS_LOCAL_BYTEORDER_BIG_ENDIAN
#define sDDS_NET_VERSION 0x01

#define sDDS_MAX_DATA_READERS 7
#define sDDS_MAX_DATA_WRITERS 7

#define sDDS_NET_MAX_OUT_QUEUE 2
#define sDDS_NET_MAX_BUF_SIZE 30
#define sDDS_NET_MAX_LOCATOR_COUNT 10
#define sDDS_QOS_DW1_LATBUD 100
#define sDDS_QOS_DW2_LATBUD 500
#define sDDS_TOPIC_HAS_SUB
#define sDDS_TOPIC_HAS_PUB
#define sDDS_TOPIC_APP_MSG_COUNT 2
#define sDDS_TOPIC_MAX_COUNT 2
#define sDDS_MNG_WORKER_CYCLE_TIME 10000
#define sDDS_MNG_BUILTINT_PUBCYCLE_PRESCALER 2


/* TWI bus speed in Hz */
#define TWI_BUS_SPEED 100000UL

/* TWI data and clock pin and port */
#define TWI_CLOCK_PORT D
#define TWI_DATA_PORT D
#define TWI_CLOCK_PIN 0
#define TWI_DATA_PIN 1


/* adc reference voltage, see adc.h */
#define WIND_VANE_ADC_REFERENCE ADC_USE_AVCC

/* adc channel for the wind vane */
#define WIND_VANE_ADC_CHANNEL 0

/* resistor used as voltage divider */
#define WIND_VANE_DIVIDER_RESISTOR 5.4f

/* internal = 10bit resoluti0on, pcf8591 = 8bit */
#define WIND_VANE_POSSIBLE_VALUES (1 << 10)

/* define port and pin for the anemometer interrupt */
#define ANEMOMETER_PORT E
#define ANEMOMETER_PIN 5

/* which interrupt is used for the anemometer (5 == PE5) */
#define ANEMOMETER_INTERRUPT 5

/* the external interrupt control register for this interrupt */
#define ANEMOMETER_EICR EICRB

/* how often is the callback called (in seconds) */
#define ANEMOMETER_CALLBACK_PERIOD 1

/* define port and pin for the rain gauge interrupt */
#define RAINGAUGE_PORT E
#define RAINGAUGE_PIN 4

/* which interrupt is used for the rain gauge (4 == PE4) */
#define RAINGAUGE_INTERRUPT 4

/* the external interrupt control register for this interrupt */
#define RAINGAUGE_EICR EICRB

/* how often is the callback called (in seconds) */
#define RAINGAUGE_CALLBACK_PERIOD 1

/* SHT15 bus speed in Hz */
#define SHT15_BUS_SPEED 100000

/* SHT15 data and clock pin and port */
#define SHT15_CLOCK_PORT D
#define SHT15_DATA_PORT D
#define SHT15_CLOCK_PIN 7
#define SHT15_DATA_PIN 5

/*
 * SHT15 temperature coefficients
 * 5V = -4010, 4V = -3980, 3.5V = -3970, 3V = -3960, 2.5V = -3940
 */
#define SHT15_TEMPERATURE_COEFFICIENT -3960

/*
 * SHT15 measurement resolution
 * can be 14bit temperature/12bit humidity
 * or 12bit temperature/8bit humidity
 */
#define SHT15_HIGH_RESOLUTION

/* settings for the AMN31112 PIR Sensor
 */

#define AMN31112_PORT B
#define AMN31112_PIN 5

/* settings for the TSL2561 light sensor
*/

#define TSL2561_TWI_ADDRESS (0x39<<1)

/*
 * Settings for the one wire bus
 */
#define ONEWIRE_PORT B
#define ONEWIRE_PIN 6


#endif   /* ----- #ifndef CONSTANTS_GEN_H_INC  ----- */
