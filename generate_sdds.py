
#!/usr/bin/env python

decl_headers = r"""
#include <dds/DDS_DCPS.h>
"""

impl_headers = r"""
#include <os-ssal/Memory.h>
#include <sdds/DataSink.h>
#include <sdds/DataSource.h>
#include <sdds/LocatorDB.h>
#include <sdds/Log.h>
#include <sdds/Network.h>
#include <sdds/sDDS.h>
/*#include <sdds_conn.h>*/
"""

impl_decl = r"""
//int init_sdds(int log_level);
"""

impl_init = r"""
rc_t sDDS_init(void)
{
	rc_t ret;
	
	Memory_init();
	LocatorDB_init();
	DataSource_init();
	DataSink_init();
	Network_init();
	BuiltinTopic_init();
	
    int log_level = 3;

	Log_setLvl(log_level);

%s

	return SDDS_RT_OK;
}
"""

impl_setListener = r"""
DDS_ReturnCode_t DDS_DataReader_set_listener(
	DDS_DataReader _this,
	const struct DDS_DataReaderListener* a_listener,
	const DDS_StatusMask mask
)
{
	rc_t ret = DataSink_set_on_data_avail_listener((DataReader) _this, (On_Data_Avail_Listener) a_listener->on_data_available, (const StatusMask) mask);
	if (ret == SDDS_RT_OK)
		return DDS_RETCODE_OK;

	return DDS_RETCODE_ERROR;
}
"""

def get_decl(f, impl_name, datastructures):
	f.write(r"""
#ifndef %(up_name)s_SDDS_IMPL_H_INC
#define %(up_name)s_SDDS_IMPL_H_INC

"""[1:] % {'up_name': impl_name.upper(), 'cap_name': impl_name[0].capitalize() + impl_name[1:]})

	for ds in datastructures:
		f.write('#include "%s-ds.h"\n' % ds['name'])

	f.write(decl_headers + '\n')

	for ds in datastructures:
		f.write(r"""
extern DDS_DataReader g_%(name)s_reader;
extern DDS_DataWriter g_%(name)s_writer;
extern DDS_Topic g_%(name)s_topic;
//extern %(name)s_data_t g_%(name)s_pool[sDDS_TOPIC_APP_MSG_COUNT];
"""[1:] % {'name': ds['name'][0].capitalize() + ds['name'][1:]})

		f.write('\n')

	f.write(impl_decl[1:] + '\n')

	f.write(r"""
#endif
"""[1:])

def get_impl(f, impl_name, datastructures):
	f.write(r"""
#include "%(name)s_sdds_impl.h"

"""[1:-1] % {'name': impl_name})

	f.write(impl_headers + '\n')
	
	f.write(impl_setListener + '\n')

	def_string = ''

	for ds in datastructures:
		if ds['subscriber'] or ds['publisher']:
			def_string += 'DDS_Topic g_%s_topic;\n' % (ds['name'][0].capitalize() + ds['name'][1:])
			def_string += '%(name)s g_%(name)s_pool[sDDS_TOPIC_APP_MSG_COUNT];\n' % {'name': ds['name'][0].capitalize() + ds['name'][1:]}

		if ds['subscriber']:
			def_string += 'DDS_DataReader g_%s_reader;\n' % (ds['name'][0].capitalize() + ds['name'][1:])

		if ds['publisher']:
			def_string += 'DDS_DataWriter g_%s_writer;\n' % (ds['name'][0].capitalize() + ds['name'][1:])
	
		def_string += '\n'

	f.write(def_string)

	impl_string = ''
	impl_string += '\tLocator l;\n'

	for ds in datastructures:
		if ds['subscriber'] or ds['publisher']:
			impl_string += '\tg_%(name)s_topic = sDDS_%(name)sTopic_create(g_%(name)s_pool, sDDS_TOPIC_APP_MSG_COUNT);\n' % {'name': ds['name'][0].capitalize() + ds['name'][1:]}

		if ds['subscriber']:
			impl_string += '\tg_%(name)s_reader = DataSink_create_datareader(g_%(name)s_topic, NULL, NULL /*&sdds_listener*/, NULL);\n' % {'name': ds['name'][0].capitalize() + ds['name'][1:]}

		if ds['publisher']:
			impl_string += '\tg_%(name)s_writer = DataSource_create_datawriter(g_%(name)s_topic, NULL, NULL, NULL);\n' % {'name': ds['name'][0].capitalize() + ds['name'][1:]}
			impl_string += '\n'
			for addr in ds['addresses']:
				impl_string += '\n'
				impl_string += '\tret = LocatorDB_newLocator(&l);\n'
				impl_string += '\tif (ret != SDDS_RT_OK)\n'
				impl_string += '\t\treturn ret;\n'
				impl_string += '\n'
				impl_string += '\tLocator_upRef(l);\n'
				impl_string += '\n'
				impl_string += '\tret = Network_setAddressToLocator(l, "%(addr)s");\n' % {'addr': addr.rstrip()}
				impl_string += '\tif (ret != SDDS_RT_OK)\n'
				impl_string += '\t\treturn ret;\n'
				impl_string += '\n'
				impl_string += '\tret = Topic_addRemoteDataSink(g_%(name)s_topic, l);\n' % {'name' : ds['name'][0].capitalize() + ds['name'][1:]}
				impl_string += '\tif (ret != SDDS_RT_OK)\n'
				impl_string += '\t\treturn ret;\n'
				impl_string += '\tLocator_downRef(l);\n'
				
		impl_string += '\n'

	f.write(impl_init[1:] % impl_string[:-2])

def make_data(role, roles, finalize = True):
	import copy

	empty_role = {
		'name': None,
		'is_subscriber': False,
		'is_publisher': False
	}

	if not role:
		role = copy.deepcopy(empty_role)
	elif finalize:
		if role['name']:
			roles.append(role)
			role = copy.deepcopy(empty_role)

	return role

def get_info(filename, datastructures):
	f = open(filename, 'r')

	roles = []
	role = make_data(None, roles)
	impl = {'endian': None, 'roles': None}

	for datastructure in datastructures:
		datastructure['subscriber'] = False
		datastructure['publisher'] = False

	for line in f:
		if line[0] == '[' and line[:-1]:
			lst = line.split()			
			possible_name = lst[0][1:-1]
		
			role = make_data(role, roles)

			if role['name']:
				print '[%s] invalid, name already parsed ([%s])' % (possible_name, role['name'])
				sys.exit(1)

			role['name'] = possible_name
			continue

		if line.startswith('endian:'):
			possible_endian = line[len('endian:'):-1].strip()

			if role['name']:
				print 'endian %s in %s invalid, because in a name declaration' % (possible_endian, role['name'])

			impl['endian'] = possible_endian
			continue

		token = None
		addr_list = None

		if line.startswith('publisher'):
			if not role['name']:
				print 'can\'t publish, because no name yet'
				sys.exit(1)
			token = 'publisher'
			addr_list = line.split(' ')
			addr_list.pop(0)
			for s in addr_list:
				s.rstrip()
				
		elif line.startswith('subscriber'):
			if not role['name']:
				print 'can\'t subscribe, because no name yet'
				sys.exit(1)
			token = 'subscriber'
		else:
			continue

		found = None

		for i, datastructure in enumerate(datastructures):
			if datastructure['name'] == role['name']:
				found = i
				break

		if found == None:
			print 'can\'t work with %s, there\'s no data structure for it' % role['name']
			sys.exit(1)

		datastructures[found][token] = True
		datastructures[found]['addresses'] = addr_list
		role['is_%s' % token] = True

	make_data(role, roles)

	impl['roles'] = roles

	return impl

import generate_ds
import sys

ds_filename = None

if len(sys.argv) < 2:
	print 'Usage: %s <implementation name> [<datastructures file>]' % sys.argv[0]
	sys.exit(1)

if len(sys.argv) >= 3:
	ds_filename = sys.argv[2]

datastructures = generate_ds.generate_datastructures(ds_filename)

extended_topic_support = ''

for ds in datastructures:
	topicID = int(ds['topic'],16)
	if topicID > 15:
		extended_topic_support = "#define sDDS_EXTENDED_TOPIC_SUPPORT"
	

impl_name = sys.argv[1]

info = get_info('%s-dds-roles' % impl_name, datastructures)

subscribed_topics = []
published_topics = []

for role in info['roles']:
	if role['is_subscriber']:
		subscribed_topics.append(role['name'])
	if role['is_publisher']:
		published_topics.append(role['name'])

if info['endian'] == 'little' or info['endian'] == 'big':
	pass
else:
	print 'you did not specify a byte order for this implementation'
	sys.exit(1)

byte_order = info['endian']
data_readers = len(subscribed_topics)
data_writers = len(published_topics)

subscribed_topics = ','.join(subscribed_topics)
published_topics = ','.join(published_topics)

any_subscriptions = ''
any_publications = ''

if data_readers > 0:
	any_subscriptions = '#define sDDS_TOPIC_HAS_PUB'

if data_writers > 0:
	any_publications = '#define sDDS_TOPIC_HAS_SUB'

fdecl = open('%s_sdds_impl.h' % impl_name, 'w')
fimpl = open('%s_sdds_impl.c' % impl_name, 'w')

get_decl(fdecl, impl_name, datastructures)
get_impl(fimpl, impl_name, datastructures)

fconstants = open('%s_constants.h' % impl_name, 'w')
fconstants.write(r"""
#ifndef %(up_name)s_CONSTANTS_H_INCLUDED
#define %(up_name)s_CONSTANTS_H_INCLUDED

#include "local_constants.h"

#define sDDS_LOCAL_BYTEORDER_%(byte_order)s_ENDIAN
#define sDDS_NET_VERSION 0x01
#ifdef sDDS_BUILTIN_TOPICS_ENABLED
#define sDDS_MAX_DATA_WRITERS %(data_writers_builtin)s
#define sDDS_MAX_DATA_READERS %(data_readers_builtin)s
#else
#define sDDS_MAX_DATA_WRITERS %(data_writers)s
#define sDDS_MAX_DATA_READERS %(data_readers)s
#endif

#ifndef sDDS_NET_MAX_OUT_QUEUE
#define sDDS_NET_MAX_OUT_QUEUE 2
#endif /*sDDS_NET_MAX_OUT_QUEUE */

#ifndef sDDS_NET_MAX_BUF_SIZE
#define sDDS_NET_MAX_BUF_SIZE 30
#endif /* sDDS_NET_MAX_BUF_SIZE */

#ifndef sDDS_NET_MAX_LOCATOR_COUNT
#define sDDS_NET_MAX_LOCATOR_COUNT 20
#endif /* sDDS_NET_MAX_LOCATOR_COUNT */

#define sDDS_QOS_DW1_LATBUD 100
#define sDDS_QOS_DW2_LATBUD 500
%(any_subscriptions)s
%(any_publications)s

#ifndef sDDS_TOPIC_APP_MSG_COUNT
#define sDDS_TOPIC_APP_MSG_COUNT 5
#endif /* sDDS_TOPIC_APP_MSG_COUNT */

#ifdef sDDS_BUILTIN_TOPICS_ENABLED
#define sDDS_TOPIC_MAX_COUNT %(max_topics_builtin)d
#else
#define sDDS_TOPIC_MAX_COUNT %(max_topics)d
#endif

#define sDDS_MNG_WORKER_CYCLE_TIME 10000
#define sDDS_MNG_BUILTINT_PUBCYCLE_PRESCALER 2

%(extended_topic_support)s

/* TWI bus speed in Hz */
#define TWI_BUS_SPEED 10000UL // 10kh to use internal pull ups
//#define TWI_BUS_SPEED 100000UL

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

#define TSL2561_TWI_ADDRESS (0x29<<1)

/*
 * Settings for the one wire bus
 */
#define ONEWIRE_PORT E
#define ONEWIRE_PIN 6

/* Settings for LEDS
*/

//#define DRV_LED_DIMMING 1

//#define DRV_LED_DIMMING_GAMMACORRECTION
/* value is the bit length of the pwm register */
//#define DRV_LED_DIMMING_GAMMACORRECTION_10BIT
/*#define DRV_LED_DIMMING_GAMMACORRECTION_16BIT*/
/*#define DRV_LED_DIMMING_GAMMACORRECTION_8BIT */

/*
 * Settings for WieDAS
 */

#define WIEDAS_SENSORAPP_DOOR_INTERVALL			10
#define WIEDAS_SENSORAPP_WINDOW_INTERVALL		10
#define WIEDAS_SENSORAPP_SWITCH_INTERVALL		10
#define WIEDAS_SENSORAPP_TEMPERATUR_INTERVALL	5
#define WIEDAS_SENSORAPP_LIGHT_INTERVALL		5
#define WIEDAS_SENSORAPP_HUMIDITY_INTERVALL		10
#define WIEDAS_SENSORAPP_MOVEMENT_INTERVALL		10
#define WIEDAS_SENSORAPP_BATTERY_INTERVALL		60
#define WIEDAS_SENSORAPP_SIMPLELAMP_INTERVALL	10
#define WIEDAS_SENSORAPP_DIMMERLAMP_INTERVALL	10
#define WIEDAS_SENSORAPP_ONOFFOUTPUT_INTERVALL	10
#define WIEDAS_SENSORAPP_POWERMETER_INTERVALL	5


#endif
"""[1:] % \
{ \
	'byte_order': byte_order.upper(), 'data_writers': data_writers, 'data_writers_builtin': data_writers + 4, \
	'data_readers': data_readers, 'data_readers_builtin': data_readers + 4, 'any_subscriptions': any_subscriptions, \
	'any_publications': any_publications, 'up_name': impl_name.upper(), \
	'max_topics': len(datastructures), 'max_topics_builtin': len(datastructures) + 4, 'extended_topic_support' : extended_topic_support \
})
