/*
 * Copyright (c) 2020/07 gravieb <truongptk30a3@gmail.com>
 * This file is released under the GPLv3
 */

#include <stdio.h>
#include <stdlib.h>
#include "meo.h"

#ifndef _GEN_ASM_H_
#define _GEN_ASM_H_

#ifdef __cplusplus
extern "C" {
#endif

enum GenArch {
    GenX86_64,
    GenArm
};

int32_t GenCreate(void);
int32_t GenDestroy(void);
int32_t GenOpen(void** gen_prm, int32_t arch, char* out_file);
int32_t GenClose(void* gen_prm);
char* GenProc(int32_t arg_num, ...);

#ifdef __cplusplus
}
#endif

#endif
