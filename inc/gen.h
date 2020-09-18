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

int32_t GenGetLabel(void* gen_prm);
char* GenLoad(void* gen_prm, int32_t value);
char* GenFree(void* gen_prm, char* r);

char* GenPlus(void* gen_prm, char* r1, char* r2);
char* GenMinus(void* gen_prm, char* r1, char* r2);
char* GenMul(void* gen_prm, char* r1, char* r2);
char* GenDiv(void* gen_prm, char* r1, char* r2);

char* GenLT(void* gen_prm, char* r1, char* r2);
char* GenLE(void* gen_prm, char* r1, char* r2);
char* GenGE(void* gen_prm, char* r1, char* r2);
char* GenGT(void* gen_prm, char* r1, char* r2);
char* GenEQ(void* gen_prm, char* r1, char* r2);
char* GenNE(void* gen_prm, char* r1, char* r2);

char* GenOr(void* gen_prm, char* r1, char* r2);
char* GenAnd(void* gen_prm, char* r1, char* r2);
char* GenBitOr(void* gen_prm, char* r1, char* r2);
char* GenBitXor(void* gen_prm, char* r1, char* r2);
char* GenBitAnd(void* gen_prm, char* r1, char* r2);

char* GenLtJump(void* gen_prm, char* r1, char* r2, char* label);
char* GenLeJump(void* gen_prm, char* r1, char* r2, char* label);
char* GenGeJump(void* gen_prm, char* r1, char* r2, char* label);
char* GenGtJump(void* gen_prm, char* r1, char* r2, char* label);
char* GenEqJump(void* gen_prm, char* r1, char* r2, char* label);
char* GenNeJump(void* gen_prm, char* r1, char* r2, char* label);
char* GenJump(void* gen_prm, char* label);
char* GenZeroJump(void* gen_prm, char* r, char* label);
char* GenLabel(void* gen_prm, char* label);
char* GenStrLabel(void* gen_prm, char* label, char* str);
char* GenFunc(void* gen_prm, char* name, int stack_size);
char* GenFuncArg(void* gen_prm, int arg_order);
char* GenFuncExit(void* gen_prm, char* exit_label, int stack_size);
char* GenFuncCall(void* gen_prm, char* name);
char* GenArg(void* gen_prm, char* arg, int idx);

void GenOut(void* gen_prm, char* r);
char* GenGlobalVar(void* gen_prm, char* var);
char* GenLocalVar(void* gen_prm, char* var, int size);
char* GenStore(void* gen_prm, char* var, char* r);
char* GenLoadVar(void* gen_prm, char* var);
char* GenReturn(void* gen_prm, char* r, char* exit_label);

char* GenRegBackup(void* gen_prm);
char* GenRegRestore(void* gen_prm);
#ifdef __cplusplus
}
#endif

#endif
