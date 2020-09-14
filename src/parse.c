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

static AstNode* function_call(ParseParameter* parse_prm, Token tok);
static AstNode* factor(ParseParameter* parse_prm);
static AstNode* mul(ParseParameter* parse_prm);
static AstNode* add(ParseParameter* parse_prm);
static AstNode* relational(ParseParameter* parse_prm);
static AstNode* logic_or(ParseParameter* parse_prm);
static AstNode* logic_and(ParseParameter* parse_prm);
static AstNode* bit_or(ParseParameter* parse_prm);
static AstNode* bit_xor(ParseParameter* parse_prm);
static AstNode* bit_and(ParseParameter* parse_prm);
static AstNode* equal(ParseParameter* parse_prm);
static AstNode* assign(ParseParameter* parse_prm);
static AstNode* expression(ParseParameter* parse_prm);

static AstNode* syntax_parse(ParseParameter* parse_prm);
static AstNode* statements(ParseParameter* parse_prm, AstNode* root);
static AstNode* stmt_decl(ParseParameter* parse_prm);
static AstNode* stmt_expr(ParseParameter* parse_prm);
static AstNode* stmt_if(ParseParameter* parse_prm);
static AstNode* stmt_while(ParseParameter* parse_prm);
static AstNode* stmt_return(ParseParameter* parse_prm);
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
    g_parse_prm[i].var_level = 0;
    g_parse_prm[i].var_map_pos = 0;
    // initialize symbol table
    symtable_init(&(g_parse_prm[i].symbol_table));

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

    while (!match(parse_prm, TokenEoi)) {
        AstNode* node = syntax_parse(parse_prm);
        // gen code
        ast_gen(parse_prm, node);
    }

    return Success;
}

AstNode* stmt_decl(ParseParameter* parse_prm)
{
    // stmt int
    AstNode* type = ast_create_leaf(parse_prm->cur_token);
    LexProc(parse_prm->lex_prm, &(parse_prm->cur_token));
    if (!match (parse_prm, TokenIdentifier)) {
        MLOG(CLGT,"Expect Identifier but token: %s at line: %d\n",tok2str(parse_prm->cur_token.tok), LexGetLine(parse_prm->lex_prm));
    }

    // add the identifier to symbol table
    if (Success != symtable_add(&(parse_prm->symbol_table), parse_prm->cur_token.id_str, parse_prm->var_level)) {
        MLOG(CLGT, "Duplicated declare variable: %s\n",parse_prm->cur_token.id_str);
        exit(1);
    }
    symtable_set_type(&(parse_prm->symbol_table), parse_prm->cur_token.id_str, parse_prm->var_level, SymbolInt);

    AstNode* var_name = ast_create_leaf(parse_prm->cur_token);
    AstNode* decl = ast_create_declare(type, var_name, AstVarLocal);

    Token var_tok = parse_prm->cur_token;
    LexProc(parse_prm->lex_prm, &(parse_prm->cur_token));

    if (match (parse_prm, TokenAssign)) {
        // initialize value at declare timming
        AstNode* left = ast_create_leaf(var_tok);
        left->type = AstLeftVar;

        Token op_tok = parse_prm->cur_token;
        LexProc(parse_prm->lex_prm, &(parse_prm->cur_token));

        AstNode* init = expression(parse_prm);
        init = ast_create_node(op_tok, left, init);
        decl = ast_create_link(decl, init);
    }

    if (match (parse_prm, TokenSemi)) {
        LexProc(parse_prm->lex_prm, &(parse_prm->cur_token));   
    } else {
        MLOG(CLGT,"Missing semicolon at line: %d\n",LexGetLine(parse_prm->lex_prm));
    }

    return decl;
}

AstNode* stmt_if(ParseParameter* parse_prm)
{
    /* if_statement: if_head
     *      |        if_head 'else' compound_statement
     *      ;
     */
    LexProc(parse_prm->lex_prm, &(parse_prm->cur_token));
    if (!match(parse_prm, TokenLP)){
        MLOG(CLGT,"Missing open parentheses ( at line: %d\n",LexGetLine(parse_prm->lex_prm));
        exit(1);
    }

    AstNode* cond = expression(parse_prm);
    if (NULL == cond) {
        MLOG(CLGT,"Error of IF condition at line: %d\n",LexGetLine(parse_prm->lex_prm));
        exit(1);
    }
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

AstNode* stmt_while(ParseParameter* parse_prm)
{
    Token tok = parse_prm->cur_token;
    LexProc(parse_prm->lex_prm, &(parse_prm->cur_token));
    if (!match(parse_prm, TokenLP)){
        MLOG(CLGT,"Missing open parentheses ( at line: %d\n",LexGetLine(parse_prm->lex_prm));
        exit(1);
    }

    AstNode* cond = expression(parse_prm);
    if (NULL == cond) {
        MLOG(CLGT,"Error of WHILE condition at line: %d\n",LexGetLine(parse_prm->lex_prm));
        exit(1);
    }
    AstNode* stmt = NULL;
    stmt = statements(parse_prm, stmt);

    return ast_create_node(tok, cond, stmt);
}

AstNode* stmt_for(ParseParameter* parse_prm)
{
    LexProc(parse_prm->lex_prm, &(parse_prm->cur_token));
    if (!match(parse_prm, TokenLP)){
        MLOG(CLGT,"Missing open parentheses ( at line: %d\n",LexGetLine(parse_prm->lex_prm));
        exit(1);
    }
    LexProc(parse_prm->lex_prm, &(parse_prm->cur_token));

    AstNode* pre_exp = NULL;
    pre_exp = statements(parse_prm, pre_exp);

    AstNode* cond_exp = stmt_expr(parse_prm);
    if (NULL == cond_exp) {
        MLOG(CLGT,"Not support without condition: %d\n",LexGetLine(parse_prm->lex_prm));
        exit(1);
    }

    AstNode* post_exp = expression(parse_prm);

    if (!match(parse_prm, TokenRP)){
        MLOG(CLGT,"Missing close parentheses ) at line: %d\n",LexGetLine(parse_prm->lex_prm));
        exit(1);
    }
    LexProc(parse_prm->lex_prm, &(parse_prm->cur_token));
 
    AstNode* stmt = NULL;
    stmt = statements(parse_prm, stmt);

    stmt = ast_create_link(stmt, post_exp);

    Token w_token;
    w_token.tok = TokenWhile; // handle For as While
    AstNode* w_stmt = ast_create_node(w_token, cond_exp, stmt);
    return ast_create_link(pre_exp, w_stmt);
}

AstNode* stmt_return(ParseParameter* parse_prm)
{
    Token op_tok = parse_prm->cur_token;
    LexProc(parse_prm->lex_prm, &(parse_prm->cur_token));
    AstNode* node = stmt_expr(parse_prm);
    return ast_create_unary(op_tok, node);
}

AstNode* stmt_scope(ParseParameter* parse_prm)
{
    // increase variable level at scope in
    parse_prm->var_level++;

    if(!match(parse_prm, TokenLBracket)) {
        MLOG(CLGT,"Missing open braces { at line: %d\n",LexGetLine(parse_prm->lex_prm));
        exit(1);
    }

    LexProc(parse_prm->lex_prm, &(parse_prm->cur_token));
    AstNode* node = NULL;
    while (!match(parse_prm, TokenRBracket) && !match(parse_prm, TokenEoi)) {
        node = statements(parse_prm, node);
    }

    if (match(parse_prm, TokenEoi)) {
        MLOG(CLGT,"Missing close braces } at line: %d\n",LexGetLine(parse_prm->lex_prm));
        exit(1);
    }

    // clear all local variable at scope
    symtable_clear_level(&(parse_prm->symbol_table), parse_prm->var_level);
    // decrease variable level at scope out
    parse_prm->var_level--;

    LexProc(parse_prm->lex_prm, &(parse_prm->cur_token));
    return node;
}


AstNode* stmt_expr(ParseParameter* parse_prm)
{
    AstNode* node = expression(parse_prm);
    if (match(parse_prm, TokenSemi)) {
        LexProc(parse_prm->lex_prm, &(parse_prm->cur_token));
    } else {
        MLOG(CLGT,"Missing semicolon at line: %d\n",LexGetLine(parse_prm->lex_prm));
    }

    return node;
}

AstNode* syntax_parse(ParseParameter* parse_prm)
{
    /* TODO: start with support void function without input param
     * void main()
     * {
     *     // function body
     * }
     */

    // At first data type
    if (!match(parse_prm, TokenVoidType) && !match(parse_prm, TokenIntType)) {
        MLOG(ERROR, "The version only support void main function : %s\n",parse_prm->cur_token.id_str);
        exit(1);
    }
    AstNode* type = ast_create_leaf(parse_prm->cur_token);

    // Identifier -> function name.
    // Identifier -> global variable.
    LexProc(parse_prm->lex_prm, &(parse_prm->cur_token));
    Token ident_tok = parse_prm->cur_token;

    // open parenthesis
    LexProc(parse_prm->lex_prm, &(parse_prm->cur_token));
    if (match(parse_prm, TokenLP)) {
        // TODO: parse input parameter of the function
        // close parenthesis
        LexProc(parse_prm->lex_prm, &(parse_prm->cur_token));
        if (!match(parse_prm, TokenRP)) {
            MLOG(ERROR, "Expect ( at line: %d\n",LexGetLine(parse_prm->lex_prm));
            exit(1);
        }

        LexProc(parse_prm->lex_prm, &(parse_prm->cur_token));
        if (!match(parse_prm, TokenLBracket)) {
            MLOG(ERROR, "Expect { at line: %d\n",LexGetLine(parse_prm->lex_prm));
        }

        AstNode* ident = ast_create_leaf(ident_tok);
        AstNode* body = stmt_scope(parse_prm);

        // create function tree.
        return ast_create_func(ident, body);
    } else {
        // add the global variable to symbol table
        if (Success != symtable_add(&(parse_prm->symbol_table), ident_tok.id_str, parse_prm->var_level)) {
            MLOG(CLGT,"Duplicated declare variable : %s\n",ident_tok.id_str);
            exit(1);
        }

        symtable_set_type(&(parse_prm->symbol_table), ident_tok.id_str, parse_prm->var_level, SymbolInt);

        // create global variable tree with type
        AstNode* var_name = ast_create_leaf(ident_tok);
        AstNode* global_var = ast_create_declare(type, var_name, AstVarGlobal);

        if (match (parse_prm, TokenSemi)) {
            LexProc(parse_prm->lex_prm, &(parse_prm->cur_token));
        } else {
            MLOG(CLGT,"Missing semicolon at line: %d\n",LexGetLine(parse_prm->lex_prm));
        }
        return global_var;
    }
}

AstNode* statements(ParseParameter* parse_prm, AstNode* root)
{
    /* statements -> expression SEMI
     *            -> expression SEMI statements
     *            -> 'int'   identifier   SEMI statements
     *            ->  identifier '=' expression  SEMI statements
     *            ->  if_statement
     */
    AstNode* node = NULL;
    switch(parse_prm->cur_token.tok)
    {
    case TokenIntType:
        node = stmt_decl(parse_prm);
        break;
    case TokenIf:
        node = stmt_if(parse_prm);
        break;
    case TokenWhile:
        node = stmt_while(parse_prm);
        break;
    case TokenFor:
        node = stmt_for(parse_prm);
        break;
    case TokenLBracket:
        node = stmt_scope(parse_prm);
        break;
    case TokenRBracket:
        MLOG(CLGT,"Redundancy open braces { at line: %d\n",LexGetLine(parse_prm->lex_prm));
        // Ignore to next
        LexProc(parse_prm->lex_prm, &(parse_prm->cur_token));
        break;
    case TokenReturn:
        node = stmt_return(parse_prm);
        break;
    case TokenSemi:
        MLOG(TRACE,"Just ; -> nothing to do :-) \n");
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
    return assign(parse_prm);
}

AstNode* assign(ParseParameter* parse_prm) {
    AstNode* node = logic_or(parse_prm);
    while (match(parse_prm, TokenAssign)) {
        Token op_tok = parse_prm->cur_token;
        LexProc(parse_prm->lex_prm, &(parse_prm->cur_token));
        AstNode* node1 = logic_or(parse_prm);

        if (AstIdentifier == node->type) {
            node->type = AstLeftVar; // todo: consider better design for Lval & Rval
        } else {
            MLOG(CLGT,"left operand is not Lvalue at line %d\n", LexGetLine(parse_prm->lex_prm));
            exit(1);
        }
        node = ast_create_node(op_tok, node, node1);
    }
    return node;
}

AstNode* logic_or(ParseParameter* parse_prm) {
    AstNode* node = logic_and(parse_prm);
    while (match(parse_prm, TokenOr)) {
        Token op_tok = parse_prm->cur_token;
        LexProc(parse_prm->lex_prm, &(parse_prm->cur_token));
        AstNode* node1 = logic_and(parse_prm);
        node = ast_create_node(op_tok, node, node1);
    }
    return node;
}

AstNode* logic_and(ParseParameter* parse_prm) {
    AstNode* node = bit_or(parse_prm);
    while (match(parse_prm, TokenAnd)) {
        Token op_tok = parse_prm->cur_token;
        LexProc(parse_prm->lex_prm, &(parse_prm->cur_token));
        AstNode* node1 = bit_or(parse_prm);
        node = ast_create_node(op_tok, node, node1);
    }
    return node;
}

AstNode* bit_or(ParseParameter* parse_prm) {
    AstNode* node = bit_xor(parse_prm);
    while (match(parse_prm, TokenBitOr)) {
        Token op_tok = parse_prm->cur_token;
        LexProc(parse_prm->lex_prm, &(parse_prm->cur_token));
        AstNode* node1 = bit_xor(parse_prm);
        node = ast_create_node(op_tok, node, node1);
    }
    return node;
}

AstNode* bit_xor(ParseParameter* parse_prm) {
    AstNode* node = bit_and(parse_prm);
    while (match(parse_prm, TokenBitXor)) {
        Token op_tok = parse_prm->cur_token;
        LexProc(parse_prm->lex_prm, &(parse_prm->cur_token));
        AstNode* node1 = bit_and(parse_prm);
        node = ast_create_node(op_tok, node, node1);
    }
    return node;
}

AstNode* bit_and(ParseParameter* parse_prm) {
    AstNode* node = equal(parse_prm);
    while (match(parse_prm, TokenBitAnd)) {
        Token op_tok = parse_prm->cur_token;
        LexProc(parse_prm->lex_prm, &(parse_prm->cur_token));
        AstNode* node1 = equal(parse_prm);
        node = ast_create_node(op_tok, node, node1);
    }
    return node;
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
    AstNode* node = NULL;
    if (match(parse_prm, TokenNumber)) {
        node = ast_create_leaf(parse_prm->cur_token);
        LexProc(parse_prm->lex_prm, &(parse_prm->cur_token));
    } else if (match(parse_prm, TokenIdentifier)) {
        Token op_tok = parse_prm->cur_token;
        LexProc(parse_prm->lex_prm, &(parse_prm->cur_token));
        if (match(parse_prm, TokenLP)) {
            // todo: need check the function is declared or not.
            node = function_call(parse_prm, op_tok);
        } else {
            if (-1 == symtable_find_valid(&(parse_prm->symbol_table), op_tok.id_str, parse_prm->var_level)) {
                MLOG(CLGT, "Can not find symbol %s at line: %d\n",op_tok.id_str, LexGetLine(parse_prm->lex_prm));
                exit(1);
            }
            node = ast_create_leaf(op_tok);
        }

    } else if (match(parse_prm, TokenLP)) {
        LexProc(parse_prm->lex_prm, &(parse_prm->cur_token));
        node = expression(parse_prm);
        if (match(parse_prm, TokenRP)) {
            LexProc(parse_prm->lex_prm, &(parse_prm->cur_token));
        } else {
            MLOG(CLGT,"Missing close parenthesis at line: %d\n",LexGetLine(parse_prm->lex_prm));
            exit(1);
        }
    } else {
        MLOG(CLGT, "Number or identifier expected but token: %s at line %d\n", tok2str(parse_prm->cur_token.tok), LexGetLine(parse_prm->lex_prm));
        exit(1);
    }

    return node;
}

AstNode* function_call(ParseParameter* parse_prm, Token tok)
{
    // create function call node.
    AstNode* node = ast_create_func_call();
    node->right = ast_create_leaf(tok); // function name

    AstNode* arg = node;
    LexProc(parse_prm->lex_prm, &(parse_prm->cur_token));
    while (!match(parse_prm, TokenRP)) {
        arg->left = ast_create_leaf(parse_prm->cur_token);
        LexProc(parse_prm->lex_prm, &(parse_prm->cur_token));
        arg = arg->left;
        if (match(parse_prm, TokenComma)) {
            LexProc(parse_prm->lex_prm, &(parse_prm->cur_token));
        } else if (match(parse_prm, TokenRP)) {
            LexProc(parse_prm->lex_prm, &(parse_prm->cur_token));
            break;
        } else {
            MLOG(CLGT,"Missing close parenthesis or comma at line: %d\n",LexGetLine(parse_prm->lex_prm));
        }
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
