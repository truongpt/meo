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
        char id_str[MAX_IDENT_LEN];
    };
    // if statement: if (left) {mid} else {right};
    // function: left -> function name, right -> function body
    struct AstNode* left;
    struct AstNode* mid;
    struct AstNode* right;
} AstNode;

enum AstType {
    AstPlus,
    AstMinus,
    AstMul,
    AstDiv,
    AstLT,
    AstLE,
    AstGT,
    AstGE,
    AstEQ,
    AstNE,
    AstNumber,
    AstPrint,
    AstIntType,
    AstString,
    AstIdentifier,
    AstLeftVar,
    AstRightVar,
    AstAssign,

    AstBitAnd,
    AstBitOr,
    AstBitXor,
    AstAnd,
    AstOr,

    AstIf,
    AstWhile,
    AstFunc,
    AstFuncCall,
    AstReturn,
    // Link statements
    AstLink
};

AstNode* ast_create_node(Token token, AstNode* left, AstNode* right);
AstNode* ast_create_leaf(Token token);
AstNode* ast_create_unary(Token token, AstNode* left);
AstNode* ast_create_link(AstNode* left, AstNode* right);
AstNode* ast_create_ifnode(AstNode* left, AstNode* mid, AstNode* right);
AstNode* ast_create_func(AstNode* left, AstNode* right);
AstNode* ast_create_func_call(void);
AstNode* ast_interpret(ParseParameter* parse_prm, AstNode* node);
void* ast_compile(void* gen_prm, AstNode* node);
void ast_gen(ParseParameter* parse_prm, AstNode* node);

#ifdef __cplusplus
}
#endif

#endif /* _AST_H_ */
