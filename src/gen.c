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

char* GenLoad(void* gen_prm, int32_t value)
{
    GenParameter* prm = (GenParameter*)gen_prm;
    return prm->func.f_load(value, prm->out_asm_file);
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
