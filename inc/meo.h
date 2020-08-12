/*
 * Copyright (c) 2020/07 gravieb <truongptk30a3@gmail.com>
 * This file is released under the GPLv3
 */

#include <stdio.h>
#include <stdint.h>

#ifndef _MEO_H_
#define _MEO_H_

typedef struct Token {
    int32_t tok;
    union {
        int32_t value;
        char* id_str;
    };
} Token;

enum TokenType{
    TokenEof,

    // operator
    TokenPlus,
    TokenMinus,
    TokenMul,
    TokenDiv,
    // data type
    TokenIntType,
    TokenVoidType,
    TokenLongType,

    TokenNumber,
    TokenLP,
    TokenRP,
    TokenLBracket,
    TokenRBracket,
    TokenEqual,

    TokenPrint,
    TokenReturn,
    TokenEoi,
    TokenSemi,

    TokenIdentifier,
};

#endif // _MEO_H_
