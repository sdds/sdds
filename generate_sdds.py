#!/usr/bin/env python

decl_headers = r"""
#include <dds/dcps.h>
"""

impl_headers = r"""
#include <os-ssal/memory.h>
#include <sdds/data_sink.h>
#include <sdds/data_source.h>
#include <sdds/locator_db.h>
#include <sdds/log.h>
#include <sdds/network.h>
"""

impl_decl = r"""
int init_sdds(int log_level);
"""

impl_init = r"""
int init_sdds(int log_level)
{
	memory_init();
	network_init();
	locator_db_init();
	data_source_init();
	data_sink_init();

	log_set_level(log_level);

%s

	return 0;
}
"""

def get_decl(f, impl_name, datastructures):
	f.write(r"""
#ifndef %(up_name)s_SDDS_IMPL_H_INCLUDED
#define %(up_name)s_SDDS_IMPL_H_INCLUDED

"""[1:] % {'up_name': impl_name.upper()})

	for ds in datastructures:
		f.write('#include "%s-ds.h"\n' % ds['name'])

	f.write(decl_headers + '\n')

	for ds in datastructures:
		f.write(r"""
extern dds_data_reader_t *g_%(name)s_reader;
extern dds_data_writer_t *g_%(name)s_writer;
extern dds_topic_t *g_%(name)s_topic;
extern %(name)s_data_t g_%(name)s_pool[sDDS_TOPIC_MSG_COUNT];
"""[1:] % {'name': ds['name']})

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

	def_string = ''

	for ds in datastructures:
		if ds['subscriber'] or ds['publisher']:
			def_string += 'dds_topic_t *g_%s_topic;\n' % ds['name']
			def_string += '%(name)s_data_t g_%(name)s_pool[sDDS_TOPIC_MSG_COUNT];\n' % {'name': ds['name']}

		if ds['subscriber']:
			def_string += 'dds_data_reader_t *g_%s_reader;\n' % ds['name']

		if ds['publisher']:
			def_string += 'dds_data_writer_t *g_%s_writer;\n' % ds['name']
	
		def_string += '\n'

	f.write(def_string)

	impl_string = ''

	for ds in datastructures:
		if ds['subscriber'] or ds['publisher']:
			impl_string += '\tg_%(name)s_topic = sdds_%(name)s_topic_create(g_%(name)s_pool, sDDS_TOPIC_MSG_COUNT);\n' % {'name': ds['name']}

		if ds['subscriber']:
			impl_string += '\tg_%(name)s_reader = data_sink_create_data_reader(g_%(name)s_topic, NULL, NULL, NULL);\n' % {'name': ds['name']}

		if ds['publisher']:
			impl_string += '\tg_%(name)s_writer = data_source_create_data_writer(g_%(name)s_topic, NULL, NULL, NULL);\n' % {'name': ds['name']}

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
			possible_name = line[1:-2]

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

		if line.strip() == 'publisher':
			if not role['name']:
				print 'can\'t publish, because no name yet'
				sys.exit(1)
			token = 'publisher'
		elif line.strip() == 'subscriber':
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

#define sDDS_LOCAL_BYTEORDER_%(byte_order)s_ENDIAN
#define sDDS_NET_VERSION 0x01
#define sDDS_MAX_DATA_WRITERS %(data_writers)s
#define sDDS_MAX_DATA_READERS %(data_readers)s
#define sDDS_NET_MAX_OUT_QUEUE 2
#define sDDS_NET_MAX_BUF_SIZE 30
#define sDDS_NET_MAX_LOCATOR_COUNT 10
#define sDDS_QOS_DATA_WRITER_1_LAT_BUD 100
#define sDDS_QOS_DATA_WRITER_2_LAT_BUD 500
%(any_subscriptions)s
%(any_publications)s
#define sDDS_TOPIC_MSG_COUNT 5
#define sDDS_TOPIC_MAX_COUNT %(max_topics)d
#define sDDS_MNG_WORKER_CYCLE_TIME 10000
#define sDDS_MNG_BUILTINT_PUBCYCLE_PRESCALER 2

#endif
"""[1:] % \
{ \
	'byte_order': byte_order.upper(), 'data_writers': data_writers, \
	'data_readers': data_readers, 'any_subscriptions': any_subscriptions, \
	'any_publications': any_publications, 'up_name': impl_name.upper(), \
	'max_topics': len(datastructures) \
})
