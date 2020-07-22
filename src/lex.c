/*
 * Copyright (c) 2020/07 gravieb <truongptk30a3@gmail.com>
 * This file is released under the GPLv3
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>

#include "lex.h"
#include "error_code.h"

typedef struct LexParameter{
    bool avail;
    FILE *in_file;
} LexParameter;

#define MAX_LEX_RSC 10
LexParameter g_lex_prm[MAX_LEX_RSC];

int32_t LexCreate(void)
{
    for (int i = 0; i < MAX_LEX_RSC; i++) {
        g_lex_prm[i].avail = true;
    }
    return Success;
}

int32_t LexDestroy(void)
{
    for (int i = 0; i < MAX_LEX_RSC; i++) {
        g_lex_prm[i].avail = true;
    }
    return Success;
}

int32_t LexOpen(void** lex_prm, const char* file_name)
{
    if (NULL != *lex_prm || NULL == file_name) {
        return InParameterInvalid;
    }

    int i = 0;
    for (i = 0; i < MAX_LEX_RSC; i++) {
        if (true == g_lex_prm[i].avail) {
            g_lex_prm[i].avail = false;
            break;
        }
    }
    if (MAX_LEX_RSC == i) {
        return LexLimitResource;
    }

    g_lex_prm[i].in_file = fopen(file_name, "r");

    *lex_prm = &g_lex_prm[i];
    return Success;
}

int32_t LexClose(void* prm)
{
    if (NULL == prm) {
        return InParameterInvalid;
    }
    LexParameter* lex_prm = (LexParameter*)prm;

    if (NULL == lex_prm->in_file) return -1;
    fclose(lex_prm->in_file);
    lex_prm->avail = true;   

    return Success;
}

int32_t LexProc(void* prm, Token *t)
{
    LexParameter* lex_prm = (LexParameter*)prm;
    return Success;
}

int32_t LexGetLine(void* prm)
{
    if (NULL == prm) {
        return -1;
    }
    return 0;
}
