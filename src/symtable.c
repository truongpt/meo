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
    return Success;
}

int32_t symtable_add(SymbolTable* st, char* symbol)
{
    if (st->cur_pos >= NSYMBOLS-1) {
        return SymbolTableOver;
    }

    st->cur_pos++;
    int size_sym = strlen(symbol);
    st->data[st->cur_pos].name = (char*)malloc(size_sym+1);
    memcpy(st->data[st->cur_pos].name , symbol, size_sym);
    st->data[st->cur_pos].name[size_sym] = '\0';

    return Success;
}

int32_t symtable_remove(SymbolTable* st, char* symbol)
{
    // todo
    return Success;
}

int32_t symtable_find(SymbolTable* st, char* symbol)
{
    if (NULL == symbol) {
        return -1;
    }

    for (int i = 0; i <= st->cur_pos; i++) {
        if (*(st->data[i].name) == *symbol &&
            strlen(st->data[i].name) == strlen(symbol) &&
            !strncmp(st->data[i].name, symbol, strlen(symbol))) {
            return i;
        }
    }

    return -1;
}

int32_t symtable_get_value(SymbolTable* st, char* symbol)
{
    int32_t idx = symtable_find(st, symbol);
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

int32_t symtable_set_type(SymbolTable* st, char* symbol, int32_t type)
{
    int32_t idx = symtable_find(st, symbol);
    if (-1 == idx) {
        MLOG(CLGT,"Not found the symbol %s\n", symbol);
        return -1;
    }
    st->data[idx].type = type;
    return 0;
}

int32_t symtable_set_value(SymbolTable* st, char* symbol, int32_t value)
{
    int32_t idx = symtable_find(st, symbol);
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

