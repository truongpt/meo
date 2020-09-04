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

static int32_t tok_2_ast(Token token);
static void ast_tree_free(AstNode* node);
static int32_t invert_ast(int32_t ast_type);

int32_t tok_2_ast (Token token)
{
    int32_t ast = -1;
    switch (token.tok) {
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
    case TokenLT:
        ast = AstLT;
        break;
    case TokenLE:
        ast = AstLE;
        break;
    case TokenGT:
        ast = AstGT;
        break;
    case TokenGE:
        ast = AstGE;
        break;
    case TokenEQ:
        ast = AstEQ;
        break;
    case TokenNE:
        ast = AstNE;
        break;
    case TokenOr:
        ast = AstOr;
        break;
    case TokenAnd:
        ast = AstAnd;
        break;
    case TokenBitOr:
        ast = AstBitOr;
        break;
    case TokenBitXor:
        ast = AstBitXor;
        break;
    case TokenBitAnd:
        ast = AstBitAnd;
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

// NOT logic
static int32_t invert_ast(int32_t ast_type)
{
    switch (ast_type) {
    case AstLT:
        return AstGE;
    case AstLE:
        return AstGT;
    case AstGT:
        return AstLE;
    case AstGE:
        return AstLT;
    case AstEQ:
        return AstNE;
    case AstNE:
        return AstEQ;
    }
    MLOG(CLGT, "Invalid input ast type\n");
    return -1;
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
    AstNode* right)
{
    AstNode* node = (AstNode*) malloc(sizeof(AstNode));
    memset(node, 0x00, sizeof(AstNode));

    node->type = AstFunc;
    node->left = left;
    node->right = right;
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
        memcpy(node->id_str, token.id_str, strlen(token.id_str));
        node->id_str[strlen(token.id_str)] = '\0';
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

void* ast_compile_if(void* gen_prm, AstNode* node)
{
    char label_false[10];
    char label_end[10];
    memset(label_false, 0x00, sizeof(label_false));
    memset(label_end, 0x00, sizeof(label_end));

    sprintf(label_false,"L%d",GenGetLabel(gen_prm));
    if (node->right) {
        sprintf(label_end,"L%d",GenGetLabel(gen_prm));
    }

    // Generate condition IF
    void* cond_value = ast_compile(gen_prm, node->left);
    GenZeroJump(gen_prm, cond_value, label_false);

    // Generation TRUE statements
    ast_compile(gen_prm, node->mid);

    // Jump to end if having FALSE statements
    if (node->right) {
        GenJump(gen_prm, label_end);
    }

    GenLabel(gen_prm, label_false);
    if (node->right) {
        // Generation FALSE statements
        ast_compile(gen_prm, node->right);
        GenLabel(gen_prm, label_end);
    }
    return NULL;
}

void* ast_compile_while(void* gen_prm, AstNode* node)
{
    char label_start[10];
    char label_end[10];
    memset(label_start, 0x00, sizeof(label_start));
    memset(label_end, 0x00, sizeof(label_end));

    sprintf(label_start,"L%d",GenGetLabel(gen_prm));
    sprintf(label_end,"L%d",GenGetLabel(gen_prm));

    GenLabel(gen_prm, label_start);

    // Generate condition WHILE
    void* cond_value = ast_compile(gen_prm, node->left);
    GenZeroJump(gen_prm, cond_value, label_end);

    // Generation statements
    ast_compile(gen_prm, node->right);

    // Jump to start for continue loop
    GenJump(gen_prm, label_start);

    // End label
    GenLabel(gen_prm, label_end);

    return NULL;
}

void* ast_compile_func(void* gen_prm, AstNode* node)
{
    // gen function lable
    GenFunc(gen_prm, node->left->id_str);
    // gen body
    return ast_compile(gen_prm, node->right);
}

char* gen_bin_op(void* gen_prm, char* left, char* right, int type)
{
    switch (type) {
    case AstPlus:
        return GenPlus(gen_prm, left, right);
    case AstMinus:
        return GenMinus(gen_prm, left, right);
    case AstMul:
        return GenMul(gen_prm, left, right);
    case AstDiv:
        return GenDiv(gen_prm, left, right);
    default:
        MLOG(CLGT,"Invalid type binary operator\n");
        return NULL;
    }
}

char* gen_relational_op(void* gen_prm, char* left, char* right, int type)
{
    switch (type) {
    case AstLT:
        return GenLT(gen_prm, left, right);
    case AstLE:
        return GenLE(gen_prm, left, right);
    case AstGT:
        return GenGT(gen_prm, left, right);
    case AstGE:
        return GenGE(gen_prm, left, right);
    case AstEQ:
        return GenEQ(gen_prm, left, right);
    case AstNE:
        return GenNE(gen_prm, left, right);
    default:
        MLOG(CLGT,"Invalid type relational operator\n");
        return NULL;
    }
}

char* gen_relational_jump(void* gen_prm, char* left, char* right, char* label, int32_t type)
{
    // invert type
    int32_t t = invert_ast(type);
    switch (t) {
    case AstLT:
        return GenLtJump(gen_prm, left, right, label);
    case AstLE:
        return GenLeJump(gen_prm, left, right, label);
    case AstGT:
        return GenGtJump(gen_prm, left, right, label);
    case AstGE:
        return GenGeJump(gen_prm, left, right, label);
    case AstEQ:
        return GenEqJump(gen_prm, left, right, label);
    case AstNE:
        return GenNeJump(gen_prm, left, right, label);
    default:
        MLOG(CLGT,"Invalid type relational operator jump\n");
        return NULL;
    }
}

void* ast_compile(void* gen_prm, AstNode* node)
{
    if (NULL == node) {
        // do nothing
        MLOG(DEBUG,"NULL node \n");
        return NULL;
    }

    switch (node->type)
    {
    case AstLink:
        ast_compile(gen_prm, node->left);
        ast_compile(gen_prm, node->right);
        return NULL;
    case AstIf:
        return ast_compile_if(gen_prm, node);
    case AstWhile:
        return ast_compile_while(gen_prm, node);
    case AstFunc:
        return ast_compile_func(gen_prm, node);
    }

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
    case AstLeftVar:
        if (NULL != left && AstIntType == ((AstNode*)left)->type) {
            // declare variable
            return GenVar(gen_prm, node->id_str);
        }
        return node->id_str;
    case AstIdentifier:
        return GenLoadVar(gen_prm, node->id_str);
    case AstReturn:
        return GenReturn(gen_prm, left);
    case AstAssign:
        return GenStore(gen_prm, left, right);
    case AstPlus:
    case AstMinus:
    case AstMul:
    case AstDiv:
        return gen_bin_op(gen_prm, left, right, node->type);
    case AstLT:
    case AstLE:
    case AstGT:
    case AstGE:
    case AstEQ:
    case AstNE:
        return gen_relational_op(gen_prm, left, right, node->type);
    default:
        MLOG(CLGT,"Not yet to support ast type %d\n",node->type);
    }
    return NULL;
}

void ast_gen(ParseParameter* parse_prm, AstNode* node)
{
    if (parse_prm->is_interpret) {
        // todo:
        // ast_interpret(parse_prm, node);
    } else {
        ast_compile(parse_prm->gen_prm, node);
        ast_tree_free(node);
    }
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
