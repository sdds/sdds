/*  =========================================================================
    Log - AutoBest logging implementation

    Copyright (c) the Contributors as noted in the AUTHORS file.

    This file is part of sDDS:
    http://wwwvs.cs.hs-rm.de.
    =========================================================================
 */

#include "Log.h"
#include <stdio.h>
#include <stdarg.h>
#include <api_calls.h>

// 0 all, 1
static int logLvl =0;

void
Log_setLvl(int lvl) {
    logLvl = lvl;
}
#ifndef UTILS_NO_LOGGING
void
_log_trace(const char* fnk, int line, ...) {
    if (logLvl < 1 ) {
        va_list arg;
        char* string;
        va_start(arg, line);

        string = va_arg(arg, char*);
        printf("T: %s()[%d]: ", fnk, line);
        vprintf(string, arg);
        va_end(arg);
    }
}

void
_log_debug(const char* fnk, int line, ...) {
    if (logLvl < 2 ) {
        va_list arg;
        char* string;
        va_start(arg, line);

        string = va_arg(arg, char*);
        printf("D: %s()[%d]: ", fnk, line);
        vprintf(string, arg);
        va_end(arg);
    }
}

void
_log_info(const char* fnk, int line, ...) {
    if (logLvl < 3) {
        va_list arg;
        char* string;
        va_start(arg, line);

        string = va_arg(arg, char*);
        printf("I: %s()[%d]: ", fnk, line);
        vprintf(string, arg);

        va_end(arg);
    }
}

void
_log_warn(const char* fnk, int line, ...) {
    if (logLvl < 4) {
        va_list arg;
        char* string;
        va_start(arg, line);

        string = va_arg(arg, char*);
        printf("W: %s()[%d]: ", fnk, line);
        vprintf(string, arg);
        va_end(arg);
    }
}

void
_log_error(const char* fnk, int line, ...) {
    if (logLvl < 5) {
        va_list arg;
        char* string;
        va_start(arg, line);

        string = va_arg(arg, char*);
        printf("E: %s()[%d]: ", fnk, line);
        vprintf(string, arg);
        va_end(arg);
    }
}

void
_log_panic(const char* fnk, int line, ...) {
    if (logLvl < 6) {
        va_list arg;
        char* string;
        va_start(arg, line);

        string = va_arg(arg, char*);
        printf("P: %s()[%d]: ", fnk, line);
        vprintf(string, arg);
        va_end(arg);
        sys_abort();
    }
}
#endif
