/*
 * Copyright (c) 2020/07 gravieb <truongptk30a3@gmail.com>
 * This file is released under the GPLv3
 */

#include <stdlib.h>
#include <stdbool.h>
#include "symtable.h"

#ifndef _PARSE_INTERNAL_H_
#define _PARSE_INTERNAL_H_

typedef struct StringMap {
    char id[MAX_IDENT_LEN];
    char* label;
} StringMap;

typedef struct ParseParameter{
    bool avail;
    bool is_interpret;
    void *lex_prm;
    void *gen_prm;
    SymbolTable symbol_table;
    Token cur_token;
    int var_level;

    StringMap var_map[MAX_IDENT_CNT];
    int var_map_pos;
} ParseParameter;

#endif
