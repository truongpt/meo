/*
 * Copyright (c) 2020/07 gravieb <truongptk30a3@gmail.com>
 * This file is released under the GPLv3
 */

#include <stdbool.h>
#include <stdarg.h>
#include <limits.h>
#include "gen.h"
#include "meo.h"
#include "error_code.h"
#include "log.h"
#include "gen_internal.h"

typedef struct GenParameter{
    bool avail;
    FILE* out_asm_file;
    GenFuncTable func;
    int32_t label_id;
} GenParameter;

#define MAX_GEN_RSC 10
GenParameter g_gen_prm[MAX_GEN_RSC];

int32_t GenCreate(void)
{
    for (int i = 0; i < MAX_GEN_RSC; i++) {
        g_gen_prm[i].avail = true;
    }
    return Success;
}

int32_t GenDestroy(void)
{
    for (int i = 0; i < MAX_GEN_RSC; i++) {
        g_gen_prm[i].avail = true;
    }
    return Success;
}

int32_t GenOpen(void** gen_prm, int32_t arch, char* out_file_name)
{
    if (NULL != *gen_prm || NULL == out_file_name) {
        return InParameterInvalid;
    }
    int i = 0;
    for (i = 0; i < MAX_GEN_RSC; i++) {
        if (true == g_gen_prm[i].avail) {
            g_gen_prm[i].avail = false;
            break;
        }
    }
    if (MAX_GEN_RSC == i) {
        return GenLimitResource;
    }

    g_gen_prm[i].out_asm_file = fopen(out_file_name, "w");
    g_gen_prm[i].label_id = 0;
    if (NULL == g_gen_prm[i].out_asm_file) {
        return OpenFileError;
    }
    switch (arch) {
    case GenX86_64:
        // todo: hardcode
        fprintf(g_gen_prm[i].out_asm_file,"%s\n",".LC0:");
        fprintf(g_gen_prm[i].out_asm_file,"\t.string \"%%d\\n\"\n");
        fprintf(g_gen_prm[i].out_asm_file,"\t%s\t%s\n",".globl","main");
        fprintf(g_gen_prm[i].out_asm_file,"\t%s\t%s\n",".type","main, @function");
        fprintf(g_gen_prm[i].out_asm_file,"%s\n","main:");
        GenLoadX86_64(&(g_gen_prm[i].func));
        break;
    default:
        printf("Unsupported\n");
    }
    *gen_prm = &g_gen_prm[i];
    return Success;
}

int32_t GenClose(void* gen_prm)
{
    if (NULL == gen_prm) {
        return InParameterInvalid;
    }

    GenParameter* l_gen_prm = (GenParameter*)gen_prm;
    fclose(l_gen_prm->out_asm_file);

    return Success;
}

int32_t GenGetLabel(void* gen_prm)
{
    GenParameter* prm = (GenParameter*)gen_prm;
    if (prm->label_id == INT_MAX) {
        mlog(CLGT,"Too much label \n");
        exit(1);
    }
    return prm->label_id++;
}

char* GenLoad(void* gen_prm, int32_t value)
{
    GenParameter* prm = (GenParameter*)gen_prm;
    return prm->func.f_load(value, prm->out_asm_file);
}

char* GenFree(void* gen_prm, char* r)
{
    GenParameter* prm = (GenParameter*)gen_prm;
    return prm->func.f_free(r, prm->out_asm_file);
}

char* GenPlus(void* gen_prm, char* r1, char* r2)
{
    GenParameter* prm = (GenParameter*)gen_prm;
    return prm->func.f_add(r1, r2, prm->out_asm_file);
}

char* GenMinus(void* gen_prm, char* r1, char* r2)
{
    GenParameter* prm = (GenParameter*)gen_prm;
    return prm->func.f_sub(r1, r2, prm->out_asm_file);
}

char* GenMul(void* gen_prm, char* r1, char* r2)
{
    GenParameter* prm = (GenParameter*)gen_prm;
    return prm->func.f_mul(r1, r2, prm->out_asm_file);
}

char* GenDiv(void* gen_prm, char* r1, char* r2)
{
    GenParameter* prm = (GenParameter*)gen_prm;
    return prm->func.f_div(r1, r2, prm->out_asm_file);
}

char* GenLT(void* gen_prm, char* r1, char* r2)
{
    GenParameter* prm = (GenParameter*)gen_prm;
    return prm->func.f_lt(r1, r2, prm->out_asm_file);
}

char* GenLE(void* gen_prm, char* r1, char* r2)
{
    GenParameter* prm = (GenParameter*)gen_prm;
    return prm->func.f_le(r1, r2, prm->out_asm_file);
}

char* GenGT(void* gen_prm, char* r1, char* r2)
{
    GenParameter* prm = (GenParameter*)gen_prm;
    return prm->func.f_gt(r1, r2, prm->out_asm_file);
}

char* GenGE(void* gen_prm, char* r1, char* r2)
{
    GenParameter* prm = (GenParameter*)gen_prm;
    return prm->func.f_ge(r1, r2, prm->out_asm_file);
}

char* GenEQ(void* gen_prm, char* r1, char* r2)
{
    GenParameter* prm = (GenParameter*)gen_prm;
    return prm->func.f_eq(r1, r2, prm->out_asm_file);
}

char* GenNE(void* gen_prm, char* r1, char* r2)
{
    GenParameter* prm = (GenParameter*)gen_prm;
    return prm->func.f_ne(r1, r2, prm->out_asm_file);
}

char* GenLtJump(void* gen_prm, char* r1, char* r2, char* label)
{
    GenParameter* prm = (GenParameter*)gen_prm;
    return prm->func.f_lt_j(r1, r2, label, prm->out_asm_file);
}

char* GenLeJump(void* gen_prm, char* r1, char* r2, char* label)
{
    GenParameter* prm = (GenParameter*)gen_prm;
    return prm->func.f_le_j(r1, r2, label, prm->out_asm_file);
}

char* GenGtJump(void* gen_prm, char* r1, char* r2, char* label)
{
    GenParameter* prm = (GenParameter*)gen_prm;
    return prm->func.f_gt_j(r1, r2, label, prm->out_asm_file);
}

char* GenGeJump(void* gen_prm, char* r1, char* r2, char* label)
{
    GenParameter* prm = (GenParameter*)gen_prm;
    return prm->func.f_ge_j(r1, r2, label, prm->out_asm_file);
}

char* GenEqJump(void* gen_prm, char* r1, char* r2, char* label)
{
    GenParameter* prm = (GenParameter*)gen_prm;
    return prm->func.f_eq_j(r1, r2, label, prm->out_asm_file);
}

char* GenNeJump(void* gen_prm, char* r1, char* r2, char* label)
{
    GenParameter* prm = (GenParameter*)gen_prm;
    return prm->func.f_ne_j(r1, r2, label, prm->out_asm_file);
}

char* GenJump(void* gen_prm, char* label)
{
    GenParameter* prm = (GenParameter*)gen_prm;
    return prm->func.f_jump(label, prm->out_asm_file);
}

char* GenLabel(void* gen_prm, char* label)
{
    GenParameter* prm = (GenParameter*)gen_prm;
    return prm->func.f_label(label, prm->out_asm_file);
}

void GenOut(void* gen_prm, char* r)
{
    GenParameter* prm = (GenParameter*)gen_prm;
    prm->func.f_out(r, prm->out_asm_file);
}

char* GenPrint(void* gen_prm, char* r)
{
    GenParameter* prm = (GenParameter*)gen_prm;
    return prm->func.f_print(r, prm->out_asm_file);
}

char* GenVar(void* gen_prm, char* var)
{
    GenParameter* prm = (GenParameter*)gen_prm;
    prm->func.f_var(var, prm->out_asm_file);
    return var;
}

char* GenStore(void* gen_prm, char* var, char* r)
{
    GenParameter* prm = (GenParameter*)gen_prm;
    return prm->func.f_store(var, r, prm->out_asm_file);
}

char* GenLoadVar(void* gen_prm, char* var)
{
    GenParameter* prm = (GenParameter*)gen_prm;
    return prm->func.f_load_var(var, prm->out_asm_file);
}
