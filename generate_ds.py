#!/usr/bin/env python

# TODO:
# support arrays (names with [])

valid_types = [ \
	('DDS_short', 2, 'int16'),
	('DDS_long', 4, 'int32'),
	('DDS_long_long', 8, 'int64'),
	('DDS_unsigned_short', 2, 'uint16'),
	('DDS_unsigned_long', 4, 'uint32'),
	('DDS_unsigned_long_long', 8, 'uint64'),
	('DDS_float', 4, None),
	('DDS_double', 8, None),
	('DDS_long_double', 16, None),
	('DDS_char', 1, 'int8'),
	('DDS_octet', 1, 'uint8'),
	('DDS_boolean', 1, 'bool'),
	('DDS_string', 4, None)
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
#ifndef %(up_name)s_H_INC
#define %(up_name)s_H_INC

#include <dds/DDS_DCPS.h>

#include <sdds/Topic.h>

struct %(cap_name)s
{
%(data)s
};

typedef struct %(cap_name)s %(cap_name)s;

#ifdef sDDS_TOPIC_HAS_PUB
DDS_ReturnCode_t DDS_%(cap_name)sDataReader_take_next_sample(
%(indent)sDDS_DataReader _this,
%(indent)s%(cap_name)s** values,
%(indent)sDDS_SampleInfo* sample_info
);

#endif

#ifdef sDDS_TOPIC_HAS_SUB
DDS_ReturnCode_t DDS_%(cap_name)sDataWriter_write(
%(indent)sDDS_DataWriter _this,
%(indent)sconst %(cap_name)s* instance_data,
%(indent)sconst DDS_InstanceHandle_t handle
);
#endif

Topic sDDS_%(cap_name)sTopic_create(%(cap_name)s* pool, int count);

#endif
""" % {'name': name, 'up_name': name.upper(), 'data': data[:-1], 'indent': whitespace_string, 'cap_name': name[0].capitalize() + name[1:]}
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

#include <os-ssal/Memory.h>

#include <sdds/DataSink.h>
#include <sdds/DataSource.h>
#include <sdds/LocatorDB.h>
#include <sdds/Marshalling.h>
#include <sdds/Network.h>
#include <sdds/TopicDB.h>
#include <sdds/TopicMarshalling.h>

#include <stdio.h>
#include <string.h>

#ifdef __cplusplus
}
#endif

#define sDDS_DOMAIN %(sdds_domain)s
#define sDDS_TOPIC %(sdds_topic)s


rc_t TopicMarshalling_%(cap_name)s_cpy(Data dest, Data source);

#ifdef sDDS_TOPIC_HAS_PUB

rc_t TopicMarshalling_%(cap_name)s_decode(byte_t* buffer, Data data, size_t* size);

DDS_ReturnCode_t DDS_%(cap_name)sDataReader_take_next_sample(
%(indent)sDDS_DataReader _this,
%(indent)s%(cap_name)s** data_values,
%(indent)sDDS_SampleInfo* sample_info
)
{
	rc_t ret = DataSink_take_next_sample((DataReader) _this, (Data*) data_values, (DataInfo) sample_info);

	if (ret == SDDS_RT_NODATA)
		return DDS_RETCODE_NO_DATA;

	if (ret == SDDS_RT_OK)
		return DDS_RETCODE_OK;

	return DDS_RETCODE_ERROR;
}

DDS_ReturnCode_t DDS_DataReader_set_listener(
%(indent)sDDS_DataReader _this,
%(indent)sconst struct DDS_DataReaderListener* a_listener,
%(indent)sconst DDS_StatusMask mask
)
{
	rc_t ret = DataSink_set_on_data_avail_listener((DataReader) _this, (On_Data_Avail_Listener) a_listener->on_data_available, (const StatusMask) mask);
	if (ret == SDDS_RT_OK)
		return DDS_RETCODE_OK;

	return DDS_RETCODE_ERROR;
}
#endif

#ifdef sDDS_TOPIC_HAS_SUB

rc_t TopicMarshalling_%(cap_name)s_encode(byte_t* buffer, Data data, size_t* size);

DDS_ReturnCode_t DDS_%(cap_name)sDataWriter_write(
%(indent)sDDS_DataWriter _this,
%(indent)sconst %(cap_name)s* instance_data,
%(indent)sconst DDS_InstanceHandle_t  handle
)
{
	rc_t ret = DataSource_write((DataWriter) _this, (Data)instance_data, (void*) handle);

	if (ret == SDDS_RT_OK)
		return DDS_RETCODE_OK;

	return DDS_RETCODE_ERROR;
}
#endif

Topic sDDS_%(cap_name)sTopic_create(%(cap_name)s* pool, int count)
{
	Topic topic = TopicDB_createTopic();
	//Locator locator;

	//Network_createLocator(&locator);

	for (int i = 0; i < count; i++)
		Msg_init(&(topic->msg.pool[i]), (Data) &(pool[i]));

#ifdef sDDS_TOPIC_HAS_SUB
	topic->Data_encode = TopicMarshalling_%(cap_name)s_encode;
	//topic->dsinks.list = locator;
#endif

#ifdef sDDS_TOPIC_HAS_PUB
	topic->Data_decode = TopicMarshalling_%(cap_name)s_decode;
#endif

	topic->domain = sDDS_DOMAIN;
	topic->id = sDDS_TOPIC;
	topic->Data_cpy = TopicMarshalling_%(cap_name)s_cpy;

	return topic;
}

rc_t TopicMarshalling_%(cap_name)s_cpy(Data dest, Data source)
{
	memcpy(dest, source, sizeof(%(cap_name)s));

	return SDDS_RT_OK;
}

rc_t TopicMarshalling_%(cap_name)s_encode(byte_t* buffer, Data data, size_t* size)
{
	*size = 0;

	%(cap_name)s* real_data = (%(cap_name)s*) data;

%(marshalling_enc)s


	return SDDS_RT_OK;
}

rc_t TopicMarshalling_%(cap_name)s_decode(byte_t* buffer, Data data, size_t* size)
{
	if (*size != %(size)s)
		fprintf(stderr, "size mismatch\n");

	*size = 0;

	%(cap_name)s* real_data = (%(cap_name)s*) data;

%(marshalling_dec)s

	return SDDS_RT_OK;
}
""" % \
{ \
	'name': name, 'marshalling_enc': marshalling_enc, 'marshalling_dec': marshalling_dec, \
	'sdds_domain': sdds_domain, 'sdds_topic': sdds_topic, 'size': size, \
	'indent': whitespace_string, 'cap_name': name[0].capitalize() + name[1:] \
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
	Marshalling_%(encdec)s_%(decoder)s(buffer + *size, &real_data->%(varname)s);
	*size += sizeof(real_data->%(varname)s);

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
