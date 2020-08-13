/*
 * Copyright (c) 2020/07 gravieb <truongptk30a3@gmail.com>
 * This file is released under the GPLv3
 */

#include <stdlib.h>
#include <stdbool.h>
#include "parse.h"
#include "lex.h"
#include "ast.h"
#include "gen.h"
#include "symtable.h"
#include "error_code.h"

typedef struct ParseParameter{
    bool avail;
    void *lex_prm;
    void *gen_prm;
    SymbolTable symbol_table;
    Token cur_token;
} ParseParameter;

#define MAX_PARSE_RSC 10

ParseParameter g_parse_prm[MAX_PARSE_RSC];

static AstNode* factor(ParseParameter* parse_prm);
static AstNode* term(ParseParameter* parse_prm);
static AstNode* expression(ParseParameter* parse_prm);
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
    /* statements -> expression SEMI
     *            -> expression SEMI statements
     *            -> 'print' expression   SEMI statements
     *            -> 'int'   identifier   SEMI statements
     *            ->  identifier '=' expression  SEMI statements
     */

    AstNode *node, *node1;
    Token op_tok;
    while ( !match(parse_prm, TokenEoi)) {
        if (match (parse_prm, TokenPrint)) {
            // stmt print
            op_tok = parse_prm->cur_token;
            LexProc(parse_prm->lex_prm, &(parse_prm->cur_token));
            node1 = expression(parse_prm);
            node = ast_create_unary(op_tok, node1);
        } else if (match (parse_prm, TokenIntType)) {
            // stmt int
            op_tok = parse_prm->cur_token;
            LexProc(parse_prm->lex_prm, &(parse_prm->cur_token));
            if (!match (parse_prm, TokenIdentifier)) {
                printf("Expect Identifier but token %d\n",parse_prm->cur_token.tok);
            }
            node1 = ast_create_leaf(parse_prm->cur_token);
            node = ast_create_unary(op_tok, node1);
            LexProc(parse_prm->lex_prm, &(parse_prm->cur_token));
        } else if (match (parse_prm, TokenIdentifier)) {
            // stmt identifier
            node = ast_create_leaf(parse_prm->cur_token);

            LexProc(parse_prm->lex_prm, &(parse_prm->cur_token));
            if (!match (parse_prm, TokenEqual)) {
                printf("Expect Equal but token %d\n",parse_prm->cur_token.tok);
            }
            op_tok = parse_prm->cur_token;

            LexProc(parse_prm->lex_prm, &(parse_prm->cur_token));
            node1 = expression(parse_prm);

            node = ast_create_node(op_tok, node, node1);
        } else {
            node = expression(parse_prm);
        }

        if (match (parse_prm, TokenSemi)) {
            LexProc(parse_prm->lex_prm, &(parse_prm->cur_token));   
        } else {
            printf("Missing semicolon\n");
        }

        int32_t res = ast_interpreter(node);
        // TODO: tentative disable gen x86 asm code, it will be removed after x64 support
        if (node->type != AstIntType && node->type != AstEqual) {
            char* r = ast_gen(parse_prm->gen_prm, node);
            GenOut(parse_prm->gen_prm, r);
        }
    }
}

AstNode* expression(ParseParameter* parse_prm)
{
    /*
     * expression -> term expression'
     * expression' -> PLUS term expression'
     */
    //TODO: confirm start valid token?
    AstNode* node = term(parse_prm);
    while (match(parse_prm, TokenPlus) || match(parse_prm, TokenMinus)) {
        Token op_tok = parse_prm->cur_token;
        LexProc(parse_prm->lex_prm, &(parse_prm->cur_token));
        AstNode* node1 = term(parse_prm);
        node = ast_create_node(op_tok, node, node1);
    }
    return node;
}

AstNode* term(ParseParameter* parse_prm)
{
    //TODO: confirm start valid token
    AstNode *node, *node1;
    node = factor(parse_prm);
    while (match(parse_prm, TokenMul) || match(parse_prm, TokenDiv)) {
        Token op_tok = parse_prm->cur_token;
        LexProc(parse_prm->lex_prm, &(parse_prm->cur_token));
        node1 = factor(parse_prm);
        node = ast_create_node(op_tok, node, node1);
    }
    return node;
}

AstNode* factor(ParseParameter* parse_prm)
{
    //TODO: confirm start valid token
    AstNode* node;
    if (match(parse_prm, TokenNumber)) { // TODO: or TokenId
        node = ast_create_leaf(parse_prm->cur_token);
        LexProc(parse_prm->lex_prm, &(parse_prm->cur_token));
    } else if (match(parse_prm, TokenLP)) {
        LexProc(parse_prm->lex_prm, &(parse_prm->cur_token));
        node = expression(parse_prm);
        if (match(parse_prm, TokenRP)) {
            LexProc(parse_prm->lex_prm, &(parse_prm->cur_token));
        } else {
            fprintf(stderr, "Missing close parenthesis\n");
        }
    } else {
        fprintf(stderr, "Number or identifier expected but tok %d\n", parse_prm->cur_token.tok);
    }

    return node;
}

bool match(ParseParameter* parse_prm, int32_t tok_type)
{
    if (-1 == parse_prm->cur_token.tok) {
        LexProc(parse_prm->lex_prm, &(parse_prm->cur_token));
    }
    return (parse_prm->cur_token.tok == tok_type);
}
