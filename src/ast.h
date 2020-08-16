/*
 * Copyright (c) 2020/08 gravieb <truongptk30a3@gmail.com>
 * This file is released under the GPLv3
 */
#include <stdio.h>
#include <stdlib.h>
#include "meo.h"
#include "parse_internal.h"

#ifndef _AST_H_
#define _AST_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef struct AstNode {
    int32_t type;
    union {
        int32_t value;
        char* id_str;
    };
    struct AstNode* left;
    struct AstNode* right;
} AstNode;

enum AstType {
    AstPlus,
    AstMinus,
    AstMul,
    AstDiv,
    AstNumber,
    AstPrint,
    AstIntType,
    AstIdentifier,
    AstEqual
};

AstNode* ast_create_node(Token token, AstNode* left, AstNode* right);
AstNode* ast_create_leaf(Token token);
AstNode* ast_create_unary(Token token, AstNode* left);
AstNode* ast_interpret(ParseParameter* parse_prm, AstNode* node);
char* ast_compile(void* gen_prm, AstNode* node);
void ast_gen(ParseParameter* parse_prm, AstNode* node);

#ifdef __cplusplus
}
#endif

#endif /* _AST_H_ */
