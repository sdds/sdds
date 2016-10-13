/*
################################################################################
#  THIS FILE IS 100% GENERATED FOR sDDS; DO NOT EDIT EXCEPT EXPERIMENTALLY     #
#  Please refer to the README for information about making permanent changes.  #
################################################################################
*/
#ifndef LINUX_DEMOPUB3_CONSTANTS_H_INCLUDED
#define LINUX_DEMOPUB3_CONSTANTS_H_INCLUDED

#include "local_constants.h"

// -------------------------
// SDDS

#define SDDS_NET_VERSION 0x01

#ifndef SDDS_QOS_RELIABILITY_RELIABLE_SUBS_WITHOUT_PUBS
    #define SDDS_QOS_RELIABILITY_RELIABLE_SUBS_WITHOUT_PUBS 0
#endif

#ifdef FEATURE_SDDS_BUILTIN_TOPICS_ENABLED
    #define SDDS_MAX_DATA_WRITERS (6 + SDDS_QOS_RELIABILITY_RELIABLE_SUBS_WITHOUT_PUBS)
    #undef SDDS_DATA_READER_MAX_OBJS
    #define SDDS_DATA_READER_MAX_OBJS 5
#else
    #define SDDS_MAX_DATA_WRITERS (1 + SDDS_QOS_RELIABILITY_RELIABLE_SUBS_WITHOUT_PUBS)
    #undef SDDS_DATA_READER_MAX_OBJS
    #define SDDS_DATA_READER_MAX_OBJS 0
#endif

#ifndef SDDS_MAX_LINKED_LIST_NODES
#define SDDS_MAX_LINKED_LIST_NODES 20
#endif /* SDDS_MAX_LINKED_LIST_NODES */

#ifndef SDDS_NET_MAX_LOCATOR_COUNT
#define SDDS_NET_MAX_LOCATOR_COUNT 25
#endif /* SDDS_NET_MAX_LOCATOR_COUNT */

#ifndef SDDS_QOS_HISTORY_DEPTH
#define SDDS_QOS_HISTORY_DEPTH 20
#endif

#define SDDS_QOS_DW1_LATBUD 100
#define SDDS_QOS_DW2_LATBUD 500

#define SDDS_TOPIC_HAS_SUB

#ifdef SDDS_HAS_QOS_RELIABILITY
#   define SDDS_SEQNR_BIGGEST_TYPE uint32_t
#   define SDDS_SEQNR_BIGGEST_TYPE_BITSIZE 32
#   define SDDS_MAX_BLOCKING_TIME_BIGGEST_TYPE uint8_t
#endif

#ifndef SDDS_TOPIC_APP_MSG_COUNT
#define SDDS_TOPIC_APP_MSG_COUNT 5
#endif /* SDDS_TOPIC_APP_MSG_COUNT */

#ifdef FEATURE_SDDS_BUILTIN_TOPICS_ENABLED
#define SDDS_TOPIC_MAX_COUNT 6
#else
#define SDDS_TOPIC_MAX_COUNT 1
#endif

#define SDDS_MNG_WORKER_CYCLE_TIME 10000
#define SDDS_MNG_BUILTINT_PUBCYCLE_PRESCALER 2

// -------------------------
// PLATFORM

#define SDDS_LOCAL_BYTEORDER_LITTLE_ENDIAN

#ifndef SDDS_NET_MAX_OUT_QUEUE
#define SDDS_NET_MAX_OUT_QUEUE 2
#endif /*SDDS_NET_MAX_OUT_QUEUE */

#ifndef SDDS_NET_MAX_BUF_SIZE
#define SDDS_NET_MAX_BUF_SIZE 128
#endif /* SDDS_NET_MAX_BUF_SIZE */

/*
 * Network settings
 */
#ifndef PLATFORM_LINUX_SDDS_IFACE
#define PLATFORM_LINUX_SDDS_IFACE               "usb0"
#endif
#ifndef PLATFORM_LINUX_SDDS_IP_ADDRESS
#define PLATFORM_LINUX_SDDS_IP_ADDRESS 			"fe80::12:13ff:fe14:1516"
#endif
#ifndef PLATFORM_LINUX_SDDS_ADDRESS
#define PLATFORM_LINUX_SDDS_ADDRESS		PLATFORM_LINUX_SDDS_IP_ADDRESS"%"PLATFORM_LINUX_SDDS_IFACE
#endif

#ifndef PLATFORM_LINUX_SDDS_DEVICE
#define PLATFORM_LINUX_SDDS_DEVICE		PLATFORM_LINUX_SDDS_IFACE
#endif

#ifndef PLATFORM_LINUX_SDDS_PROTOCOL
#define PLATFORM_LINUX_SDDS_PROTOCOL			AF_INET6
#endif
#ifndef PLATFORM_LINUX_SDDS_PORT
#define PLATFORM_LINUX_SDDS_PORT				23234
#endif

// -------------------------
// DEVICE

/* TWI bus speed in Hz */
#define DRIVER_TWI_BUS_SPEED 10000UL // 10kh to use internal pull ups
//#define DRIVER_TWI_BUS_SPEED 100000UL

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

#define DRIVER_TSL2561_TWI_ADDRESS (0x29<<1)

/*
 * Settings for the one wire bus
 */
#define DRIVER_ONEWIRE_PORT E
#define DRIVER_ONEWIRE_PIN 6

#endif
/*
################################################################################
#  THIS FILE IS 100% GENERATED FOR sDDS; DO NOT EDIT EXCEPT EXPERIMENTALLY     #
#  Please refer to the README for information about making permanent changes.  #
################################################################################
*/
