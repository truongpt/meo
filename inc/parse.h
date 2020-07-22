/*
 * Copyright (c) 2020/07 gravieb <truongptk30a3@gmail.com>
 * This file is released under the GPLv3
 */

#ifndef _PARSE_H_
#define _PARSE_H_

#include <stdio.h>
#include "meo.h"

#ifdef __cplusplus
extern "C" {
#endif

int32_t ParseCreate(void);
int32_t ParseDestroy(void);
int32_t ParseOpen(void** prm, void* scan_prm);
int32_t ParseClose(void* prm);
int32_t ParseProc(void* prm);

#ifdef __cplusplus
}
#endif

#endif
