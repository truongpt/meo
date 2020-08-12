/*
 * Copyright (c) 2020/08 gravieb <truongptk30a3@gmail.com>
 * This file is released under the GPLv3
 */

/*
 * Symtable for quick simple version. poor performance.
 * It will be replace by hashtable version later.
 */
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
        return InParameterInvalid;
    }

    for (int i = 0; i <= st->cur_pos; i++) {
        if (*(st->data[i].name) == *symbol &&
            strlen(st->data[i].name) == strlen(symbol) &&
            !strncmp(st->data[i].name, symbol, strlen(symbol))) {
            return Success;
        }
    }

    return SymbolNotFound;
}
