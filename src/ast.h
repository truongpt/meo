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
        char str[MAX_STR_LEN];
        char exit_label[MAX_LABEL_LEN];
    };
    int32_t var_type;

    int32_t var_level; // use to manage local var in scope
    int32_t arg_order; // use to handle function argument.

    // if statement: if (left) {mid} else {right};
    // function: left -> function name, mid -> argument, right -> function body
    struct AstNode* left;
    struct AstNode* mid;
    struct AstNode* right;
} AstNode;

enum AstVarType{
    AstVarGlobal,
    AstVarLocal
};

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

    AstDeclare,
    AstNumber,
    AstIntType,
    AstVoidType,
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
    AstFuncArg,

    AstFuncCall,
    AstArgPass,
    AstReturn,
    // Link statements
    AstLink
};

AstNode* ast_create_node(Token token, AstNode* left, AstNode* right);
AstNode* ast_create_leaf(Token token);
AstNode* ast_create_unary(Token token, AstNode* left);
AstNode* ast_create_link(AstNode* left, AstNode* right);
AstNode* ast_create_ifnode(AstNode* left, AstNode* mid, AstNode* right);
AstNode* ast_create_func(AstNode* left, AstNode* mid, AstNode* right);
AstNode* ast_create_arg_init(Token token, int arg_order);
AstNode* ast_create_leaf(Token token);
AstNode* ast_create_func_call(void);
AstNode* ast_create_arg_pass(AstNode* arg, int arg_order);

AstNode* ast_create_declare(AstNode* left, AstNode* right, int var_type);
AstNode* ast_interpret(ParseParameter* parse_prm, AstNode* node);
void* ast_compile(ParseParameter* parse_prm, AstNode* node);
void ast_gen(ParseParameter* parse_prm, AstNode* node);

void ast_map_init(ParseParameter* parse_prm);

#ifdef __cplusplus
}
#endif

#endif /* _AST_H_ */
