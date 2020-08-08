/*
 * Copyright (c) 2020/08 gravieb <truongptk30a3@gmail.com>
 * This file is released under the GPLv3
 */

#include "ast.h"

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
    int32_t tok_type,
    int32_t value,
    AstNode* left,
    AstNode* right)
{
    AstNode* node = (AstNode*) malloc(sizeof(AstNode));
    int32_t ast_type = tok_2_ast(tok_type);
    node->type = ast_type;
    node->value = value;
    node->left = left;
    node->right = right;
    return node;
}

AstNode* ast_create_leaf(int32_t tok_type, int32_t value)
{
    return ast_create_node(tok_type, value, NULL, NULL);
}
