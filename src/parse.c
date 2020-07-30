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

static int32_t factor(ParseParameter* parse_prm);
static int32_t term(ParseParameter* parse_prm);
static int32_t expression(ParseParameter* parse_prm);
static void statements(ParseParameter* parse_prm);
static bool match(ParseParameter* parse_prm, int32_t tok_type);

int32_t register_r[5] = {0,0,0,0,0};
int32_t idx = 0;

int reg_alloc()
{
    if (idx >= sizeof(register_r)/sizeof(register_r[0])) {
        printf("Error CMNR\n");
        exit(1);
    }

    return idx++;
}

int reg_free(int i) {
    register_r[--idx] = i;
}

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
    int reg;
    while ( !match(parse_prm, TokenEoi)) {
        reg = expression(parse_prm);
        if (match (parse_prm, TokenSemi)) {
            LexProc(parse_prm->lex_prm, &(parse_prm->cur_token));   
        } else {
            printf("Missing semicolon\n");
        }

        printf("value = %d\n", register_r[reg]);
        reg_free(reg);
    }
}

int32_t expression(ParseParameter* parse_prm)
{
    /*
     * expression -> term expression'
     * expression' -> PLUS term expression'
     */
    //TODO: confirm start valid token?
    int32_t reg = term(parse_prm);
    while (match(parse_prm, TokenPlus)) {
        LexProc(parse_prm->lex_prm, &(parse_prm->cur_token));   
        int32_t reg1 = term(parse_prm);
        register_r[reg] += register_r[reg1];
        printf("r%d += r%d\n",reg, reg1);
        reg_free(reg1);
    }
    return reg;
}

int32_t term(ParseParameter* parse_prm)
{
    //TODO: confirm start valid token
    int32_t reg, reg1;
    reg = factor(parse_prm);
    while (match(parse_prm, TokenMul)) {
        LexProc(parse_prm->lex_prm, &(parse_prm->cur_token));   
        reg1 = factor(parse_prm);
        register_r[reg] *= register_r[reg1];
        printf("r%d *= r%d\n",reg,reg1);
        reg_free(reg1);
    }
    return reg;
}

int32_t factor (ParseParameter* parse_prm)
{
    //TODO: confirm start valid token
    int32_t reg;
    if (match(parse_prm, TokenNumber)) { // TODO: or TokenId

        reg = reg_alloc();
        register_r[reg] = parse_prm->cur_token.value;
        printf("r%d = %d\n", reg, parse_prm->cur_token.value);
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
