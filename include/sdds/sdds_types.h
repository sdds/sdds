
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

#ifdef SDDS_ARCH_ATmega
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stddef.h>
typedef bool                    bool_t;
typedef uint8_t                 byte_t;
typedef uint8_t                 char_t;
typedef float                   float32_t;
typedef double                  float64_t;
#endif // #ifdef SDDS_ARCH_ATmega

#ifdef SDDS_ARCH_CC2430
#include "hal_sdds_types.h"
#include "sysmac.h"

typedef int8                    int8_t;
typedef int16                   int16_t;
typedef int32                   int32_t;
typedef long int                int64_t;

typedef uint8                   uint8_t;
typedef uint16                  uint16_t;
typedef uint32                  uint32_t;
typedef unsigned long int       uint64_t;

typedef bool                    bool_t;
typedef uint8                   byte_t;
typedef uint8                   char_t;

typedef float                   float32_t;
typedef double                  float64_t;

//typedef uint32_t                size_t;
#endif /* SDDS_ARCH_CC2430 */



#ifdef SDDS_ARCH_x86
#ifndef __int8_t_defined
#define __int8_t_defined
typedef	char 			int8_t;
typedef	short int 		int16_t;
typedef	int 			int32_t;
typedef long int 		int64_t;
#endif

typedef	unsigned char		uint8_t;		/* 8 bit unsigned  */
typedef	unsigned short int	uint16_t;	/* 16 bit ungned  */
typedef unsigned int		uint32_t;	/*  */
typedef unsigned long 		uint64_t;	/*  */
typedef unsigned char		bool_t;		/*  */
typedef char			char_t;
typedef unsigned char		byte_t;
typedef float			float32_t;
typedef double			float64_t;
typedef long double		float128_t;
#ifndef __size_t
#define __size_t__
typedef unsigned int 		size_t;
#endif
#endif   /* TYPE_DEFINES_x86 */

#ifndef NULL
#define NULL    ((void *)0)
#endif
#define false 0
#define true 1

typedef uint32_t SDDS_timestamp_t ;
typedef uint16_t SDDS_msec_t ;
typedef uint16_t SDDS_usec_t ;

typedef uint8_t domainid_t;
#define SDDS_DOMAIN_NIL 0
#define SDDS_DOMAIN_DEFAULT 1

typedef uint8_t topicid_t;
typedef uint8_t version_t;

typedef uint8_t rc_t;
#define SDDS_RT_OK 0
#define SDDS_RT_FAIL 1
#define SDDS_RT_NOMEM 2
#define SDDS_RT_NODATA 3


//abstract definistion for Data
struct Data_t;
typedef struct Data_t* Data;
struct DataInfo_t;
typedef struct DataInfo_t* DataInfo;

typedef void* Qos;
typedef void* Listener;
typedef uint8_t* StatusMask;

#endif   /* ----- #ifndef TYPES_H_INC  ----- */
