/*
 * Copyright (c) 2020/07 gravieb <truongptk30a3@gmail.com>
 * This file is released under the GPLv3
 */

#include "gen.h"
#include "gen_internal.h"
#include "error_code.h"

char* x86_64_load(int32_t value, FILE* out_file);
char* x86_64_out(char* value, FILE* out_file);
char* x86_64_add(char* r1, char* r2, FILE* out_file);
char* x86_64_sub(char* r1, char* r2, FILE* out_file);
char* x86_64_mul(char* r1, char* r2, FILE* out_file);
char* x86_64_div(char* r1, char* r2, FILE* out_file);

int32_t GenLoadX86_64(GenFuncTable *func)
{
    func->f_load = &x86_64_load;
    func->f_out  = &x86_64_out;
    func->f_add  = &x86_64_add;
    func->f_sub  = &x86_64_sub;
    func->f_mul  = &x86_64_mul;
    func->f_div  = &x86_64_div;
    return Success;
}

char* x86_64_load(int32_t value, FILE* out_file)
{
    //todo:
    return "x";
}
char* x86_64_out(char* value, FILE* out_file)
{
    //todo:
    return "x";
}
char* x86_64_add(char* r1, char* r2, FILE* out_file)
{
    //todo:
    return "x";
}
char* x86_64_sub(char* r1, char* r2, FILE* out_file)
{
    //todo:
    return "x";
}
char* x86_64_mul(char* r1, char* r2, FILE* out_file)
{
    //todo:
    return "x";
}
char* x86_64_div(char* r1, char* r2, FILE* out_file)
{
    //todo:
    return "x";
}
