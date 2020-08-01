/*
 * Copyright (c) 2020/07 gravieb <truongptk30a3@gmail.com>
 * This file is released under the GPLv3
 */

#include <stdlib.h>
#include <stdbool.h>
#include "parse.h"
#include "lex.h"
#include "gen.h"
#include "error_code.h"

typedef struct ParseParameter{
    bool avail;
    void *lex_prm;
    void *gen_prm;
    Token cur_token;
} ParseParameter;

#define MAX_PARSE_RSC 10

ParseParameter g_parse_prm[MAX_PARSE_RSC];

static char* factor(ParseParameter* parse_prm);
static char* term(ParseParameter* parse_prm);
static char* expression(ParseParameter* parse_prm);
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

int32_t ParseOpen(void** parse_prm, void* lex_prm, void* gen_prm)
{
    if (NULL != *parse_prm ||
        NULL == lex_prm ||
        NULL == gen_prm) {
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
    g_parse_prm[i].gen_prm = gen_prm;
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

int32_t ParseProc(void* prm)
{
    if (NULL == prm) {
        return InParameterInvalid;
    }

    ParseParameter* parse_prm = (ParseParameter*)prm;
    statements(parse_prm);    

    return Success;
}

void statements(ParseParameter* parse_prm)
{
    /* statements -> expression SEMI 1 expression SEMI statements */
    char* reg;
    while ( !match(parse_prm, TokenEoi)) {
        reg = expression(parse_prm);
        if (match (parse_prm, TokenSemi)) {
            LexProc(parse_prm->lex_prm, &(parse_prm->cur_token));   
        } else {
            printf("Missing semicolon\n");
        }

        // TODO: consider better meaning
        GenProc(2, parse_prm->gen_prm, (Token){TokenEoi, -1}, reg);
    }
}

char* expression(ParseParameter* parse_prm)
{
    /*
     * expression -> term expression'
     * expression' -> PLUS term expression'
     */
    //TODO: confirm start valid token?
    char* reg = term(parse_prm);
    while (match(parse_prm, TokenPlus)) {
        Token op_tok = parse_prm->cur_token;
        LexProc(parse_prm->lex_prm, &(parse_prm->cur_token));
        char* reg1 = term(parse_prm);
        reg = GenProc(4, parse_prm->gen_prm, op_tok, reg, reg1);
    }
    return reg;
}

char* term(ParseParameter* parse_prm)
{
    //TODO: confirm start valid token
    char *reg, *reg1;
    reg = factor(parse_prm);
    while (match(parse_prm, TokenMul)) {
        Token op_tok = parse_prm->cur_token;
        LexProc(parse_prm->lex_prm, &(parse_prm->cur_token));
        reg1 = factor(parse_prm);
        reg = GenProc(4, parse_prm->gen_prm, op_tok, reg, reg1);
    }
    return reg;
}

char* factor(ParseParameter* parse_prm)
{
    //TODO: confirm start valid token
    char* reg;
    if (match(parse_prm, TokenNumber)) { // TODO: or TokenId
        reg = GenProc(2, parse_prm->gen_prm, parse_prm->cur_token);
        LexProc(parse_prm->lex_prm, &(parse_prm->cur_token));
    } else if (match(parse_prm, TokenLP)) {
        LexProc(parse_prm->lex_prm, &(parse_prm->cur_token));
        reg = expression(parse_prm);
        if (match(parse_prm, TokenRP)) {
            LexProc(parse_prm->lex_prm, &(parse_prm->cur_token));
        } else {
            fprintf(stderr, "Missing close parenthesis\n");
        }
    } else {
        fprintf(stderr, "Number or identifier expected\n");
    }

    return reg;
}

bool match(ParseParameter* parse_prm, int32_t tok_type)
{
    if (-1 == parse_prm->cur_token.tok) {
        LexProc(parse_prm->lex_prm, &(parse_prm->cur_token));
    }
    return (parse_prm->cur_token.tok == tok_type);
}
