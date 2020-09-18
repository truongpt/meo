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
    char* (*f_free)(char* r, FILE*);
    char* (*f_out)(char* r, FILE*);
    char* (*f_var)(char* var, FILE*);
    char* (*f_l_var)(char* var, int size, FILE*);

    char* (*f_add)(char* r1, char* r2, FILE*);
    char* (*f_sub)(char* r1, char* r2, FILE*);
    char* (*f_mul)(char* r1, char* r2, FILE*);
    char* (*f_div)(char* r1, char* r2, FILE*);
    char* (*f_lt)(char* r1, char* r2, FILE*);
    char* (*f_le)(char* r1, char* r2, FILE*);
    char* (*f_gt)(char* r1, char* r2, FILE*);
    char* (*f_ge)(char* r1, char* r2, FILE*);
    char* (*f_eq)(char* r1, char* r2, FILE*);
    char* (*f_ne)(char* r1, char* r2, FILE*);

    char* (*f_or)(char* r1, char* r2, FILE*);
    char* (*f_and)(char* r1, char* r2, FILE*);
    char* (*f_b_or)(char* r1, char* r2, FILE*);
    char* (*f_b_xor)(char* r1, char* r2, FILE*);
    char* (*f_b_and)(char* r1, char* r2, FILE*);

    char* (*f_lt_j)(char* r1, char* r2, char* label, FILE*);
    char* (*f_le_j)(char* r1, char* r2, char* label, FILE*);
    char* (*f_gt_j)(char* r1, char* r2, char* label, FILE*);
    char* (*f_ge_j)(char* r1, char* r2, char* label, FILE*);
    char* (*f_eq_j)(char* r1, char* r2, char* label, FILE*);
    char* (*f_ne_j)(char* r1, char* r2, char* label, FILE*);
    char* (*f_zero_j)(char* r, char* label, FILE*);
    char* (*f_jump)(char* label, FILE*);
    char* (*f_label)(char* label, FILE*);
    char* (*f_str_label)(char* label, char* str, FILE*);

    char* (*f_func)(char* name, int stack_size, FILE*);
    char* (*f_func_arg)(int arg_order, FILE*);
    char* (*f_func_exit)(char* exit_label, int stack_size, FILE*);
    char* (*f_func_call)(char* name, FILE*);
    char* (*f_arg)(char* arg, int idx, FILE*);
    char* (*f_reg_backup)(FILE*);
    char* (*f_reg_restore)(FILE*);

    char* (*f_store)(char* var, char* r, FILE*);
    char* (*f_load_var)(char* var, FILE*);
    char* (*f_return)(char* r, char* exit_label, FILE*);
} GenFuncTable;

int32_t GenLoadX86_64(GenFuncTable *func);

#ifdef __cplusplus
}
#endif

#endif
