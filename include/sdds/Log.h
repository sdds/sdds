/****************************************************************************
 * Copyright (C) 2017 RheinMain University of Applied Sciences              *
 *                                                                          *
 * This file is part of:                                                    *
 *      _____  _____   _____                                                *
 *     |  __ \|  __ \ / ____|                                               *
 *  ___| |  | | |  | | (___                                                 *
 * / __| |  | | |  | |\___ \                                                *
 * \__ \ |__| | |__| |____) |                                               *
 * |___/_____/|_____/|_____/                                                *
 *                                                                          *
 * This Source Code Form is subject to the terms of the Mozilla Public      *
 * License, v. 2.0. If a copy of the MPL was not distributed with this      *
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.                 *
 ****************************************************************************/

/**
 * @file      Log.h
 * @author    Kai Beckmann
 * @copyright MPL 2 
 * @see       https://github.com/sdds/sdds
 * Logger class
 */




#ifndef  LOG_H_INC
#define  LOG_H_INC

#ifdef UTILS_NO_LOGGING
#define Log_trace(args ...)
#define Log_debug(args ...)
#define Log_info(args ...)
#define Log_warn(args ...)
#define Log_error(args ...)
#define Log_PANIC(args ...)
#endif


#ifndef UTILS_NO_LOGGING
#include <stdio.h>
#include <assert.h>
#ifndef SDDS_PLATFORM_autobest
#include <stdlib.h>
#endif
#define Log_trace(args ...) _log_trace(__FUNCTION__, __LINE__, ## args);
#define Log_debug(args ...) _log_debug(__FUNCTION__, __LINE__, ## args);
#define Log_info(args ...) _log_info(__FUNCTION__, __LINE__, ## args);
#define Log_warn(args ...) _log_warn(__FUNCTION__, __LINE__, ## args);
#define Log_error(args ...) _log_error(__FUNCTION__, __LINE__, ## args);
#define Log_PANIC(args ...) _log_panic(__FUNCTION__, __LINE__, ## args);

void
_log_trace(const char* fnk, int line, ...);
void
_log_debug(const char* fnk, int line, ...);
void
_log_info(const char* fnk, int line, ...);
void
_log_warn(const char* fnk, int line, ...);
void
_log_error(const char* fnk, int line, ...);
void
_log_panic(const char* fnk, int line, ...);

void
Log_setLvl(int lvl);

#define LOG_ASSERT(cond) \
    do { if (!(cond)) {             \
             fprintf(stderr, "ASSERT FAILS in function %s in line %d \n", \
                     __FUNCTION__, __LINE__);                                                                        \
             assert(cond);                                                                                           \
             exit(-1);                                                                                                       \
         }} while(0);

#endif
#endif   /* ----- #ifndef LOG_H_INC  ----- */
