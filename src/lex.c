/*
 * Copyright (c) 2020/07 gravieb <truongptk30a3@gmail.com>
 * This file is released under the GPLv3
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>

#include "log.h"
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
static char get_next(LexParameter *prm);
static char push_back(LexParameter *prm, char c);
static void read_number(LexParameter* lex_prm, char c, Token* t);
static void read_identifier(LexParameter* lex_prm, char c, Token* t);

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
    g_lex_prm[i].line = 1;
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
    char c = get_next(lex_prm);
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
    case '=':
        c = get_char(lex_prm);
        if (c == '=') {
            t->tok = TokenEqual;
        } else {
            t->tok = TokenAssign;
            push_back(lex_prm, c);
        }
        break;
    case '!':
        c = get_char(lex_prm);
        if (c == '=') {
            t->tok = TokenNE;
        } else {
            mlog(ERROR,"Not yet support the lexeme\n");
            // push_back(lex_prm, c);
        }
        break;
    case '>':
        c = get_char(lex_prm);
        if (c == '=') {
            t->tok = TokenGE;
        } else {
            t->tok = TokenGT;
            push_back(lex_prm, c);
        }
        break;
    case '<':
        c = get_char(lex_prm);
        if (c == '=') {
            t->tok = TokenLE;
        } else {
            t->tok = TokenLT;
            push_back(lex_prm, c);
        }
        break;
    case '0' ... '9':
        read_number(lex_prm, c, t);
        break;
    case 'a' ... 'z':
    case 'A' ... 'Z':
        read_identifier(lex_prm, c, t);
        break;
    default:
        mlog(CLGT, "Syntax Error\n");
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

static void read_identifier(LexParameter* lex_prm, char c, Token* t)
{
    char id[100] = "";
    int i = 0;
    id[i++] = c;

    c = get_char(lex_prm);
    while (('a' <= c && c <= 'z') ||
           ('A' <= c && c <= 'Z') ||
           ('0' <= c && c <= '9')) {
        if (i >= 100) {
            mlog(CLGT, "wft identifier too long\n");
            exit(1);
        }
        id[i++] = c;
        c = get_char(lex_prm);
    }

    if (!strncmp(id, "int", sizeof("int"))) {
        t->tok = TokenIntType;
    } else if (!strncmp(id, "void", sizeof("void"))) {
        t->tok = TokenVoidType;
    } else if (!strncmp(id, "return", sizeof("return"))) {
        t->tok = TokenReturn;
    } else if (!strncmp(id, "print", sizeof("print"))) {
        t->tok = TokenPrint;
    } else {
        t->tok = TokenIdentifier;
        t->id_str = (char*)malloc(i+1);
        memcpy(t->id_str, id, i);
        t->id_str[i] = '\0';
    }
    push_back(lex_prm, c);
}

void read_number(LexParameter* lex_prm, char c, Token* t)
{
    if ('0' <= c && c <= '9') {
        t->value = c - '0';
        t->tok = TokenNumber;
        c = get_char(lex_prm);
        while ('0' <= c && c <= '9') {
            t->value = t->value*10 + c-'0';
            c = get_char(lex_prm);
        }
        push_back(lex_prm, c);
    }
}

static char push_back(LexParameter *prm, char c)
{
    prm->push_back = c;
}

static char get_next(LexParameter *prm)
{
    char c = get_char(prm);
    while (c == ' '  ||
           c == '\t' ||
           c == '\n' ||
           c == '\v') {
        c = get_char(prm);
    }

    return c;
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
    if ('\n' == c) {
        prm->line++;
    }
    return c;
}
