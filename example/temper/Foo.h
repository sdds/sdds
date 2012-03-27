/*
 * =====================================================================================
 *
 *       Filename:  Foo.h
 *
 *    Description:  Impl Header file for Datatype Foo
 *
 *        Version:  1.0
 *        Created:  22.02.2010 15:26:42
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Kai Beckmann (kai), kai-oliver.beckmann@hs-rm.de
 *        Company:  Hochschule RheinMain - DOPSY Labor für verteilte Systeme
 *
 * =====================================================================================
 */


#ifndef  FOO_H_INC
#define  FOO_H_INC

#include "dds/DDS_DCPS.h"
#include "sdds/sdds_types.h"

typedef DDS_DataWriter  DDS_FooDataWriter;
typedef DDS_DataReader	DDS_FooDataReader;

extern DDS_FooDataWriter fooDataWriter;
extern DDS_Topic fooTopic;

extern DDS_FooDataReader fooDataReader;

#define FooTypeSupport DDS_TypeSupport

struct Foo{
    DDS_long temperatur;
    DDS_unsigned_short id;
};

typedef struct Foo Foo;

DDS_ReturnCode_t DDS_FooDataWriter_write(
    DDS_DataWriter _this,
    const Foo* instance_data,
    const DDS_InstanceHandle_t handle);

DDS_ReturnCode_t DDS_FooDataReader_take_next_sample (
    DDS_DataReader _this,
    Foo** data_values,
    DDS_SampleInfo *sample_info);
#endif   /* ----- #ifndef FOO_H_INC  ----- */
