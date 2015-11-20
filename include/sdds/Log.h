/*
 * =====================================================================================
 *
 *       Filename:  Log.h
 *
 *    Description:  Logger class
 *
 *        Version:  1.0
 *        Created:  22.02.2010 16:19:18
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Kai Beckmann (kai), kai-oliver.beckmann@hs-rm.de
 *        Company:  Hochschule RheinMain - DOPSY Labor für verteilte Systeme
 *
 * =====================================================================================
 */


#ifndef  LOG_H_INC
#define  LOG_H_INC

#ifdef UTILS_NO_LOGGING
#define Log_debug(args...)
#define Log_info(args...)
#define Log_warn(args...)
#define Log_error(args...)
#define Log_PANIC(args...)
#endif


#ifndef UTILS_NO_LOGGING
#include <stdio.h>
#include <assert.h>
#ifndef SDDS_PLATFORM_autobest
#include <stdlib.h>
#endif
#define Log_debug(args...) _log_debug(__FUNCTION__,##args);
#define Log_info(args...) _log_info(__FUNCTION__,##args);
#define Log_warn(args...) _log_warn(__FUNCTION__,##args);
#define Log_error(args...) _log_error(__FUNCTION__,##args);
#define Log_PANIC(args...) _log_panic(__FUNCTION__,##args);

void _log_debug(const char* fnk, ...);
void _log_info(const char* fnk, ...);
void _log_warn(const char* fnk, ...);
void _log_error(const char* fnk, ...);
void _log_panic(const char* fnk, ...);

void Log_setLvl(int lvl);

#define LOG_ASSERT(cond)\
           do { if (!(cond)) {             \
              fprintf(stderr, "ASSERT FAILS in function %s in line %d \n", \
              __FUNCTION__, __LINE__);                                                                        \
              assert(cond);                                                                                           \
              exit(-1);                                                                                                       \
           }}while(0);

#endif
#endif   /* ----- #ifndef LOG_H_INC  ----- */
