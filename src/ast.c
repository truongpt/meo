/*
 * Copyright (c) 2020/08 gravieb <truongptk30a3@gmail.com>
 * This file is released under the GPLv3
 */

#include "ast.h"
#include "lex.h"
#include "gen.h"

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

// for interpreter
int32_t ast_interpreter(AstNode* node)
{
    int32_t left = -1, right = -1;
    if (NULL != node->left) {
        left = ast_interpreter(node->left);
    }
    if (NULL != node->right) {
        right = ast_interpreter(node->right);
    }

    switch (node->type) {
    case AstPrint:
        printf("%d\n",left);
        return 0;
    case AstNumber:
        return node->value;
     case AstPlus:
        return (left+right);
    case AstMinus:
        return (left-right);
    case AstMul:
        return (left*right);
    case AstDiv:
        return (left/right);
    default:
        printf("Not yet to support ast type %d\n",node->type);
    }
    return -1;
}

char* ast_gen(void* gen_prm, AstNode* node)
{
    char *left = NULL, *right = NULL;
    if (NULL != node->left) {
        left = ast_gen(gen_prm, node->left);
    }
    if (NULL != node->right) {
        right = ast_gen(gen_prm, node->right);
    }

    switch (node->type) {
    case AstNumber:
        return GenLoad(gen_prm,node->value);
    case AstPrint:
        //todo
        return 0;
    case AstPlus:
        return GenPlus(gen_prm, left, right);
    case AstMinus:
        return GenMinus(gen_prm, left, right);
    case AstMul:
        return GenMul(gen_prm, left, right);
    case AstDiv:
        return GenDiv(gen_prm, left, right);
    default:
        printf("Not yet to support ast type %d\n",node->type);
    }
    return NULL;
}
