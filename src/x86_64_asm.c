/*
 * Copyright (c) 2020/07 gravieb <truongptk30a3@gmail.com>
 * This file is released under the GPLv3
 */

#include <string.h>
#include "log.h"
#include "gen.h"
#include "gen_internal.h"
#include "error_code.h"

int32_t reg_index(char* r);
char* x86_64_load(int32_t value, FILE* out_file);
char* x86_64_free(char* r, FILE* out_file);
char* x86_64_out(char* value, FILE* out_file);
char* x86_64_print(char* r, FILE* out_file);
char* x86_64_var(char* var, FILE* out_file);
char* x86_64_add(char* r1, char* r2, FILE* out_file);
char* x86_64_sub(char* r1, char* r2, FILE* out_file);
char* x86_64_mul(char* r1, char* r2, FILE* out_file);
char* x86_64_div(char* r1, char* r2, FILE* out_file);
char* x86_64_lt(char* r1, char* r2, FILE* out_file);
char* x86_64_le(char* r1, char* r2, FILE* out_file);
char* x86_64_gt(char* r1, char* r2, FILE* out_file);
char* x86_64_ge(char* r1, char* r2, FILE* out_file);

char* x86_64_store(char* r, char* var, FILE* out_file);
char* x86_64_load_var(char* var, FILE* out_file);

static char* reg[] = {"%r8","%r9","%r10","%r11","%r12","%r13","%r14","%r15"};
static char* breg[] = {"%r8b","%r9b","%r10b","%r11b","%r12b","%r13b","%r14b","%r15b"};

static int cur_reg = 0;

int32_t GenLoadX86_64(GenFuncTable *func)
{
    func->f_load   = &x86_64_load;
    func->f_free   = &x86_64_free;
    func->f_out    = &x86_64_out;
    func->f_var    = &x86_64_var;
    func->f_print  = &x86_64_print;
    func->f_add    = &x86_64_add;
    func->f_sub    = &x86_64_sub;
    func->f_mul    = &x86_64_mul;
    func->f_div    = &x86_64_div;
    func->f_lt     = &x86_64_lt;
    func->f_le     = &x86_64_le;
    func->f_gt     = &x86_64_gt;
    func->f_ge     = &x86_64_ge;
    func->f_store  = &x86_64_store;
    func->f_load_var = &x86_64_load_var;
    cur_reg = 0;
    return Success;
}

char* reg_alloc()
{
    if (cur_reg >= sizeof(reg)/sizeof(*reg)) {
        mlog(CLGT,"Not available register\n");
        return NULL;
    }
    return reg[cur_reg++];
}

void reg_free(char* r)
{
    if (0 == cur_reg) {
        mlog(CLGT,"Free register invalid\n");
        exit(1);
    }
    reg[--cur_reg] = r;
}

int32_t reg_index(char* r)
{
    for (int i = 0; i < sizeof(reg)/sizeof(reg[0]); i++) {
        if (!strncmp(reg[i],r,strlen(r))) {
            return i;
        }
    }
    return -1;
}

char* x86_64_load(int32_t value, FILE* out_file)
{
    char* r = reg_alloc();
    fprintf(out_file,"\tmovq $%d, %s\n",value,r);
    return r;
}

char* x86_64_free(char* r, FILE* out_file)
{
    reg_free(r);
    return r;
}

char* x86_64_out(char* r, FILE* out_file)
{
    fprintf(out_file,"\tmovq %s, %s\n",r,"%rax");
    fprintf(out_file,"\tret\n");
    reg_free(r);
    return r;
}

char* x86_64_print(char* r, FILE* out_file)
{
    fprintf(out_file,"\tmovq %s, %%rsi\n",r);
    fprintf(out_file,"\tmovq $.LC0, %%rdi\n");
    fprintf(out_file,"\tmovq $0, %%rax\n");
    fprintf(out_file,"\tcall printf\n");
    reg_free(r);
    return r;
}

char* x86_64_var(char* var, FILE* out_file)
{
    fprintf(out_file,"\t.comm\t%s,8,8\n",var);
    return var;
}

char* x86_64_add(char* r1, char* r2, FILE* out_file)
{
    fprintf(out_file,"\taddq %s, %s\n",r1,r2);
    reg_free(r1);
    return r2;
}

char* x86_64_sub(char* r1, char* r2, FILE* out_file)
{
    fprintf(out_file,"\tsubq %s, %s\n",r2,r1);
    reg_free(r2);
    return r1;
}

char* x86_64_mul(char* r1, char* r2, FILE* out_file)
{
    fprintf(out_file,"\timulq %s, %s\n",r1,r2);
    reg_free(r1);
    return r2;
}

char* x86_64_div(char* r1, char* r2, FILE* out_file)
{
    fprintf(out_file,"\tmovq %s, %s\n",r1,"%rax");
    fprintf(out_file, "\tcqo\n");
    fprintf(out_file,"\tidivq %s\n",r2);
    fprintf(out_file,"\tmovq %s, %s\n","%rax",r2);
    reg_free(r1);
    return r2;
}

char* x86_64_lt(char* r1, char* r2, FILE* out_file)
{
    fprintf(out_file, "\tcmpq %s, %s\n", r2, r1);
    int ir2 = reg_index(r2);
    fprintf(out_file, "\tsetl %s\n", breg[ir2]);
    fprintf(out_file, "\tandq $255,%s\n", r2);
    reg_free(r1);
    return r2;
}

char* x86_64_le(char* r1, char* r2, FILE* out_file)
{
    fprintf(out_file, "\tcmpq %s, %s\n", r2, r1);
    int ir2 = reg_index(r2);
    fprintf(out_file, "\tsetle %s\n", breg[ir2]);
    fprintf(out_file, "\tandq $255,%s\n", r2);
    reg_free(r1);
    return r2;
}

char* x86_64_gt(char* r1, char* r2, FILE* out_file)
{
    fprintf(out_file, "\tcmpq %s, %s\n", r2, r1);
    int ir2 = reg_index(r2);
    fprintf(out_file, "\tsetg %s\n", breg[ir2]);
    fprintf(out_file, "\tandq $255,%s\n", r2);
    reg_free(r1);
    return r2;
}

char* x86_64_ge(char* r1, char* r2, FILE* out_file)
{
    fprintf(out_file, "\tcmpq %s, %s\n", r2, r1);
    int ir2 = reg_index(r2);
    fprintf(out_file, "\tsetge %s\n", breg[ir2]);
    fprintf(out_file, "\tandq $255,%s\n", r2);
    reg_free(r1);
    return r2;
}

char* x86_64_store(char* var, char* r, FILE* out_file)
{
    fprintf(out_file, "\tmovq %s, %s(\%%rip)\n", r, var);
    reg_free(r);
    return var;
}

char* x86_64_load_var(char* var, FILE* out_file)
{
    char* r = reg_alloc();
    fprintf(out_file, "\tmovq %s(\%%rip), %s\n", var, r);
    return r;
}
