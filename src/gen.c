/*
 * Copyright (c) 2020/07 gravieb <truongptk30a3@gmail.com>
 * This file is released under the GPLv3
 */

#include <stdbool.h>
#include <stdarg.h> 
#include "gen.h"
#include "meo.h"
#include "error_code.h"

#include "gen_internal.h"

typedef struct GenParameter{
    bool avail;
    FILE* out_asm_file;
    GenFuncTable func;
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
    if (NULL == g_gen_prm[i].out_asm_file) {
        return OpenFileError;
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

char* GenProc(int tok_type, ...)
{
    va_list ap;
    int arg_count = (TokenNumber == tok_type) ? 2 : 3;
    va_start(ap, arg_count);

    GenParameter *gen_prm =  va_arg(ap, GenParameter*);
    if (NULL == gen_prm) {
        return NULL;
    }

    char* r  = NULL;
    switch(tok_type) {
    case TokenNumber:
    {
        int32_t val = va_arg(ap, int32_t);
        r = gen_prm->func.f_load(val, gen_prm->out_asm_file);
        break;
    }
    case TokenPlus:
    {
        char* r1 = va_arg(ap, char*);
        char* r2 = va_arg(ap, char*);
        r = gen_prm->func.f_add(r1, r2, gen_prm->out_asm_file);
        break;
    }
    case TokenMinus:
    {
        char* r1 = va_arg(ap, char*);
        char* r2 = va_arg(ap, char*);
        r = gen_prm->func.f_sub(r1, r2, gen_prm->out_asm_file);
        break;
    }
    case TokenMul:
    {
        char* r1 = va_arg(ap, char*);
        char* r2 = va_arg(ap, char*);
        r = gen_prm->func.f_mul(r1, r2, gen_prm->out_asm_file);
        break;
    }
    case TokenDiv:
    {
        char* r1 = va_arg(ap, char*);
        char* r2 = va_arg(ap, char*);
        r = gen_prm->func.f_div(r1, r2, gen_prm->out_asm_file);
        break;
    }
    default:
        printf("Unsupported now¥n");
        break;
    }

    va_end(ap);
    return r;
}
