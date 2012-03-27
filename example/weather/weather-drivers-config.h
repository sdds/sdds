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
