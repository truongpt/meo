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
    int32_t line;
    char push_back;
    FILE *in_file;
} LexParameter;

#define MAX_LEX_RSC 10
LexParameter g_lex_prm[MAX_LEX_RSC];

static char get_char(LexParameter *prm);

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
    g_lex_prm[i].push_back = -1; // -1 is OK? LOL
    g_lex_prm[i].line = 0;
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
    if (NULL == prm || NULL == t) {
        return InParameterInvalid;
    }

    LexParameter* lex_prm = (LexParameter*)prm;
    char c = get_char(lex_prm);
    switch (c) {
    case EOF:
        t->tok = TokenEoi;
        break;
    case '+':
        t->tok = TokenPlus;
        break;
    case '-':
        t->tok = TokenMinus;
        break;
    case '*':
        t->tok = TokenMul;
        break;
    case '/':
        t->tok = TokenDiv;
        break;
    case '(':
        t->tok = TokenLP;
        break;
    case ')':
        t->tok = TokenRP;
        break;
    case '{':
        t->tok = TokenLBracket;
        break;
    case '}':
        t->tok = TokenRBracket;
        break;
    case ';':
        t->tok = TokenSemi;
        break;
    default:
        // confirm it is numeric
        if ('0' <= c && c <= '9') {
            t->value = c - '0';
            t->tok = TokenNumber;

            // TODO: ugly code -> need refine
            c = fgetc(lex_prm->in_file);
            while ('0' <= c && c <= '9') {
                c = fgetc(lex_prm->in_file);
                t->value = t->value*10 + c-'0';
            }

            if (c != ' '  &&
                c != '\t' &&
                c != '\n' &&
                c != '\v') {
                lex_prm->push_back = c;
            }
            break;
        }
        return SyntaxError;
    }
    return Success;
}

int32_t LexGetLine(void* prm)
{
    if (NULL == prm) {
        return -1;
    }
    LexParameter* lex_prm = (LexParameter*)prm;
    return lex_prm->line;
}

static char get_char(LexParameter *prm)
{
    char c;
    if (-1 != prm->push_back) {
        c = prm->push_back;
        prm->push_back = -1;
        return c;
    }

    c = fgetc(prm->in_file);
    while (c == ' '  ||
           c == '\t' ||
           c == '\n' ||
           c == '\v') {
        c = fgetc(prm->in_file);
    }

    return c;
}
