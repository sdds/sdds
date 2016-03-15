/*
 * =====================================================================================
 *
 *       Filename:  sdds_sdds_types.h
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  19.02.2010 12:55:38
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Kai Beckmann (kai), kai-oliver.beckmann@hs-rm.de
 *        Company:  Hochschule RheinMain - DOPSY Labor für verteilte Systeme
 *
 * =====================================================================================
 */
#ifndef  SDDS_TYPES_H_INC
#define  SDDS_TYPES_H_INC

#define  SDDS_STRING_BYTES 10

#ifdef _ECLIPSE_DEV_
#include "CONSTANTS.h"
#endif // _ECLIPSE_DEV

#ifdef SDDS_ARCH_esp
#define SDDS_SNPS_ADDR_STR_LENGTH 45
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stddef.h>
typedef bool bool_t;
typedef uint8_t byte_t;
typedef uint8_t char_t;
typedef float float32_t;
typedef double float64_t;
#endif // #ifdef SDDS_ARCH_esp

#ifdef SDDS_ARCH_atmega
#define SDDS_SNPS_ADDR_STR_LENGTH 45
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stddef.h>
#include <assert.h>
typedef bool bool_t;
typedef uint8_t byte_t;
typedef uint8_t char_t;
typedef float float32_t;
typedef double float64_t;
#endif // #ifdef SDDS_ARCH_ATmega

#ifdef SDDS_ARCH_CC2430
#define SDDS_SNPS_ADDR_STR_LENGTH 45
#include "hal_sdds_types.h"
#include "sysmac.h"

typedef int8 int8_t;
typedef int16 int16_t;
typedef int32 int32_t;
typedef long int int64_t;

typedef uint8 uint8_t;
typedef uint16 uint16_t;
typedef uint32 uint32_t;
typedef unsigned long int uint64_t;

typedef bool bool_t;
typedef uint8 byte_t;
typedef uint8 char_t;

typedef float float32_t;
typedef double float64_t;
#endif /* SDDS_ARCH_CC2430 */

#ifdef SDDS_ARCH_x86
#include <netdb.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <inttypes.h>
#include <string.h>
#include <assert.h>
#include <signal.h>
#include <time.h>

#define SDDS_SNPS_ADDR_STR_LENGTH NI_MAXHOST
/*#ifndef __int8_t_defined
  #define __int8_t_defined
  typedef char                  int8_t;
  typedef short int     int16_t;
  typedef int                   int32_t;
  typedef long int              int64_t;
  #endif*/
#ifndef  IPV6_JOIN_GROUP      /* APIv0 compatibility */
#define  IPV6_JOIN_GROUP      IPV6_ADD_MEMBERSHIP
#endif

typedef unsigned char bool_t;
typedef char char_t;
typedef unsigned char byte_t;
typedef float float32_t;
typedef double float64_t;
typedef long double float128_t;

/*#ifndef __size_t
  #define __size_t__
  typedef unsigned int  size_t;
  #endif*/
#endif   /* TYPE_DEFINES_x86 */

#ifndef NULL
#define NULL    ((void*)0)
#endif

#ifdef SDDS_ARCH_ARM
#define SDDS_SNPS_ADDR_STR_LENGTH 45
#include <stddef.h>
#include <stdint.h>
#include <assert.h>
#ifdef SDDS_PLATFORM_autobest
typedef uint8_t bool;
#else
#include <stdbool.h>
#endif
#ifndef __int8_t_defined
#define __int8_t_defined
typedef short int int16_t;
typedef int int32_t;
#endif
//#define AF_INET 1
//#define AF_INET6 2

typedef unsigned char bool_t;           /*  */
typedef char char_t;
typedef unsigned char byte_t;
typedef float float32_t;
typedef double float64_t;
typedef long double float128_t;
#define true 1
#define false 0
#ifndef __size_t
#define __size_t__
typedef unsigned int size_t;
#endif
#endif   /* TYPE_DEFINES_ARM */

#ifndef NULL
#define NULL    ((void*)0)
#endif

typedef uint32_t SDDS_timestamp_t;
typedef uint16_t SDDS_msec_t;
typedef uint16_t SDDS_usec_t;

typedef uint8_t domainid_t;
#define SDDS_DOMAIN_NIL 0
#define SDDS_DOMAIN_DEFAULT 1

#ifdef SDDS_EXTENDED_TOPIC_SUPPORT
typedef uint16_t topicid_t;
#else
typedef uint8_t topicid_t;
#endif

typedef uint8_t version_t;

typedef uint8_t rc_t;
#define SDDS_RT_OK 0
#define SDDS_RT_FAIL 1
#define SDDS_RT_NOMEM 2
#define SDDS_RT_NODATA 3
#define SDDS_RT_BAD_PARAMETER 4
#define SDDS_RT_KNOWN 5
#define SDDS_RT_DEFERRED 6
#define SDDS_RT_NO_SUB 7

//  Abstract definition for Data
struct Data_t;
typedef struct Data_t* Data;
struct DataInfo_t;
typedef struct DataInfo_t* DataInfo;

typedef void* Qos;
typedef void* Listener;
typedef uint8_t* StatusMask;

#ifndef NI_MAXHOST
#define NI_MAXHOST 45
#endif

#endif   /* ----- #ifndef TYPES_H_INC  ----- */
