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
static void* ast_compile_node(
    ParseParameter* parse_prm,
    AstNode* node,
    char* left,
    char* right);
static void ast_map_set_label(
    ParseParameter* parse_prm,
    char* id_str,
    char* label);
static char* ast_map_get_label(
    ParseParameter* parse_prm,
    char* id_str);
static void ast_compile_pre_func(
    void* gen_prm,
    AstNode* node,
    const char* exit_label,
    int* local_var_size);
static void* ast_compile_declare(
    ParseParameter* parse_prm,
    AstNode* node);

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
        memcpy(node->id_str, token.id_str, strlen(token.id_str));
        node->id_str[strlen(token.id_str)] = '\0';
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

void* ast_compile_if(ParseParameter* parse_prm, AstNode* node)
{
    void* gen_prm = parse_prm->gen_prm;
    char label_false[10];
    char label_end[10];
    memset(label_false, 0x00, sizeof(label_false));
    memset(label_end, 0x00, sizeof(label_end));

    sprintf(label_false,"L%d",GenGetLabel(gen_prm));
    if (node->right) {
        sprintf(label_end,"L%d",GenGetLabel(gen_prm));
    }

    // Generate condition IF
    void* cond_value = ast_compile(parse_prm, node->left);
    GenZeroJump(gen_prm, cond_value, label_false);

    // Generation TRUE statements
    ast_compile(parse_prm, node->mid);

    // Jump to end if having FALSE statements
    if (node->right) {
        GenJump(gen_prm, label_end);
    }

    GenLabel(gen_prm, label_false);
    if (node->right) {
        // Generation FALSE statements
        ast_compile(parse_prm, node->right);
        GenLabel(gen_prm, label_end);
    }
    return NULL;
}

void* ast_compile_while(ParseParameter* parse_prm, AstNode* node)
{
    void* gen_prm = parse_prm->gen_prm;
    char label_start[10];
    char label_end[10];
    memset(label_start, 0x00, sizeof(label_start));
    memset(label_end, 0x00, sizeof(label_end));

    sprintf(label_start,"L%d",GenGetLabel(gen_prm));
    sprintf(label_end,"L%d",GenGetLabel(gen_prm));

    GenLabel(gen_prm, label_start);

    // Generate condition WHILE
    void* cond_value = ast_compile(parse_prm, node->left);
    GenZeroJump(gen_prm, cond_value, label_end);

    // Generation statements
    ast_compile(parse_prm, node->right);

    // Jump to start for continue loop
    GenJump(gen_prm, label_start);

    // End label
    GenLabel(gen_prm, label_end);

    return NULL;
}

void ast_compile_pre_func(
    void* gen_prm,
    AstNode* node,
    const char* exit_label,
    int* local_var_size)
{
    if (NULL == node) {
        return;
    }

    if (AstString == node->type) {
        char label_str[10];
        memset(label_str, 0x00, sizeof(label_str));
        sprintf(label_str,".LS%d",GenGetLabel(gen_prm));
        GenStrLabel(gen_prm, label_str, node->str);

        // change string -> label str
        memset(node->str, 0x00, sizeof(node->str));
        sprintf(node->str,"$%s",label_str);
    } else if (AstReturn == node->type) {
        // setting exit label
        memset(node->exit_label, 0x00, sizeof(node->exit_label));
        memcpy(node->exit_label, exit_label, strlen(exit_label));
    } else if (AstDeclare == node->type) {
        if (AstIntType == node->left->type) {
            // TODO: using 4 size
            MLOG(TRACE, "var name: %s\n",node->right->id_str);
            *local_var_size += 8;
        } else {
            MLOG(CLGT, "Now, meo only support int type\n");
        }
    }

    ast_compile_pre_func(gen_prm, node->left, exit_label, local_var_size);
    ast_compile_pre_func(gen_prm, node->mid, exit_label, local_var_size);
    ast_compile_pre_func(gen_prm, node->right, exit_label, local_var_size);
}

void* ast_compile_func(ParseParameter* parse_prm, AstNode* node)
{
    void* gen_prm = parse_prm->gen_prm;
    char exit_label[10];
    memset(exit_label, 0x00, sizeof(exit_label));
    sprintf(exit_label,"L%d",GenGetLabel(gen_prm));

    // gen string before
    int local_var_size = 0;
    ast_compile_pre_func(gen_prm, node, exit_label, &local_var_size);

    //TODO: stack size = alignment of 16? need investigate
    local_var_size = ((local_var_size+15)/16)*16;
    MLOG(TRACE, "local_var_size %d\n",local_var_size);

    // gen function lable
    GenFunc(gen_prm, node->left->id_str, local_var_size);

    // gen input argument
    ast_compile(parse_prm, node->mid);

    // gen body
    ast_compile(parse_prm, node->right);

    // gen function end
    GenFuncExit(gen_prm, exit_label, local_var_size);
    return NULL;
}

void* ast_compile_func_call(ParseParameter* parse_prm, AstNode* node)
{
    //gen and pass input parameter.
    ast_compile(parse_prm, node->left);

    // call function
    return GenFuncCall(parse_prm->gen_prm, node->right->id_str);
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
    case AstBitOr:
        return GenBitOr(gen_prm, left, right);
    case AstBitXor:
        return GenBitXor(gen_prm, left, right);
    case AstBitAnd:
        return GenBitAnd(gen_prm, left, right);
    default:
        MLOG(CLGT,"Invalid type binary operator\n");
        return NULL;
    }
}

char* gen_logical_op(void* gen_prm, char* left, char* right, int type)
{
    switch (type) {
    case AstOr:
        return GenOr(gen_prm, left, right);
    case AstAnd:
        return GenAnd(gen_prm, left, right);
    default:
        MLOG(CLGT,"Invalid type logical operator\n");
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

void* ast_compile(ParseParameter* parse_prm, AstNode* node)
{
    if (NULL == node) {
        // do nothing
        MLOG(DEBUG,"NULL node \n");
        return NULL;
    }

    switch (node->type)
    {
    case AstLink:
        ast_compile(parse_prm, node->left);
        ast_compile(parse_prm, node->right);
        return NULL;
    case AstIf:
        return ast_compile_if(parse_prm, node);
    case AstWhile:
        return ast_compile_while(parse_prm, node);
    case AstFunc:
        return ast_compile_func(parse_prm, node);
    case AstFuncCall:
        return ast_compile_func_call(parse_prm, node);
    case AstDeclare:
        return ast_compile_declare(parse_prm, node);
    }

    char *left = NULL, *right = NULL;
    if (NULL != node->left) {
        left = (char*)ast_compile(parse_prm, node->left);
    }
    if (NULL != node->right) {
        right = (char*)ast_compile(parse_prm, node->right);
    }

    return ast_compile_node(parse_prm, node, left, right);
}

void* ast_compile_declare(ParseParameter* parse_prm, AstNode* node)
{
    if (NULL == node || NULL == node->left || NULL == node->right) {
        MLOG(CLGT, "Declare tree is not correct\n");
        exit(1);
    }

    void* gen_prm = parse_prm->gen_prm;

    // left -> type
    // right -> variable name
    AstNode* var = node->right;

    char* label = NULL;
    if (AstVarGlobal == node->var_type) {
        label = GenGlobalVar(gen_prm, var->id_str);
    } else if (AstVarLocal == node->var_type) {
        //todo: base on variable type (node->left) to decide size
        label =  GenLocalVar(gen_prm, var->id_str, 8);
    } else {
        MLOG(CLGT,"Unknow variable type %d\n",node->left->var_type);
    }

    /* TODO: Consider how to support variable of each scope can be same name. */
    /* I considered use node->var_level, that is passed from parse processing, but it need more consider. */
    /* At the moment, meo only supports variable name distint */
    // mapping ID -> label
    ast_map_set_label(parse_prm, var->id_str, label);
    return label;
}

void* ast_compile_node(ParseParameter* parse_prm, AstNode* node, char* left, char* right)
{
    void* gen_prm = parse_prm->gen_prm;
    switch (node->type) {
    case AstNumber:
        return GenLoad(gen_prm,node->value);
    case AstIntType:
        return node;
    case AstLeftVar:
        return ast_map_get_label(parse_prm, node->id_str);
    case AstIdentifier:
    {
        char* id_label = ast_map_get_label(parse_prm, node->id_str);
        return GenLoadVar(gen_prm, id_label);
    }
    case AstReturn:
        return GenReturn(gen_prm, left, node->exit_label);
    case AstFuncArg:
        return GenFuncArg(gen_prm, node->arg_order);
    case AstArgPass:
        return GenArg(parse_prm->gen_prm, left, node->arg_order);
    case AstAssign:
        return GenStore(gen_prm, left, right);
    case AstPlus:
    case AstMinus:
    case AstMul:
    case AstDiv:
    case AstBitOr:
    case AstBitXor:
    case AstBitAnd:
        return gen_bin_op(gen_prm, left, right, node->type);
    case AstOr:
    case AstAnd:
        return gen_logical_op(gen_prm, left, right, node->type);
    case AstLT:
    case AstLE:
    case AstGT:
    case AstGE:
    case AstEQ:
    case AstNE:
        return gen_relational_op(gen_prm, left, right, node->type);
    case AstString:
        return node->str;
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
        ast_compile(parse_prm, node);
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

void ast_map_set_label(ParseParameter* parse_prm, char* id_str,char* label)
{
    int pos = parse_prm->var_map_pos;
    if (pos >= MAX_IDENT_CNT) {
        MLOG(CLGT, "Too many variable \n");
        exit(1);
    }

    memcpy(parse_prm->var_map[pos].id, id_str, strlen(id_str));
    parse_prm->var_map[pos].label = label;
    parse_prm->var_map_pos++;
}

char* ast_map_get_label(ParseParameter* parse_prm, char* id_str)
{
    for (int i = 0; i < parse_prm->var_map_pos; i++) {
        if (strlen(parse_prm->var_map[i].id) == strlen(id_str) &&
            !strncmp(parse_prm->var_map[i].id, id_str, strlen(id_str))) {
            return parse_prm->var_map[i].label;
        }
    }
    return NULL;
}
