/*
 * Copyright (c) 2020/08 gravieb <truongptk30a3@gmail.com>
 * This file is released under the GPLv3
 */

#ifndef _SYM_TABLE_H_
#define _SYM_TABLE_H_

#include <stdio.h>
#include <stdint.h>
#include "meo.h"
#include "error_code.h"

#define NSYMBOLS    1024

typedef struct SymbolData {
    char name[MAX_IDENT_LEN];
    int32_t type;
    int32_t pointer_level;
    int32_t level;
    int32_t id;
    char* label;
    union {
        int32_t int_value;
        float float_value;
    };
} SymbolData;

enum SymbolType {
    SymbolInt,
    SymbolFloat
};

typedef struct SymbolTable {
    SymbolData data[NSYMBOLS];
    int32_t cur_pos;
    int32_t cur_id;
} SymbolTable;

#ifdef __cplusplus
extern "C" {
#endif

int32_t symtable_init(SymbolTable* st);
int32_t symtable_add(SymbolTable* st, char* symbol, int level);
int32_t symtable_remove(SymbolTable* st, char* symbol);
int32_t symtable_find(SymbolTable* st, char* symbol, int level);
int32_t symtable_find_valid(SymbolTable* st, char* symbol, int level);
int32_t symtable_get_value(SymbolTable* st, char* symbol, int level);
int32_t symtable_get_id(SymbolTable* st, char* symbol, int level);
int32_t symtable_set_type(SymbolTable* st, char* symbol, int level, int32_t type);
int32_t symtable_set_label(SymbolTable* st, char* symbol, int level, char* lable);
char* symtable_get_label(SymbolTable* st, char* symbol, int level);
int32_t symtable_set_value(SymbolTable* st, char* symbol, int level, int32_t value);
int32_t symtable_clear_level(SymbolTable* st, int32_t level);
int32_t symtable_set_pointer_level(SymbolTable* st, char* symbol, int level, int32_t pointer_level);

#ifdef __cplusplus
}
#endif

#endif
