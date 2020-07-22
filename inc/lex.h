/*
 * Copyright (c) 2020/07 gravieb <truongptk30a3@gmail.com>
 * This file is released under the GPLv3
 */

#ifndef _LEX_H_
#define _LEX_H_

#include <stdio.h>
#include <stdint.h>
#include "meo.h"

#ifdef __cplusplus
extern "C" {
#endif

int32_t LexCreate(void);
int32_t LexDestroy(void);
int32_t LexOpen(void** lex_prm, const char* file);
int32_t LexClose(void* lex_prm);
int32_t LexProc(void* lex_prm, Token *t);
int32_t LexGetLine(void* lex_prm);

#ifdef __cplusplus
}
#endif

#endif
