/*
 * Copyright (c) 2020/07 gravieb <truongptk30a3@gmail.com>
 * This file is released under the GPLv3
 */

#include <stdlib.h>
#include <stdbool.h>
#include "symtable.h"

#ifndef _PARSE_INTERNAL_H_
#define _PARSE_INTERNAL_H_

typedef struct ParseParameter{
    bool avail;
    bool is_interpret;
    void *lex_prm;
    void *gen_prm;
    SymbolTable symbol_table;
    Token cur_token;
    int var_level;
} ParseParameter;

#endif
