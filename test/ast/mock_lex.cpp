/*
 * Copyright (c) 2020/07 gravieb <truongptk30a3@gmail.com>
 * This file is released under the GPLv3
 */

#include "lex.h"
#include "error_code.h"
#include <vector>
#include <stdlib.h>

using namespace std;
vector<Token> g_tok_array;
int32_t tok_idx = 0;

int32_t MockLexCreate(vector<Token> tok_array)
{
    g_tok_array = tok_array;
    //g_tok_array(tok_array.begin(), tok_array.end());
    tok_idx = 0;
    return Success;
}

int32_t MockLexDestroy()
{
    g_tok_array.clear();
    tok_idx = 0;
    return Success;
}

int32_t LexCreate(void)
{
    return Success;
}
int32_t LexDestroy(void)
{
    return Success;
    
}

int32_t LexOpen(void** prm, const char* file)
{
    *prm = malloc(1);
    return Success;
}
int32_t LexClose(void* prm)
{
    free(prm);
    return Success;
}

int32_t LexProc(void* prm, Token *t)
{
    if (tok_idx >= g_tok_array.size()) {
        return -1;
    }
    *t = g_tok_array[tok_idx++];
    return Success;
}

int32_t LexGetLine(void* prm)
{
    return Success;
}
