/*
 * Copyright (c) 2020/07 gravieb <truongptk30a3@gmail.com>
 * This file is released under the GPLv3
 */

/*
 * The document should be read
 * - https://aaronbloomfield.github.io/pdr/book/x86-64bit-ccc-chapter.pdf
 */

#include <string.h>
#include "log.h"
#include "gen.h"
#include "gen_internal.h"
#include "error_code.h"

static bool is_op_reg(char* arg);
static char* x86_64_load(int32_t value, FILE* out_file);
static char* x86_64_free(char* r, FILE* out_file);
static char* x86_64_out(char* value, FILE* out_file);
static char* x86_64_var(char* var, FILE* out_file);
static char* x86_64_local_var(char* var, int size, FILE* out_file);
static char* x86_64_add(char* r1, char* r2, FILE* out_file);
static char* x86_64_sub(char* r1, char* r2, FILE* out_file);
static char* x86_64_mul(char* r1, char* r2, FILE* out_file);
static char* x86_64_div(char* r1, char* r2, FILE* out_file);
static char* x86_64_lt(char* r1, char* r2, FILE* out_file);
static char* x86_64_le(char* r1, char* r2, FILE* out_file);
static char* x86_64_gt(char* r1, char* r2, FILE* out_file);
static char* x86_64_ge(char* r1, char* r2, FILE* out_file);
static char* x86_64_eq(char* r1, char* r2, FILE* out_file);
static char* x86_64_ne(char* r1, char* r2, FILE* out_file);

static char* x86_64_or(char* r1, char* r2, FILE* out_file);
static char* x86_64_and(char* r1, char* r2, FILE* out_file);
static char* x86_64_b_or(char* r1, char* r2, FILE* out_file);
static char* x86_64_b_xor(char* r1, char* r2, FILE* out_file);
static char* x86_64_b_and(char* r1, char* r2, FILE* out_file);

static char* x86_64_lt_j(char* r1, char* r2, char* label, FILE* out_file);
static char* x86_64_le_j(char* r1, char* r2, char* label, FILE* out_file);
static char* x86_64_gt_j(char* r1, char* r2, char* label, FILE* out_file);
static char* x86_64_ge_j(char* r1, char* r2, char* label, FILE* out_file);
static char* x86_64_eq_j(char* r1, char* r2, char* label, FILE* out_file);
static char* x86_64_ne_j(char* r1, char* r2, char* label, FILE* out_file);
static char* x86_64_jump(char* label, FILE* out_file);
static char* x86_64_zero_j(char* r, char* label, FILE* out_file);
static char* x86_64_label(char* label, FILE* out_file);
static char* x86_64_str_label(char* label, char* str, FILE* out_file);
static char* x86_64_func(char* name, int stack_size, FILE* out_file);
static char* x86_64_func_arg(int arg_order, FILE* out_file);
static char* x86_64_func_exit(char* exit_label, int stack_size, FILE* out_file);

static char* x86_64_func_call(char* name, FILE* out_file);
static char* x86_64_arg(char* arg, int idx, FILE* out_file);

static char* x86_64_store(char* r, char* var, FILE* out_file);
static char* x86_64_load_var(char* var, FILE* out_file);
static char* x86_64_return(char* r, char* exit_label, FILE* out_file);
static char* x86_64_reg_backup(FILE* out_file);
static char* x86_64_reg_restore(FILE* out_file);

typedef struct RegMap {
    char* reg64;
    char* reg8;
} RegMap;

//TODO: move all global to gen structure.
static char* const op_reg[] = {
    "%r10",
    "%r11",
    "%r12",
    "%r13",
    "%r14",
    "%r15"};

static char* reg[] = {
    "%r10",
    "%r11",
    "%r12",
    "%r13",
    "%r14",
    "%r15"};

static RegMap reg_map[] = {
    {"%r10", "%r10b"},
    {"%r11", "%r11b"},
    {"%r12", "%r12b"},
    {"%r13", "%r13b"},
    {"%r14", "%r14b"},
    {"%r15", "%r15b"}
};

static char* arg_reg[] = {
    "%rdi",
    "%rsi",
    "%rdx",
    "%rcx",
    "%r8",
    "%r9"};


static int cur_reg = 0;
static int var_on_stack = 0;
static int allocated_stack = 0;

int32_t GenLoadX86_64(GenFuncTable *func)
{
    func->f_load   = &x86_64_load;
    func->f_free   = &x86_64_free;
    func->f_out    = &x86_64_out;
    func->f_var    = &x86_64_var;
    func->f_l_var  = &x86_64_local_var;
    func->f_add    = &x86_64_add;
    func->f_sub    = &x86_64_sub;
    func->f_mul    = &x86_64_mul;
    func->f_div    = &x86_64_div;
    func->f_lt     = &x86_64_lt;
    func->f_le     = &x86_64_le;
    func->f_gt     = &x86_64_gt;
    func->f_ge     = &x86_64_ge;
    func->f_eq     = &x86_64_eq;
    func->f_ne     = &x86_64_ne;

    func->f_or     = &x86_64_or;
    func->f_and    = &x86_64_and;
    func->f_b_or   = &x86_64_b_or;
    func->f_b_xor  = &x86_64_b_xor;
    func->f_b_and  = &x86_64_b_and;

    func->f_lt_j   = &x86_64_lt_j;
    func->f_le_j   = &x86_64_le_j;
    func->f_gt_j   = &x86_64_gt_j;
    func->f_ge_j   = &x86_64_ge_j;
    func->f_eq_j   = &x86_64_eq_j;
    func->f_ne_j   = &x86_64_ne_j;
    func->f_jump   = &x86_64_jump;
    func->f_zero_j = &x86_64_zero_j;

    func->f_label      = &x86_64_label;
    func->f_str_label  = &x86_64_str_label;
    func->f_func       = &x86_64_func;
    func->f_func_arg   = &x86_64_func_arg;
    func->f_func_exit  = &x86_64_func_exit;
    func->f_func_call  = &x86_64_func_call;
    func->f_arg        = &x86_64_arg;
    func->f_store      = &x86_64_store;
    func->f_load_var   = &x86_64_load_var;
    func->f_return     = &x86_64_return;
    func->f_reg_backup = &x86_64_reg_backup;
    func->f_reg_restore= &x86_64_reg_restore;

    cur_reg = 0;
    var_on_stack = 0;

    return Success;
}

static bool is_op_reg(char* arg)
{
    for (int i = 0; i < sizeof(op_reg)/sizeof(*op_reg); i++) {
        if (!strncmp(arg, op_reg[i], sizeof(*op_reg))) {
            return true;
        }
    }
    return false;
}

char* reg_alloc()
{
    if (cur_reg >= sizeof(reg)/sizeof(*reg)) {
        MLOG(CLGT,"Not availabel register\n");
        return NULL;
    }

    MLOG(TRACE,"Alloc ============================\n");
    for (int i = 0; i < sizeof(reg)/sizeof(*reg); i++) {
        MLOG(TRACE,"Alloc: %d = %s\n",i,reg[i]);
    }
    MLOG(TRACE,"Alloc ============================\n");
    return reg[cur_reg++];
}

void reg_free(char* r)
{
    if (!is_op_reg(r)) {
        MLOG(TRACE,"Oops the register is not op register: %s\n",r);
        return;
    }

    if (0 == cur_reg) {
        MLOG(CLGT,"Free register invalid: r = %s\n",r);
        return;
    }

    for (int i = cur_reg; i < sizeof(reg)/sizeof(*reg); i++) {
        if (!strncmp(r, reg[i], sizeof(*reg))) {
            MLOG(CLGT,"Oops the register is freed\n");
            return;
        }
    }

    reg[--cur_reg] = r;
    MLOG(TRACE,"Free ============================\n");
    for (int i = 0; i < sizeof(reg)/sizeof(*reg); i++) {
        MLOG(TRACE,"Free: %d = %s\n",i,reg[i]);
    }
    MLOG(TRACE,"Free ============================\n");
}

char* reg64_to_8(char* r)
{
    for (int i = 0; i < sizeof(reg_map)/sizeof(reg_map[0]); i++) {
        if (!strncmp(reg_map[i].reg64 , r, strlen(r))) {
            return reg_map[i].reg8;
        }
    }
    return NULL;
}

char* x86_64_load(int32_t value, FILE* out_file)
{
    char* r = reg_alloc();
    fprintf(out_file,"\tmovq\t $%d, %s\n",value,r);
    return r;
}

char* x86_64_free(char* r, FILE* out_file)
{
    reg_free(r);
    return NULL;
}

char* x86_64_out(char* r, FILE* out_file)
{
    fprintf(out_file,"\tmovq\t %s, %s\n",r,"%rax");
    fprintf(out_file,"\tret\n");
    reg_free(r);
    return NULL;
}

char* x86_64_var(char* var, FILE* out_file)
{
    fprintf(out_file,"\t.comm\t %s,8,8\n",var);

    int s = sizeof(var) + sizeof("(%rip)");
    char* label = malloc(s*sizeof(char)+1);
    sprintf(label,"%s(%%rip)",var);

    return label;
}

char* x86_64_local_var(char* var, int size, FILE* out_file)
{
    (void) out_file;
    (void) var;
    var_on_stack = var_on_stack - size;
    if (allocated_stack < -var_on_stack) {
        MLOG(CLGT,"Stack is not enought\n");
        exit(1);
    }
    int s = sizeof("-1000") + sizeof("(%rbp)");
    char* label = malloc(s*sizeof(char)+1);
    sprintf(label,"%d(%%rbp)",var_on_stack);

    return label;
}

char* x86_64_add(char* r1, char* r2, FILE* out_file)
{
    fprintf(out_file,"\taddq\t %s, %s\n",r1,r2);
    reg_free(r1);
    return r2;
}

char* x86_64_sub(char* r1, char* r2, FILE* out_file)
{
    fprintf(out_file,"\tsubq\t %s, %s\n",r2,r1);
    reg_free(r2);
    return r1;
}

char* x86_64_mul(char* r1, char* r2, FILE* out_file)
{
    fprintf(out_file,"\timulq\t %s, %s\n",r1,r2);
    reg_free(r1);
    return r2;
}

char* x86_64_div(char* r1, char* r2, FILE* out_file)
{
    fprintf(out_file,"\tmovq\t %s, %s\n",r1,"%rax");
    fprintf(out_file, "\tcqo\n");
    fprintf(out_file,"\tidivq\t %s\n",r2);
    fprintf(out_file,"\tmovq\t %s, %s\n","%rax",r2);
    reg_free(r1);
    return r2;
}

char* x86_64_lt(char* r1, char* r2, FILE* out_file)
{
    fprintf(out_file, "\tcmpq\t %s, %s\n", r2, r1);
    fprintf(out_file, "\tsetl\t %s\n", reg64_to_8(r2));
    fprintf(out_file, "\tandq\t $255,%s\n", r2);
    reg_free(r1);
    return r2;
}

char* x86_64_le(char* r1, char* r2, FILE* out_file)
{
    fprintf(out_file, "\tcmpq\t %s, %s\n", r2, r1);
    fprintf(out_file, "\tsetle\t %s\n", reg64_to_8(r2));
    fprintf(out_file, "\tandq\t $255,%s\n", r2);
    reg_free(r1);
    return r2;
}

char* x86_64_gt(char* r1, char* r2, FILE* out_file)
{
    fprintf(out_file, "\tcmpq\t %s, %s\n", r2, r1);
    fprintf(out_file, "\tsetg\t %s\n", reg64_to_8(r2));
    fprintf(out_file, "\tandq\t $255,%s\n", r2);
    reg_free(r1);
    return r2;
}

char* x86_64_ge(char* r1, char* r2, FILE* out_file)
{
    fprintf(out_file, "\tcmpq\t %s, %s\n", r2, r1);
    fprintf(out_file, "\tsetge\t %s\n", reg64_to_8(r2));
    fprintf(out_file, "\tandq\t $255,%s\n", r2);
    reg_free(r1);
    return r2;
}

char* x86_64_eq(char* r1, char* r2, FILE* out_file)
{
    fprintf(out_file, "\tcmpq\t %s, %s\n", r2, r1);
    fprintf(out_file, "\tsete\t %s\n", reg64_to_8(r2));
    fprintf(out_file, "\tandq\t $255,%s\n", r2);
    reg_free(r1);
    return r2;
}

char* x86_64_ne(char* r1, char* r2, FILE* out_file)
{
    fprintf(out_file, "\tcmpq\t %s, %s\n", r2, r1);
    fprintf(out_file, "\tsetne\t %s\n", reg64_to_8(r2));
    fprintf(out_file, "\tandq\t $255,%s\n", r2);
    reg_free(r1);
    return r2;
}

static char* x86_64_or(char* r1, char* r2, FILE* out_file)
{
    // todo: how to execute? need investigate
    // tentative workarround throuth bitwise op
    char* r = reg_alloc();
    fprintf(out_file,"\tmovq\t $0, %s\n",r);

    fprintf(out_file, "\tcmpq\t %s, %s\n", r2, r);
    fprintf(out_file, "\tsetne\t %s\n", reg64_to_8(r2));
    fprintf(out_file, "\tandq\t $255,%s\n", r2);

    fprintf(out_file, "\tcmpq\t %s, %s\n", r1, r);
    fprintf(out_file, "\tsetne\t %s\n", reg64_to_8(r1));
    fprintf(out_file, "\tandq\t $255,%s\n", r1);

    fprintf(out_file,"\tor\t %s, %s\n",r1,r2);
    reg_free(r1);
    reg_free(r);
    return r2;
}

static char* x86_64_and(char* r1, char* r2, FILE* out_file)
{
    // todo: how to execute? need investigate
    // tentative workarround throuth bitwise op

    char* r = reg_alloc();
    fprintf(out_file,"\tmovq\t $0, %s\n",r);

    fprintf(out_file, "\tcmpq\t %s, %s\n", r2, r);
    fprintf(out_file, "\tsetne\t %s\n", reg64_to_8(r2));
    fprintf(out_file, "\tandq\t $255,%s\n", r2);

    fprintf(out_file, "\tcmpq\t %s, %s\n", r1, r);
    fprintf(out_file, "\tsetne\t %s\n", reg64_to_8(r1));
    fprintf(out_file, "\tandq\t $255,%s\n", r1);

    fprintf(out_file,"\tand\t %s, %s\n",r1,r2);
    reg_free(r1);
    return r2;
}

static char* x86_64_b_or(char* r1, char* r2, FILE* out_file)
{
    fprintf(out_file,"\tor\t %s, %s\n",r1,r2);
    reg_free(r1);
    return r2;
}

static char* x86_64_b_xor(char* r1, char* r2, FILE* out_file)
{
    fprintf(out_file,"\txor\t %s, %s\n",r1,r2);
    reg_free(r1);
    return r2;
}

static char* x86_64_b_and(char* r1, char* r2, FILE* out_file)
{
    fprintf(out_file,"\tand\t %s, %s\n",r1,r2);
    reg_free(r1);
    return r2;
}

char* x86_64_lt_j(char* r1, char* r2, char* label, FILE* out_file)
{
    fprintf(out_file, "\tcmpq\t %s, %s\n", r2, r1);
    fprintf(out_file, "\tjl\t %s\n", label);
    reg_free(r1);
    reg_free(r2);
    return label;
}

char* x86_64_le_j(char* r1, char* r2, char* label, FILE* out_file)
{
    fprintf(out_file, "\tcmpq\t %s, %s\n", r2, r1);
    fprintf(out_file, "\tjle\t %s\n", label);
    reg_free(r1);
    reg_free(r2);
    return label;
}

char* x86_64_gt_j(char* r1, char* r2, char* label, FILE* out_file)
{
    fprintf(out_file, "\tcmpq\t %s, %s\n", r2, r1);
    fprintf(out_file, "\tjg\t %s\n", label);
    reg_free(r1);
    reg_free(r2);
    return label;
}

char* x86_64_ge_j(char* r1, char* r2, char* label, FILE* out_file)
{
    fprintf(out_file, "\tcmpq\t %s, %s\n", r2, r1);
    fprintf(out_file, "\tjge\t %s\n", label);
    reg_free(r1);
    reg_free(r2);
    return label;
}

char* x86_64_eq_j(char* r1, char* r2, char* label, FILE* out_file)
{
    fprintf(out_file, "\tcmpq\t %s, %s\n", r2, r1);
    fprintf(out_file, "\tje\t %s\n", label);
    reg_free(r1);
    reg_free(r2);
    return label;
}

char* x86_64_ne_j(char* r1, char* r2, char* label, FILE* out_file)
{
    fprintf(out_file, "\tcmpq\t %s, %s\n", r2, r1);
    fprintf(out_file, "\tjne\t %s\n", label);
    reg_free(r1);
    reg_free(r2);
    return label;
}

char* x86_64_jump(char* label, FILE* out_file)
{
    fprintf(out_file, "\tjmp\t %s\n", label);
    return label;
}

char* x86_64_zero_j(char* r, char* label, FILE* out_file)
{
    fprintf(out_file, "\tcmpq\t $0, %s\n", r);
    fprintf(out_file, "\tje\t %s\n", label);
    reg_free(r);
    return label;
}

char* x86_64_label(char* label, FILE* out_file)
{
    fprintf(out_file, "%s:\n", label);
    return label;
}

char* x86_64_str_label(char* label, char* str, FILE* out_file)
{
    fprintf(out_file, "%s:\n", label);
    fprintf(out_file, "\t.string \"%s\"\n", str);
    return label;
}

char* x86_64_func(char* name, int stack_size, FILE* out_file)
{
    fprintf(out_file, "\t.text\n");
    fprintf(out_file, "\t.globl\t %s\n", name);
    fprintf(out_file, "\t.type\t %s, @function\n", name);
    fprintf(out_file, "%s:\n", name);
    // backup register
    x86_64_reg_backup(out_file);
    fprintf(out_file, "\tpushq\t %%rbp\n");
    fprintf(out_file, "\tmovq\t %%rsp, %%rbp\n");
    fprintf(out_file, "\tsubq\t $%d, %%rsp\n",stack_size);
    var_on_stack = 0;
    allocated_stack = stack_size;
    return name;
}

char* x86_64_func_arg(int arg_order, FILE* out_file)
{
    if (arg_order > 5) {
        MLOG(ERROR, "Now, only support until 6 input parameter\n");
        return NULL;
    }
    return arg_reg[arg_order];
}

char* x86_64_func_exit(char* exit_label, int stack_size, FILE* out_file)
{
    fprintf(out_file, "\t%s:\n", exit_label);
    fprintf(out_file, "\taddq\t $%d, %%rsp\n",stack_size);
    fprintf(out_file, "\tpopq\t %%rbp\n");
    // restore register
    x86_64_reg_restore(out_file);
    fprintf(out_file, "\tret\n");
    return NULL;
}

char* x86_64_func_call(char* name, FILE* out_file)
{
    fprintf(out_file, "\tcall\t %s\n", name);
    // return stored result register
    char* r = reg_alloc();
    fprintf(out_file, "\tmovq\t %%rax, %s\n", r);
    return r;
}

char* x86_64_arg(char* arg, int idx, FILE* out_file)
{
    if (idx > 5) {
        MLOG(ERROR, "Now, only support until 6 input parameter\n");
        return NULL;
    }

    // release if register
    if (is_op_reg(arg)) {
        reg_free(arg);
    }

    fprintf(out_file, "\tmovq\t %s, %s\n", arg, arg_reg[idx]);
    return NULL;
}


char* x86_64_store(char* var, char* r, FILE* out_file)
{
    fprintf(out_file, "\tmovq\t %s, %s\n", r, var);

    // todo: in func return, r can be %rax register.
    // need consider better solution
    if (is_op_reg(r)) {
        reg_free(r);
    }
    return var;
}

char* x86_64_load_var(char* var, FILE* out_file)
{
    char* r = reg_alloc();
    fprintf(out_file, "\tmovq\t %s, %s\n", var, r);
    return r;
}

static char* x86_64_return(char* r, char* exit_label, FILE* out_file)
{
    fprintf(out_file, "\tmovq\t %s, %%rax\n", r);
    fprintf(out_file, "\tjmp\t %s\n", exit_label);

    if (is_op_reg(r)) {
        reg_free(r);
    }
    return NULL;
}

static char* x86_64_reg_backup(FILE* out_file)
{
    // TODO: just tentatve backup all parameter
    for (int i = 0; i < sizeof(op_reg)/sizeof(*op_reg); i++) {
        fprintf(out_file, "\tpushq\t %s\n", op_reg[i]);
    }

    return NULL;
}

static char* x86_64_reg_restore(FILE* out_file)
{
    for (int i = sizeof(op_reg)/sizeof(*op_reg) - 1; i >= 0; i--) {
        fprintf(out_file, "\tpopq\t %s\n", op_reg[i]);
    }
    return NULL;
}
