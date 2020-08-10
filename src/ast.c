/*
 * Copyright (c) 2020/08 gravieb <truongptk30a3@gmail.com>
 * This file is released under the GPLv3
 */

#include "ast.h"
#include "lex.h"

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
    default:
        printf("Can not create AstNode with tok %d\n", tok_type);
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
    node->type = ast_type;
    node->value = token.value;
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

int32_t ast_interpreter(AstNode* node)
{
    // todo:
    int32_t left = -1, right = -1;
    if (NULL != node->left) {
        left = ast_interpreter(node->left);
    }
    if (NULL != node->right) {
        right = ast_interpreter(node->right);
    }

    switch (node->type) {
    case AstNumber:
        return node->value;
        break;
    case AstPlus:
        return (left+right);
    case AstMinus:
        return (left-right);
    default:
        printf("Not yet to support ast type %d\n",node->type);
    }
    return -1;
}
