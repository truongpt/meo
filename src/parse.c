/*
 * Copyright (c) 2020/07 gravieb <truongptk30a3@gmail.com>
 * This file is released under the GPLv3
 */

#include <stdlib.h>
#include <stdbool.h>
#include "log.h"
#include "parse.h"
#include "lex.h"
#include "ast.h"
#include "gen.h"
#include "symtable.h"
#include "error_code.h"

#include "parse_internal.h"

#define MAX_PARSE_RSC 10

ParseParameter g_parse_prm[MAX_PARSE_RSC];

static AstNode* factor(ParseParameter* parse_prm);
static AstNode* mul(ParseParameter* parse_prm);
static AstNode* add(ParseParameter* parse_prm);
static AstNode* relational(ParseParameter* parse_prm);
static AstNode* equal(ParseParameter* parse_prm);
static AstNode* expression(ParseParameter* parse_prm);

static AstNode* statements(ParseParameter* parse_prm, AstNode* root);
static AstNode* stmt_print(ParseParameter* parse_prm);
static AstNode* stmt_decl(ParseParameter* parse_prm);
static AstNode* stmt_ident(ParseParameter* parse_prm);
static AstNode* stmt_expr(ParseParameter* parse_prm);
static AstNode* stmt_if(ParseParameter* parse_prm);
static AstNode* stmt_scope(ParseParameter* parse_prm);
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

int32_t ParseOpen(void** parse_prm, void* lex_prm, void* gen_prm, bool is_interpret)
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
    g_parse_prm[i].is_interpret = is_interpret;
    g_parse_prm[i].cur_token.tok = -1;
    // initialize symbol table
    symtable_init(&(g_parse_prm[i].symbol_table));
    symtable_init(&(g_parse_prm[i].var_table));

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
    AstNode* node = NULL;
    while (!match(parse_prm, TokenEoi)) {
        node = statements(parse_prm, node);
    }

    // gen code
    ast_gen(parse_prm, node);
    return Success;
}

AstNode* stmt_print(ParseParameter* parse_prm)
{
    AstNode *node, *node1;
    Token op_tok;
    op_tok = parse_prm->cur_token;
    LexProc(parse_prm->lex_prm, &(parse_prm->cur_token));
    node1 = expression(parse_prm);
    node = ast_create_unary(op_tok, node1);

    if (match (parse_prm, TokenSemi)) {
        LexProc(parse_prm->lex_prm, &(parse_prm->cur_token));   
    } else {
        mlog(CLGT,"Missing semicolon at line: %d\n",LexGetLine(parse_prm->lex_prm));
    }
    return node;
}

AstNode* stmt_decl(ParseParameter* parse_prm)
{
    // stmt int
    AstNode *node, *node1;

    node1 = ast_create_leaf(parse_prm->cur_token);
    LexProc(parse_prm->lex_prm, &(parse_prm->cur_token));
    if (!match (parse_prm, TokenIdentifier)) {
        mlog(CLGT,"Expect Identifier but token: %s at line: %d\n",tok2str(parse_prm->cur_token.tok), LexGetLine(parse_prm->lex_prm));
    }

    // add the identifier to symbol table
    symtable_add(&(parse_prm->symbol_table), parse_prm->cur_token.id_str);
    symtable_set_type(&(parse_prm->symbol_table), parse_prm->cur_token.id_str, SymbolInt);

    parse_prm->cur_token.left_value = true;
    node = ast_create_unary(parse_prm->cur_token, node1);
    LexProc(parse_prm->lex_prm, &(parse_prm->cur_token));

    if (match (parse_prm, TokenSemi)) {
        LexProc(parse_prm->lex_prm, &(parse_prm->cur_token));   
    } else {
        mlog(CLGT,"Missing semicolon at line: %d\n",LexGetLine(parse_prm->lex_prm));
    }

    return node;
}

AstNode* stmt_ident(ParseParameter* parse_prm)
{
    AstNode *node, *node1;
    Token op_tok;

    parse_prm->cur_token.left_value = true;
    node = ast_create_leaf(parse_prm->cur_token);

    // verify that existence in symbol table
    if (-1 == symtable_find(&(parse_prm->symbol_table), parse_prm->cur_token.id_str)) {
        mlog(CLGT, "Can not find symbol %s at line: %d\n",parse_prm->cur_token.id_str, LexGetLine(parse_prm->lex_prm));
        exit(1);
    }

    LexProc(parse_prm->lex_prm, &(parse_prm->cur_token));
    if (!match (parse_prm, TokenAssign)) {
        mlog(CLGT,"Expect Equal but token: %s at line: %d\n",tok2str(parse_prm->cur_token.tok),LexGetLine(parse_prm->lex_prm));
    }
    op_tok = parse_prm->cur_token;

    LexProc(parse_prm->lex_prm, &(parse_prm->cur_token));
    node1 = expression(parse_prm);
    node = ast_create_node(op_tok, node, node1);

    if (match (parse_prm, TokenSemi)) {
        LexProc(parse_prm->lex_prm, &(parse_prm->cur_token));   
    } else {
        mlog(CLGT,"Missing semicolon at line: %d\n",LexGetLine(parse_prm->lex_prm));
    }

    return node;
}

AstNode* stmt_if(ParseParameter* parse_prm)
{
    /* if_statement: if_head
    *      |        if_head 'else' compound_statement
    *      ;
    */
    LexProc(parse_prm->lex_prm, &(parse_prm->cur_token));
    if (!match(parse_prm, TokenLP)){
        mlog(CLGT,"Missing open parentheses ( at line: %d\n",LexGetLine(parse_prm->lex_prm));
        exit(1);
    }

    AstNode* cond = expression(parse_prm);
    AstNode* true_stmt = NULL;
    true_stmt = statements(parse_prm, true_stmt);

    // else
    AstNode* false_stmt = NULL;
    if (match(parse_prm, TokenElse)){
        LexProc(parse_prm->lex_prm, &(parse_prm->cur_token));
        false_stmt = statements(parse_prm, false_stmt);
    }

    // create If node
    return ast_create_ifnode(cond, true_stmt, false_stmt);
}

AstNode* stmt_scope(ParseParameter* parse_prm)
{
    if(!match(parse_prm, TokenLBracket)) {
        mlog(CLGT,"Missing open braces { at line: %d\n",LexGetLine(parse_prm->lex_prm));
        exit(1);
    }

    LexProc(parse_prm->lex_prm, &(parse_prm->cur_token));
    AstNode* node = NULL;
    while (!match(parse_prm, TokenRBracket) && !match(parse_prm, TokenEoi)) {
        node = statements(parse_prm, node);
    }

    if (match(parse_prm, TokenEoi)) {
        mlog(CLGT,"Missing close braces } at line: %d\n",LexGetLine(parse_prm->lex_prm));
    }

    LexProc(parse_prm->lex_prm, &(parse_prm->cur_token));
    return node;
}


AstNode* stmt_expr(ParseParameter* parse_prm)
{
    AstNode* node = expression(parse_prm);
    if (match(parse_prm, TokenSemi)) {
        LexProc(parse_prm->lex_prm, &(parse_prm->cur_token));
    } else {
        mlog(CLGT,"Missing semicolon at line: %d\n",LexGetLine(parse_prm->lex_prm));
    }

    return node;
}

AstNode* statements(ParseParameter* parse_prm, AstNode* root)
{
    /* statements -> expression SEMI
     *            -> expression SEMI statements
     *            -> 'print' expression   SEMI statements
     *            -> 'int'   identifier   SEMI statements
     *            ->  identifier '=' expression  SEMI statements
     *            ->  if_statement
     */
    AstNode* node = NULL;
    switch(parse_prm->cur_token.tok)
    {
    case TokenPrint:
        node = stmt_print(parse_prm);
        break;
    case TokenIntType:
        node = stmt_decl(parse_prm);
        break;
    case TokenIdentifier:
        node = stmt_ident(parse_prm);
        break;
    case TokenIf:
        node = stmt_if(parse_prm);
        break;
    /* case TokenElse: */
    /*     return root; */
    /*     break; */
    case TokenLBracket:
        node = stmt_scope(parse_prm);
        break;
    case TokenRBracket:
        mlog(CLGT,"Redundancy open braces { at line: %d\n",LexGetLine(parse_prm->lex_prm));
        // Ignore to next
        LexProc(parse_prm->lex_prm, &(parse_prm->cur_token));
        break;
    default:
        node = stmt_expr(parse_prm);
    }

    if (NULL == root) {
        return ast_create_link(node, NULL);
    } else {
        return ast_create_link(root, node);
    }
}

AstNode* expression(ParseParameter* parse_prm)
{
    return equal(parse_prm);
}

AstNode* equal(ParseParameter* parse_prm) {
    AstNode* node = relational(parse_prm);
    while (match(parse_prm, TokenEQ) ||
           match(parse_prm, TokenNE)) {
        Token op_tok = parse_prm->cur_token;
        LexProc(parse_prm->lex_prm, &(parse_prm->cur_token));
        AstNode* node1 = relational(parse_prm);
        node = ast_create_node(op_tok, node, node1);
    }
    return node;
}

AstNode* relational(ParseParameter* parse_prm) {
    AstNode* node = add(parse_prm);
    while (match(parse_prm, TokenLT) ||
           match(parse_prm, TokenLE) ||
           match(parse_prm, TokenGT) ||
           match(parse_prm, TokenGE)) {
        Token op_tok = parse_prm->cur_token;
        LexProc(parse_prm->lex_prm, &(parse_prm->cur_token));
        AstNode* node1 = add(parse_prm);
        node = ast_create_node(op_tok, node, node1);
    }
    return node;
}

AstNode* add(ParseParameter* parse_prm)
{
    AstNode* node = mul(parse_prm);
    while (match(parse_prm, TokenPlus) || match(parse_prm, TokenMinus)) {
        Token op_tok = parse_prm->cur_token;
        LexProc(parse_prm->lex_prm, &(parse_prm->cur_token));
        AstNode* node1 = mul(parse_prm);
        node = ast_create_node(op_tok, node, node1);
    }
    return node;
}

AstNode* mul(ParseParameter* parse_prm)
{
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
    AstNode* node = NULL;
    if (match(parse_prm, TokenNumber) || match(parse_prm, TokenIdentifier)) {
        if (match(parse_prm, TokenIdentifier)) {
            // right value
            parse_prm->cur_token.left_value = false;
        }
        node = ast_create_leaf(parse_prm->cur_token);
        LexProc(parse_prm->lex_prm, &(parse_prm->cur_token));
    } else if (match(parse_prm, TokenLP)) {
        LexProc(parse_prm->lex_prm, &(parse_prm->cur_token));
        node = expression(parse_prm);
        if (match(parse_prm, TokenRP)) {
            LexProc(parse_prm->lex_prm, &(parse_prm->cur_token));
        } else {
            mlog(CLGT,"Missing close parenthesis at line: %d\n",LexGetLine(parse_prm->lex_prm));
        }
    } else {
        mlog(CLGT, "Number or identifier expected but token: %s at line %d\n", tok2str(parse_prm->cur_token.tok), LexGetLine(parse_prm->lex_prm));
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
