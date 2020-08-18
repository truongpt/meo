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
    char* (*f_load)(int32_t value, FILE*);
    char* (*f_out)(char* r, FILE*);
    char* (*f_var)(char* var, FILE*);
    char* (*f_print)(char* r, FILE*);
    char* (*f_add)(char* r1, char* r2, FILE*);
    char* (*f_sub)(char* r1, char* r2, FILE*);
    char* (*f_mul)(char* r1, char* r2, FILE*);
    char* (*f_div)(char* r1, char* r2, FILE*);
    char* (*f_store)(char* var, char* r, FILE*);
    char* (*f_load_var)(char* var, FILE*);
} GenFuncTable;

int32_t GenLoadX86_64(GenFuncTable *func);

#ifdef __cplusplus
}
#endif

#endif
