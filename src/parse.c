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
    Token cur_token;
} ParseParameter;

#define MAX_PARSE_RSC 10

ParseParameter g_parse_prm[MAX_PARSE_RSC];

static void factor(ParseParameter* parse_prm);
static void term(ParseParameter* parse_prm);
static void expression(ParseParameter* parse_prm);
static void statements(ParseParameter* parse_prm);
static bool match(ParseParameter* parse_prm, int32_t tok_type);

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
    g_parse_prm[i].cur_token.tok = -1;
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

int32_t ParseProc(void* prm, int* res)
{
    if (NULL == prm || NULL == res) {
        return InParameterInvalid;
    }
    ParseParameter* parse_prm = (ParseParameter*)prm;
    statements(parse_prm);    
    return Success;
}

void statements(ParseParameter* parse_prm)
{
    /* statements -> expression SEMI 1 expression SEMI statements */
    while ( !match(parse_prm, TokenEoi)) {
        expression(parse_prm);
        if (match (parse_prm, TokenSemi)) {
            LexProc(parse_prm->lex_prm, &(parse_prm->cur_token));   
        } else {
            printf("Missing semicolon\n");
        }
    }
}

void expression (ParseParameter* parse_prm)
{
    /*
     * expression -> term expression'
     * expression' -> PLUS term expression'
     */
    //TODO: confirm start valid token?
    term(parse_prm);
    while (match(parse_prm, TokenPlus)) {
        LexProc(parse_prm->lex_prm, &(parse_prm->cur_token));   
        term(parse_prm);
    }
}

void term (ParseParameter* parse_prm)
{
    //TODO: confirm start valid token
    factor(parse_prm);
    while (match(parse_prm, TokenMul)) {
        LexProc(parse_prm->lex_prm, &(parse_prm->cur_token));   
        factor(parse_prm);
    }
}

void factor (ParseParameter* parse_prm)
{
    //TODO: confirm start valid token
    if (match(parse_prm, TokenNumber)) { // TODO: or TokenId
        LexProc(parse_prm->lex_prm, &(parse_prm->cur_token)); 
    } else if (match(parse_prm, TokenLP)) {
        LexProc(parse_prm->lex_prm, &(parse_prm->cur_token));
        expression(parse_prm);
        if (match(parse_prm, TokenRP)) {
            LexProc(parse_prm->lex_prm, &(parse_prm->cur_token));
        } else {
            fprintf(stderr, "Missing close parenthesis\n");
        }
    } else {
        fprintf( stderr, "Number or identifier expected\n");
    }

}

bool match(ParseParameter* parse_prm, int32_t tok_type)
{
    if (-1 == parse_prm->cur_token.tok) {
        LexProc(parse_prm->lex_prm, &(parse_prm->cur_token));
    }
    return (parse_prm->cur_token.tok == tok_type);
}
