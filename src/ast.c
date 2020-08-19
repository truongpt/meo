/*
 * Copyright (c) 2020/08 gravieb <truongptk30a3@gmail.com>
 * This file is released under the GPLv3
 */

#include <string.h>
#include "log.h"
#include "ast.h"
#include "lex.h"
#include "gen.h"
#include "parse_internal.h"

static int32_t tok_2_ast (int32_t tok_type);

int32_t tok_2_ast (int32_t tok_type)
{
    int ast = -1;
    switch (tok_type) {
    case TokenPlus:
        ast = AstPlus;
        break;
    case TokenMinus:
        ast = AstMinus;
        break;
    case TokenMul:
        ast = AstMul;
        break;
    case TokenDiv:
        ast = AstDiv;
        break;
    case TokenNumber:
        ast = AstNumber;
        break;
    case TokenPrint:
        ast = AstPrint;
        break;
    case TokenIntType:
        ast = AstIntType;
        break;
    case TokenIdentifier:
        ast = AstIdentifier;
        break;
    case TokenEqual:
        ast = AstEqual;
        break;
    default:
        mlog(CLGT,"Can not create AstNode with token: %s\n", tok2str(tok_type));
    }

    return ast;

}

AstNode* ast_create_node(
    Token token,
    AstNode* left,
    AstNode* right)
{
    AstNode* node = (AstNode*) malloc(sizeof(AstNode));
    int32_t ast_type = tok_2_ast(token.tok);
    switch (ast_type)
    {
    case AstNumber:
        node->value = token.value;
        break;
    case AstIdentifier:
        node->id_str = (char*)malloc(strlen(token.id_str));
        memcpy(node->id_str, token.id_str, strlen(token.id_str));
        break;
    /* default: */
    }

    node->type = ast_type;
    node->left = left;
    node->right = right;

    return node;
}

AstNode* ast_create_leaf(Token token)
{
    return ast_create_node(token, NULL, NULL);
}

AstNode* ast_create_unary(Token token, AstNode* left)
{
    return ast_create_node(token, left, NULL);
}

// for interpreter
AstNode* ast_bin_op(SymbolTable* var_table, AstNode* op, AstNode* left, AstNode* right)
{
    int32_t l_value = (AstIdentifier != left->type) ? left->value : symtable_get_value(var_table, left->id_str);;
    int32_t r_value = (AstIdentifier != right->type) ? right->value : symtable_get_value(var_table, right->id_str);;

    switch (op->type) {
    case AstPlus:
        l_value = l_value + r_value;
        break;
    case AstMinus:
        l_value = l_value - r_value;
        break;
    case AstMul:
        l_value = l_value * r_value;
        break;
    case AstDiv:
        l_value = l_value / r_value;
        break;
    default:
        mlog(CLGT,"Operator error %d\n",op->type);
    }
    op->value = l_value;
    return op;
}

AstNode* ast_interpret(ParseParameter* parse_prm, AstNode* node)
{
    AstNode *left = NULL, *right = NULL;
    if (NULL != node->left) {
        left = ast_interpret(parse_prm, node->left);
    }
    if (NULL != node->right) {
        right = ast_interpret(parse_prm, node->right);
    }

    switch (node->type) {
    case AstPrint:
        if (AstIdentifier != left->type) {
            printf("%d\n",left->value);
        } else {
            printf("%d\n",symtable_get_value(&(parse_prm->var_table), left->id_str));
        }
        return left;
    case AstNumber:
        return node;
    case AstIdentifier:
    {
        // symtable_add(&(parse_prm->var_table), node->id_str);
        if (-1 == symtable_find(&(parse_prm->var_table), node->id_str)) {
            // declare
            if (NULL == left) {
                mlog(CLGT, "Declare but unknow type\n");
            }
            symtable_add(&(parse_prm->var_table), node->id_str);
            if (AstIntType == left->type) {
                symtable_set_type(&(parse_prm->var_table), node->id_str, SymbolInt);
            } else {
                mlog(CLGT, "Not yet support the type %d\n",left->type);
            }
        } 
        return node;
    }
    case AstIntType:
        return node;
    case AstEqual:
    {
        int32_t value = 0;
        if (AstIdentifier != right->type) {
            value = right->value;
        } else {
            value = symtable_get_value(&(parse_prm->var_table), right->id_str);
        }
        symtable_set_value(&(parse_prm->var_table), left->id_str, value);
        return left;
    }
    case AstPlus:
    case AstMinus:
    case AstMul:
    case AstDiv:
        return ast_bin_op(&(parse_prm->var_table), node, left, right);
    default:
        mlog(CLGT, "Not yet to support ast type %d\n",node->type);
    }
    return NULL;
}

//todo: upgrade later
void* ast_compile(void* gen_prm, AstNode* node)
{
    char *left = NULL, *right = NULL;
    if (NULL != node->left) {
        left = (char*)ast_compile(gen_prm, node->left);
    }
    if (NULL != node->right) {
        right = (char*)ast_compile(gen_prm, node->right);
    }

    switch (node->type) {
    case AstNumber:
        return GenLoad(gen_prm,node->value);
    case AstPrint:
        return GenPrint(gen_prm, left);
    case AstIntType:
        return node;
    case AstIdentifier:
        if (NULL != left && AstIntType == ((AstNode*)left)->type) {
            // declare variable
            return GenVar(gen_prm, node->id_str);
        }
        return GenLoadVar(gen_prm, node->id_str);
    case AstEqual:
        return GenStore(gen_prm, node->left->id_str, right);
    case AstPlus:
        return GenPlus(gen_prm, left, right);
    case AstMinus:
        return GenMinus(gen_prm, left, right);
    case AstMul:
        return GenMul(gen_prm, left, right);
    case AstDiv:
        return GenDiv(gen_prm, left, right);
    default:
        mlog(CLGT,"Not yet to support ast type %d\n",node->type);
    }
    return NULL;
}

void ast_gen(ParseParameter* parse_prm, AstNode* node)
{
    if (parse_prm->is_interpret) {
        ast_interpret(parse_prm, node);
    } else {
        char* r = ast_compile(parse_prm->gen_prm, node);
    }
}
