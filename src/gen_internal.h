/*
 * Copyright (c) 2020/07 gravieb <truongptk30a3@gmail.com>
 * This file is released under the GPLv3
 */
#include <stdio.h>
#include <stdlib.h>

#ifndef _GEN_ASM_INTERNAL_H_
#define _GEN_ASM_INTERNAL_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef struct GenFuncTable {
    int32_t (*f_load)(int32_t value, FILE*);
    int32_t (*f_add)(int32_t r1, int32_t r2, FILE*);
    int32_t (*f_sub)(int32_t r1, int32_t r2, FILE*);
    int32_t (*f_mul)(int32_t r1, int32_t r2, FILE*);
    int32_t (*f_div)(int32_t r1, int32_t r2, FILE*);
} GenFuncTable;

int32_t GenLoadX86_64(GenFuncTable *func);

#ifdef __cplusplus
}
#endif

#endif
