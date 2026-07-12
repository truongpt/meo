/*
 * Copyright (c) 2020/08 gravieb <truongptk30a3@gmail.com>
 * This file is released under the GPLv3
 */

#include <string.h>
#include "log.h"
#include "ast.h"
#include "lex.h"
#include "op_table.h"

static int32_t tok_2_ast(Token token);

int32_t tok_2_ast (Token token)
{
    OpDesc* op = op_find(token.tok);
    if (op) {
        return op->ast;
    }

    int32_t ast = -1;
    switch (token.tok) {
    case TokenNumber:
        ast = AstNumber;
        break;
    case TokenIntType:
        ast = AstIntType;
        break;
    case TokenVoidType:
        ast = AstVoidType;
        break;
    case TokenString:
        ast = AstString;
        break;
    case TokenIdentifier:
        ast = AstIdentifier;
        break;
    case TokenAssign:
        ast = AstAssign;
        break;
    case TokenWhile:
        ast = AstWhile;
        break;
    case TokenReturn:
        ast = AstReturn;
        break;
    default:
        MLOG(CLGT,"Can not create AstNode with token: %s\n", tok2str(token.tok));
    }

    return ast;
}

AstNode* ast_create_link(
    AstNode* left,
    AstNode* right)
{
    AstNode* node = (AstNode*) malloc(sizeof(AstNode));
    memset(node, 0x00, sizeof(AstNode));

    node->type = AstLink;
    node->left = left;
    node->right = right;
    return node;
}

AstNode* ast_create_ifnode(
    AstNode* left,
    AstNode* mid,
    AstNode* right)
{
    AstNode* node = (AstNode*) malloc(sizeof(AstNode));
    memset(node, 0x00, sizeof(AstNode));

    node->type = AstIf;
    node->left = left;
    node->mid = mid;
    node->right = right;
    return node;
}

AstNode* ast_create_func(
    AstNode* left,
    AstNode* mid,
    AstNode* right)
{
    AstNode* node = (AstNode*) malloc(sizeof(AstNode));
    memset(node, 0x00, sizeof(AstNode));

    node->type = AstFunc;
    node->left = left;
    node->mid = mid;
    node->right = right;
    return node;
}

AstNode* ast_create_arg_init(
    Token token,
    int arg_order)
{
    AstNode* var = ast_create_leaf(token);
    var->type = AstLeftVar;

    AstNode* arg = (AstNode*) malloc(sizeof(AstNode));
    memset(arg, 0x00, sizeof(AstNode));
    arg->type = AstFuncArg;
    arg->arg_order = arg_order;
    arg->left = NULL;
    arg->right = NULL;

    AstNode* node = (AstNode*) malloc(sizeof(AstNode));
    memset(node, 0x00, sizeof(AstNode));
    node->type = AstAssign;
    node->left = var;
    node->right = arg;

    return node;
}
AstNode* ast_create_declare(
    AstNode* left,
    AstNode* right,
    int var_type)
{
    AstNode* node = (AstNode*) malloc(sizeof(AstNode));
    memset(node, 0x00, sizeof(AstNode));

    node->type     = AstDeclare;
    node->var_type = var_type;
    node->left     = left;
    node->right    = right;
    return node;
}


AstNode* ast_create_func_call(void)
{
    AstNode* node = (AstNode*) malloc(sizeof(AstNode));
    memset(node, 0x00, sizeof(AstNode));
    node->type = AstFuncCall;
    node->left = NULL;
    node->right = NULL;
    return node;
}

AstNode* ast_create_arg_pass(AstNode* arg, int arg_order)
{
    AstNode* node = (AstNode*) malloc(sizeof(AstNode));
    memset(node, 0x00, sizeof(AstNode));
    node->type = AstArgPass;
    node->arg_order = arg_order;
    node->left = arg;
    node->right = NULL;
    return node;

}

AstNode* ast_create_node(
    Token token,
    AstNode* left,
    AstNode* right)
{
    AstNode* node = (AstNode*) malloc(sizeof(AstNode));
    memset(node, 0x00, sizeof(AstNode));

    int32_t ast_type = tok_2_ast(token);
    switch (ast_type)
    {
    case AstNumber:
        node->value = token.value;
        break;
    case AstLeftVar:
    case AstIdentifier:
        //TODO: dangerous code :-o
        memset(node->id_str, 0x00, strlen(node->id_str));
        memcpy(node->id_str, token.id_str, strlen(token.id_str));
        if (-1 != token.var_id) {
            sprintf(&(node->id_str[strlen(token.id_str)]),"%d",token.var_id);
            MLOG(TRACE, "var: %s \n",node->id_str);
        }
        break;
    case AstString:
        memcpy(node->str, token.str, strlen(token.str));
        node->str[strlen(token.str)] = '\0';
        break;
    default:
        MLOG(TRACE, "AstNode %d\n", ast_type);
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

void ast_tree_free(AstNode* node)
{
    if (NULL == node) {
        return;
    }
    ast_tree_free(node->left);
    ast_tree_free(node->mid);
    ast_tree_free(node->right);
    free(node);
}
