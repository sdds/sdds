#!/usr/bin/env python

# TODO:
# support arrays (names with [])

valid_types = [ \
	('dds_short_t', 2, 'int16'),
	('dds_long_t', 4, 'int32'),
	('dds_long_long_t', 8, 'int64'),
	('dds_unsigned_short_t', 2, 'uint16'),
	('dds_unsigned_long_t', 4, 'uint32'),
	('dds_unsigned_long_long_t', 8, 'uint64'),
	('dds_float_t', 4, None),
	('dds_double_t', 8, None),
	('dds_long_double_t', 16, None),
	('dds_char_t', 1, 'int8'),
	('dds_octet_t', 1, 'uint8'),
	('dds_boolean_t', 1, 'bool'),
	('dds_string_t', 4, None)
]

valid_typenames = []

for type_ in valid_types:
	valid_typenames.append(type_[0])

def get_header(f, name, data):
	whitespace_string = '  '

	for i in name:
		whitespace_string += ' '

	whitespace_string = '\t'

	string = r"""
#ifndef %(up_name)s_DS_H_INCLUDED
#define %(up_name)s_DS_H_INCLUDED

#include <dds/dcps.h>

#include <sdds/topic.h>

struct %(name)s_data
{
%(data)s
};

typedef struct %(name)s_data %(name)s_data_t;

#ifdef sDDS_TOPIC_HAS_PUB
dds_return_t %(name)s_data_reader_take_next_sample(
%(indent)sdds_data_reader_t *_this,
%(indent)s%(name)s_data_t **values,
%(indent)sdds_sample_info_t *sample_info
);
rc_t topic_marshalling_%(name)s_decode(byte_t *buf, data_t *data, size_t *size);
#endif

#ifdef sDDS_TOPIC_HAS_SUB
dds_return_t %(name)s_data_writer_write(
%(indent)sdds_data_writer_t *_this,
%(indent)s%(name)s_data_t *values,
%(indent)sdds_instance_handle_t const *handle
);
rc_t topic_marshalling_%(name)s_encode(byte_t *buf, data_t *data, size_t *size);
#endif

topic_t *sdds_%(name)s_topic_create(%(name)s_data_t *pool, int count);
rc_t data_%(name)s_copy(data_t *dest, data_t *source);

#endif
""" % {'name': name, 'up_name': name.upper(), 'data': data[:-1], 'indent': whitespace_string}
	f.write(string[1:])

def get_impl(f, name, marshalling_enc, marshalling_dec, sdds_domain, sdds_topic, size):
	whitespace_string = '  '

	for i in name:
		whitespace_string += ' '

	whitespace_string = '\t'

	string = r"""#ifdef __cplusplus
extern "C"
{
#endif

#include "%(name)s-ds.h"

#include <os-ssal/memory.h>

#include <sdds/data_sink.h>
#include <sdds/data_source.h>
#include <sdds/locator_db.h>
#include <sdds/marshalling.h>
#include <sdds/network.h>
#include <sdds/topic_db.h>
#include <sdds/topic_marshalling.h>

#include <stdio.h>
#include <string.h>

#ifdef __cplusplus
}
#endif

#define sDDS_DOMAIN %(sdds_domain)s
#define sDDS_TOPIC %(sdds_topic)s

#ifdef sDDS_TOPIC_HAS_PUB
dds_return_t %(name)s_data_reader_take_next_sample(
%(indent)sdds_data_reader_t *_this,
%(indent)s%(name)s_data_t **values,
%(indent)sdds_sample_info_t *sample_info
)
{
	rc_t ret = data_sink_take_next_sample(_this, (data_t **)values, sample_info);

	if (ret == SDDS_RT_NODATA)
		return DDS_RETCODE_NO_DATA;

	if (ret == SDDS_RT_OK)
		return DDS_RETCODE_OK;

	return DDS_RETCODE_ERROR;
}
#endif

#ifdef sDDS_TOPIC_HAS_SUB
dds_return_t %(name)s_data_writer_write(
%(indent)sdds_data_writer_t *_this,
%(indent)s%(name)s_data_t *values,
%(indent)sdds_instance_handle_t const *handle
)
{
	rc_t ret = data_source_write(_this, values, handle);

	if (ret == SDDS_RT_OK)
		return DDS_RETCODE_OK;

	return DDS_RETCODE_ERROR;
}
#endif

topic_t *sdds_%(name)s_topic_create(%(name)s_data_t *pool, int count)
{
	topic_t *topic = topic_db_create_topic();
	locator_t *locator;

	network_create_locator(&locator);

	for (int i = 0; i < count; i++)
		message_init(topic->messages + i, pool + i);

#ifdef sDDS_TOPIC_HAS_SUB
	topic->data_encode = topic_marshalling_%(name)s_encode;
	topic->data_sinks.list = locator;
#endif

#ifdef sDDS_TOPIC_HAS_PUB
	topic->data_decode = topic_marshalling_%(name)s_decode;
#endif

	topic->domain_id = sDDS_DOMAIN;
	topic->id = sDDS_TOPIC;
	topic->data_copy = data_%(name)s_copy;

	return topic;
}

rc_t data_%(name)s_copy(data_t *dest, data_t *source)
{
	memcpy(dest, source, sizeof(%(name)s_data_t));

	return SDDS_RT_OK;
}

rc_t topic_marshalling_%(name)s_encode(byte_t *buffer, data_t *data, size_t *size)
{
	*size = 0;

	%(name)s_data_t *real_data = (%(name)s_data_t *)data;

%(marshalling_enc)s

	return SDDS_RT_OK;
}

rc_t topic_marshalling_%(name)s_decode(byte_t *buffer, data_t *data, size_t *size)
{
	if (*size != %(size)s)
		fprintf(stderr, "size mismatch\n");

	*size = 0;

	%(name)s_data_t *real_data = (%(name)s_data_t *)data;

%(marshalling_dec)s

	return SDDS_RT_OK;
}
""" % \
{ \
	'name': name, 'marshalling_enc': marshalling_enc, 'marshalling_dec': marshalling_dec, \
	'sdds_domain': sdds_domain, 'sdds_topic': sdds_topic, 'size': size, \
	'indent': whitespace_string \
}
	f.write(string)

def generate_marshalling(encdec, data):
	string = ''

	type__ = None

	for type_ in valid_types:
		if data[0] == type_[0]:
			type__ = type_
			break


	assert type__

	if not type__[2]:
		print 'no decoder or encoder for %s' % type__[0]
		sys.exit(1)

	string = r"""
	marshalling_%(encdec)s_%(decoder)s(buffer + *size, &real_data->%(varname)s);
	*size += sizeof real_data->%(varname)s;

""" % {'decoder': type__[2], 'encdec': encdec, 'varname': data[1]}

	return string[1:]

def make_data(data, datastructure, datastructures, finalize = True):
	import copy

	empty_datastructure = {
		'name': None,
		'domain': None,
		'topic': None,
		'data': [],
		'struct_data': '',
		'marshalling': {'enc': '', 'dec': ''},
		'size': 0
	}

	empty_data = {
		'type': None,
		'variable': None,
		'comments': []
	}

	if data:
		if data['type'] and data['variable']:
			datastructure['data'].append(data)
			data = copy.deepcopy(empty_data)
	else:
		data = copy.deepcopy(empty_data)

	if not datastructure:
		datastructure = copy.deepcopy(empty_datastructure)
	elif finalize:
		if datastructure['name'] and \
		   datastructure['domain'] and \
		   datastructure['topic'] and \
		   datastructure['data']:
			for data in datastructure['data']:
				for comment in data['comments']:
					datastructure['struct_data'] += '	%s\n' % comment

				data['comments'] = []

				for type_ in valid_types:
					if type_[0] == data['type']:
						datastructure['size'] += type_[1]

				datastructure['struct_data'] += '	%s %s;\n' % (data['type'], data['variable'])
				marshalling_data = [ data['type'], data['variable'] ]

				datastructure['marshalling']['enc'] += generate_marshalling('enc', marshalling_data)
				datastructure['marshalling']['dec'] += generate_marshalling('dec', marshalling_data)

			datastructure['marshalling']['enc'] = datastructure['marshalling']['enc'][:-2]
			datastructure['marshalling']['dec'] = datastructure['marshalling']['dec'][:-2]

			datastructures.append(datastructure)
			datastructure = copy.deepcopy(empty_datastructure)

	return (data, datastructure)

def generate_datastructures(filename):
	if not filename:
		filename = 'datastructures'

	f = open(filename, 'r')

	datastructures = []
	(data, datastructure) = make_data(None, None, datastructures)

	in_comment = False

	for line in f:
		if line[0] == '[' and line[:-1]:
			possible_name = line[1:-2]

			(data, datastructure) = make_data(data, datastructure, datastructures)

			if datastructure['name']:
				print '[%s] invalid, name already parsed ([%s])' % (possible_name, datastructure['name'])
				sys.exit(1)

			datastructure['name'] = possible_name
			continue

		if line.startswith('domain:'):
			possible_domain = line[len('domain:'):-1].strip()

			if not datastructure['name']:
				print 'domain %s invalid, because no name yet' % possible_domain

			datastructure['domain'] = hex(int(possible_domain, 16))
			continue

		if line.startswith('topic:'):
			possible_topic = line[len('topic:'):-1].strip()

			if not datastructure['name']:
				print 'topic %s invalid, because no name yet' % possible_topic

			datastructure['topic'] = hex(int(possible_topic, 16))
			continue

		stripped_line = line.strip()

		if not len(stripped_line) == 0:
			if stripped_line[0:2] == '/*':
				data['comments'].append(line[:-1])

				if stripped_line[-2:] != '*/':
					in_comment = True

				continue

			if stripped_line[0:2] == '//':
				data['comments'].append(line[:-1])
				continue

			if in_comment:
				data['comments'].append(line[:-1])

				if stripped_line[-2:] == '*/':
					in_comment = False

				continue

			if len(stripped_line.split(' ')) != 2:
				print '`%s` is an invalid type - variable combination' % stripped_line
				sys.exit(1)

			(data['type'], data['variable']) = stripped_line.split(' ')

			if data['type'] not in valid_typenames:
				print "%s is not a valid typename" % data['type']
				print "here's a list with valid types:"

				for typename in valid_typenames:
					print typename

				sys.exit(1)

			array_begin = data['variable'].find('[')
			array_end = data['variable'].find(']')

			if array_begin != -1 and array_end != -1 and array_end > array_begin:
				print "%s is an array and not supported yet" % data['variable']
				sys.exit(1)

			(data, datastructure) = make_data(data, datastructure, datastructures, False)

	(data, datastructure) = make_data(data, datastructure, datastructures)

	return datastructures


if __name__ == "__main__":
	import sys

	filename = None

	if len(sys.argv) >= 2:
		filename = sys.argv[1]

	datastructures = generate_datastructures(filename)

	for datastructure in datastructures:
		size = datastructure['size']
		name = datastructure['name']
		domain_id = datastructure['domain']
		topic_id = datastructure['topic']
		marshalling_enc = datastructure['marshalling']['enc']
		marshalling_dec = datastructure['marshalling']['dec']
		struct_data = datastructure['struct_data']

		f = open('%s-ds.h' % name, 'w')
		get_header(f, name, struct_data)

		f = open('%s-ds.c' % name, 'w')
		get_impl(f, name, marshalling_enc, marshalling_dec, domain_id, topic_id, size)
