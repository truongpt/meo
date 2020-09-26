/*
 * Copyright (c) 2020/08 gravieb <truongptk30a3@gmail.com>
 * This file is released under the GPLv3
 */

/*
 * Symtable for quick simple version. poor performance.
 * It will be replace by hashtable version later.
 */
#include "log.h"
#include "symtable.h"
#include <string.h>
#include <stdlib.h>

int32_t symtable_init(SymbolTable* st)
{
    memset((void*)st, 0x00, sizeof(SymbolTable));
    st->cur_pos = -1;
    st->cur_id = 0;
    return Success;
}

int32_t symtable_add(SymbolTable* st, char* symbol, int level)
{
    if (st->cur_pos >= NSYMBOLS-1) {
        return SymbolTableOver;
    }

    if (-1 != symtable_find(st, symbol, level)) {
        return VariableDeclared;
    }

    MLOG(TRACE,"Add to symbol table %s level %d\n", symbol, level);
    st->cur_pos++;
    int size_sym = strlen(symbol);
    memcpy(st->data[st->cur_pos].name , symbol, size_sym);
    st->data[st->cur_pos].level = level;
    st->data[st->cur_pos].name[size_sym] = '\0';
    st->data[st->cur_pos].id = st->cur_id++;
    return Success;
}

int32_t symtable_remove(SymbolTable* st, char* symbol)
{
    // todo
    return Success;
}

int32_t symtable_find(SymbolTable* st, char* symbol, int level)
{
    if (NULL == symbol) {
        return -1;
    }

    for (int i = 0; i <= st->cur_pos; i++) {
        if (*(st->data[i].name) == *symbol &&
            strlen(st->data[i].name) == strlen(symbol) &&
            !strncmp(st->data[i].name, symbol, strlen(symbol)) &&
            st->data[i].level == level) {
            return i;
        }
    }

    return -1;
}

int32_t symtable_find_valid(SymbolTable* st, char* symbol, int level)
{
    for (int i = level; i >= 0; i--) {
        if (-1 != symtable_find(st, symbol, i)) {
            return i;
        }
    }
    return -1;
}

int32_t symtable_get_value(SymbolTable* st, char* symbol, int level)
{
    int32_t idx = symtable_find(st, symbol, level);
    if (-1 == idx) {
        MLOG(CLGT,"Not found the symbol %s\n", symbol);
        return -1;
    }

    if (SymbolInt == st->data[idx].type) {
        return st->data[idx].int_value;
    } else {
        MLOG(CLGT,"Currently, only support Int type\n");
    }
    return -1;
}

int32_t symtable_get_id(SymbolTable* st, char* symbol, int level)
{
    int32_t idx = symtable_find(st, symbol, level);
    if (-1 == idx) {
        MLOG(CLGT,"Not found the symbol %s\n", symbol);
        return -1;
    }
    return st->data[idx].id;
}

int32_t symtable_set_type(SymbolTable* st, char* symbol, int level, int32_t type)
{
    int32_t idx = symtable_find(st, symbol, level);
    if (-1 == idx) {
        MLOG(CLGT,"Not found the symbol %s\n", symbol);
        return -1;
    }
    st->data[idx].type = type;
    return 0;
}

int32_t symtable_clear_level(SymbolTable* st, int32_t level)
{
    int cnt = 0;
    for (int i = 0; i <= st->cur_pos; i++) {
        if (st->data[i].level == level) {
            cnt++;
            st->data[i].level = -1;
            memset(st->data[i].name, 0x00, sizeof(st->data[i].name));
        }
    }

    if (0 == cnt) {
        MLOG(TRACE,"Not found any variable level %d to clear\n", level);
    }

    return cnt;
}

int32_t symtable_set_label(SymbolTable* st, char* symbol, int level, char* label)
{
    
    int32_t idx = symtable_find_valid(st, symbol, level);
    if (-1 == idx) {
        MLOG(CLGT,"Not found the symbol %s\n", symbol);
        return -1;
    }
    st->data[idx].label = label;
    return 0;
}

char* symtable_get_label(SymbolTable* st, char* symbol, int level)
{
    int32_t idx = symtable_find_valid(st, symbol, level);
    if (-1 == idx) {
        MLOG(CLGT,"Not found the symbol %s\n", symbol);
        return NULL;
    }
    return st->data[idx].label;
}

int32_t symtable_set_value(SymbolTable* st, char* symbol, int level, int32_t value)
{
    int32_t idx = symtable_find(st, symbol, level);
    if (-1 == idx) {
        MLOG(CLGT,"Not found the symbol %s\n", symbol);
        return -1;
    }

    if (SymbolInt == st->data[idx].type) {
        st->data[idx].int_value = value;
    } else {
        MLOG(CLGT,"Currently, only support Int type\n");
    }
    return 0;
}

