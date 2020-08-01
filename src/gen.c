/*
 * Copyright (c) 2020/07 gravieb <truongptk30a3@gmail.com>
 * This file is released under the GPLv3
 */

#include <stdbool.h>
#include <stdarg.h> 
#include "gen.h"
#include "error_code.h"

typedef struct GenParameter{
    bool avail;
    FILE* out_asm_file;
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

int32_t GenProc(int arg_count, ...)
{
    va_list ap;
    va_start(ap, arg_count);
    GenParameter *gen_prm =  va_arg(ap, GenParameter*);
    if (NULL == gen_prm) {
        printf("error cmnr\n");
        return InParameterInvalid;
    }

    int32_t tok_type =  va_arg(ap, int32_t);
    printf("tok_type %d\n",tok_type);

    int32_t tok_type1 =  va_arg(ap, int32_t);
    printf("tok_type1 %d\n",tok_type1);

    va_end(ap);
    return Success;
}
