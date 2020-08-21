/*
 * Copyright (c) 2020/08 gravieb <truongptk30a3@gmail.com>
 * This file is released under the GPLv3
 */
#include <stdio.h>
#include <stdarg.h>

#ifndef _LOG_H_
#define _LOG_H_

#ifdef __cplusplus
extern "C" {
#endif

enum LogLevel {
    TRACE,
    DEBUG,
    INFO,
    WARN,
    ERROR,
    CLGT
};

void set_log_level(int level);
void mlog(int level, char* format, ...);
char* tok2str(int tok);


#ifdef __cplusplus
}
#endif

#endif
