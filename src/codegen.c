/*
 * Copyright (c) 2020/08 gravieb <truongptk30a3@gmail.com>
 * This file is released under the GPLv3
 */

#include <string.h>
#include "log.h"
#include "codegen.h"
#include "lex.h"
#include "gen.h"
#include "parse_internal.h"
#include "op_table.h"

static void* codegen_compile(ParseParameter* parse_prm, AstNode* node);
static void codegen_compile_pre_func(
    void* gen_prm,
    AstNode* node,
    const char* exit_label,
    int* local_var_size);
static void* codegen_compile_declare(
    ParseParameter* parse_prm,
    AstNode* node);
static void* codegen_compile_node(
    ParseParameter* parse_prm,
    AstNode* node,
    char* left,
    char* right);
static void codegen_map_set_label(
    ParseParameter* parse_prm,
    char* id_str,
    char* label);
static char* codegen_map_get_label(
    ParseParameter* parse_prm,
    char* id_str);

static void* codegen_compile_if(ParseParameter* parse_prm, AstNode* node)
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
    void* cond_value = codegen_compile(parse_prm, node->left);
    GenZeroJump(gen_prm, cond_value, label_false);

    // Generation TRUE statements
    codegen_compile(parse_prm, node->mid);

    // Jump to end if having FALSE statements
    if (node->right) {
        GenJump(gen_prm, label_end);
    }

    GenLabel(gen_prm, label_false);
    if (node->right) {
        // Generation FALSE statements
        codegen_compile(parse_prm, node->right);
        GenLabel(gen_prm, label_end);
    }
    return NULL;
}

static void* codegen_compile_while(ParseParameter* parse_prm, AstNode* node)
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
    void* cond_value = codegen_compile(parse_prm, node->left);
    GenZeroJump(gen_prm, cond_value, label_end);

    // Generation statements
    codegen_compile(parse_prm, node->right);

    // Jump to start for continue loop
    GenJump(gen_prm, label_start);

    // End label
    GenLabel(gen_prm, label_end);

    return NULL;
}

static void codegen_compile_pre_func(
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

    codegen_compile_pre_func(gen_prm, node->left, exit_label, local_var_size);
    codegen_compile_pre_func(gen_prm, node->mid, exit_label, local_var_size);
    codegen_compile_pre_func(gen_prm, node->right, exit_label, local_var_size);
}

static void* codegen_compile_func(ParseParameter* parse_prm, AstNode* node)
{
    void* gen_prm = parse_prm->gen_prm;
    char exit_label[10];
    memset(exit_label, 0x00, sizeof(exit_label));
    sprintf(exit_label,"L%d",GenGetLabel(gen_prm));

    // gen string before
    int local_var_size = 0;
    codegen_compile_pre_func(gen_prm, node, exit_label, &local_var_size);

    //TODO: stack size = alignment of 16? need investigate
    local_var_size = ((local_var_size+15)/16)*16;
    MLOG(TRACE, "local_var_size %d\n",local_var_size);

    // gen function lable
    GenFunc(gen_prm, node->left->id_str, local_var_size);

    // gen input argument
    codegen_compile(parse_prm, node->mid);

    // gen body
    codegen_compile(parse_prm, node->right);

    // gen function end
    GenFuncExit(gen_prm, exit_label, local_var_size);
    return NULL;
}

static void* codegen_compile_func_call(ParseParameter* parse_prm, AstNode* node)
{
    //gen and pass input parameter.
    codegen_compile(parse_prm, node->left);

    // call function
    return  GenFuncCall(parse_prm->gen_prm, node->right->id_str);
}

typedef char* (*GenOpFunc)(void* gen_prm, char* r1, char* r2);

static GenOpFunc codegen_op_funcs[] = {
    [AstPlus]    = GenPlus,
    [AstMinus]   = GenMinus,
    [AstMul]     = GenMul,
    [AstDiv]     = GenDiv,
    [AstBitOr]   = GenBitOr,
    [AstBitXor]  = GenBitXor,
    [AstBitAnd]  = GenBitAnd,
    [AstOr]      = GenOr,
    [AstAnd]     = GenAnd,
    [AstLT]      = GenLT,
    [AstLE]      = GenLE,
    [AstGT]      = GenGT,
    [AstGE]      = GenGE,
    [AstEQ]      = GenEQ,
    [AstNE]      = GenNE,
};

static char* gen_op_dispatch(void* gen_prm, char* left, char* right, int type)
{
    if (type >= 0 && type < (int)(sizeof(codegen_op_funcs) / sizeof(codegen_op_funcs[0]))) {
        GenOpFunc fn = codegen_op_funcs[type];
        if (fn) {
            return fn(gen_prm, left, right);
        }
    }
    MLOG(CLGT, "No gen_func for ast type %d\n", type);
    return NULL;
}

static void* codegen_compile_declare(ParseParameter* parse_prm, AstNode* node)
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

    // mapping ID -> label
    codegen_map_set_label(parse_prm, var->id_str, label);
    return label;
}

static void* codegen_compile_node(ParseParameter* parse_prm, AstNode* node, char* left, char* right)
{
    void* gen_prm = parse_prm->gen_prm;
    switch (node->type) {
    case AstNumber:
        return GenLoad(gen_prm,node->value);
    case AstIntType:
        return node;
    case AstLeftVar:
        return codegen_map_get_label(parse_prm, node->id_str);
    case AstIdentifier:
    {
        char* id_label = codegen_map_get_label(parse_prm, node->id_str);
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
    case AstOr:
    case AstAnd:
    case AstLT:
    case AstLE:
    case AstGT:
    case AstGE:
    case AstEQ:
    case AstNE:
        return gen_op_dispatch(gen_prm, left, right, node->type);
    case AstString:
        return node->str;
    default:
        MLOG(CLGT,"Not yet to support ast type %d\n",node->type);
    }
    return NULL;
}

void* codegen_compile(ParseParameter* parse_prm, AstNode* node)
{
    if (NULL == node) {
        // do nothing
        MLOG(DEBUG,"NULL node \n");
        return NULL;
    }

    switch (node->type)
    {
    case AstLink:
    {
        char* l = (char*)codegen_compile(parse_prm, node->left);
        if (NULL != l) {
            MLOG(TRACE, "l = %s\n",l);
            GenFree(parse_prm->gen_prm, l);
        }
        char* r = codegen_compile(parse_prm, node->right);
        if (NULL != r) {
            MLOG(TRACE, "r = %s\n",r);
            GenFree(parse_prm->gen_prm, r);
        }
        return NULL;
    }
    case AstIf:
        return codegen_compile_if(parse_prm, node);
    case AstWhile:
        return codegen_compile_while(parse_prm, node);
    case AstFunc:
        return codegen_compile_func(parse_prm, node);
    case AstFuncCall:
        return codegen_compile_func_call(parse_prm, node);
    case AstDeclare:
        return codegen_compile_declare(parse_prm, node);
    }

    char *left = NULL, *right = NULL;
    if (NULL != node->left) {
        left = (char*)codegen_compile(parse_prm, node->left);
    }
    if (NULL != node->right) {
        right = (char*)codegen_compile(parse_prm, node->right);
    }

    return codegen_compile_node(parse_prm, node, left, right);
}

void codegen_gen(ParseParameter* parse_prm, AstNode* node)
{
    if (parse_prm->is_interpret) {
        // todo:
        // ast_interpret(parse_prm, node);
    } else {
        char* r = codegen_compile(parse_prm, node);
        if (NULL != r) {
            MLOG(TRACE, "final reg = %s",r);
            GenFree(parse_prm->gen_prm, r);
        }
        ast_tree_free(node);
    }
}

void codegen_gen_all(ParseParameter* parse_prm)
{
    for (int i = 0; i < parse_prm->ast_count; i++) {
        codegen_gen(parse_prm, parse_prm->ast_list[i]);
        parse_prm->ast_list[i] = NULL;
    }
    parse_prm->ast_count = 0;
}

void codegen_map_init(ParseParameter* parse_prm)
{
    parse_prm->var_map_pos = 0;
    memset(parse_prm->var_map, 0x00, sizeof(parse_prm->var_map));
}

static void codegen_map_set_label(ParseParameter* parse_prm, char* id_str, char* label)
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

static char* codegen_map_get_label(ParseParameter* parse_prm, char* id_str)
{
    for (int i = 0; i < parse_prm->var_map_pos; i++) {
        if (strlen(parse_prm->var_map[i].id) == strlen(id_str) &&
            !strncmp(parse_prm->var_map[i].id, id_str, strlen(id_str))) {
            return parse_prm->var_map[i].label;
        }
    }
    return NULL;
}
