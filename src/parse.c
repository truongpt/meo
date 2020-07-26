/*
 * Copyright (c) 2020/07 gravieb <truongptk30a3@gmail.com>
 * This file is released under the GPLv3
 */

#include <stdlib.h>
#include <stdbool.h>
#include "parse.h"
#include "lex.h"
#include "error_code.h"

typedef struct ParseParameter{
    bool avail;
    void *lex_prm;
} ParseParameter;

#define MAX_PARSE_RSC 10

ParseParameter g_parse_prm[MAX_PARSE_RSC];

static void factor(const ParseParameter* parse_prm);
static void term(const ParseParameter* parse_prm);
static void expression(const ParseParameter* parse_prm);
static void statements(const ParseParameter* parse_prm);
static bool match(const ParseParameter* parse_prm, int32_t tok_type);

int32_t ParseCreate(void)
{
    for (int i = 0; i < MAX_PARSE_RSC; i++) {
        g_parse_prm[i].avail = true;
    }
    return Success;
}

int32_t ParseDestroy(void)
{
    for (int i = 0; i < MAX_PARSE_RSC; i++) {
        g_parse_prm[i].avail = true;
    }
    return Success;
}

int32_t ParseOpen(void** parse_prm, void* lex_prm)
{
    if (NULL != *parse_prm || NULL == lex_prm) {
        return InParameterInvalid;
    }
    int i = 0;
    for (i = 0; i < MAX_PARSE_RSC; i++) {
        if (true == g_parse_prm[i].avail) {
            g_parse_prm[i].avail = false;
            break;
        }
    }
    if (MAX_PARSE_RSC == i) {
        return ParseLimitResource;
    }

    g_parse_prm[i].lex_prm = lex_prm;
    *parse_prm = &g_parse_prm[i];

    return Success;
}

int32_t ParseClose(void* prm)
{
    if (NULL == prm) {
        return InParameterInvalid;
    }

    ((ParseParameter*)prm)->avail = true;

    return Success;
}

int32_t ParseProc(void* prm)
{
    if (NULL == prm) {
        return InParameterInvalid;
    }
    ParseParameter* parse_prm = (ParseParameter*)prm;
    
    return Success;
}

void statements(const ParseParameter* parse_prm)
{
    /* statements -> expression SEMI 1 expression SEMI statements */
    while ( !match(parse_prm, TokenEoi)) {
        expression(parse_prm);
        if (match (parse_prm, TokenSemi)) {
            //TODO: get next token by LexProc(void* lex_prm, Token *t);
        } else {
            printf("Missing semicolon\n");
        }
    }
}

void expression (const ParseParameter* parse_prm)
{
    /*
     * expression -> term expression'
     * expression' -> PLUS term expression'
     */
    //TODO: confirm start valid token?
    term(parse_prm);
    while (match(parse_prm, TokenPlus)) {
        //TODO: get next token.
        term(parse_prm);
    }
}

void term (const ParseParameter* parse_prm)
{
    //TODO: confirm start valid token
    factor(parse_prm);
    while (match(parse_prm, TokenMul)) {
        //TODO: find next token
        factor(parse_prm);
    }
}

void factor (const ParseParameter* parse_prm)
{
    //TODO: confirm start valid token

    if (match(parse_prm, TokenNumber)) { // TODO: or TokenId
        //TODO: get next token
    } else if (match(parse_prm, TokenLP)) {
        //TODO: get next token
        expression(parse_prm);
        if (match(parse_prm, TokenRP)) {
            //TODO: get next token
        } else {
            fprintf(stderr, "Missing close parenthesis\n");
        }
    } else {
        fprintf( stderr, "Number or identifier expected\n");
    }

}

bool match(const ParseParameter* parse_prm, int32_t tok_type)
{
    //TODO: get next token by LexProc(void* lex_prm, Token *t);
    return true;
}
