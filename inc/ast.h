/*
 * Copyright (c) 2020/08 gravieb <truongptk30a3@gmail.com>
 * This file is released under the GPLv3
 */
#include <stdio.h>
#include <stdlib.h>
#include "meo.h"

#ifndef _AST_H_
#define _AST_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef struct AstNode {
    int32_t type;
    int32_t value; // todo: extend
    struct AstNode* left;
    struct AstNode* right;
} AstNode;

enum AstType {
    AstPlus,
    AstMinus,
    AstMul,
    AstDiv,
    AstNumber
};

AstNode* ast_create_node(int32_t tok_type, int32_t value, AstNode* left, AstNode* right);
AstNode* ast_create_leaf(int32_t tok_type, int32_t value);

#ifdef __cplusplus
}
#endif

#endif /* _AST_H_ */
