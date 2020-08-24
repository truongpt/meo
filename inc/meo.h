/*
 * Copyright (c) 2020/07 gravieb <truongptk30a3@gmail.com>
 * This file is released under the GPLv3
 */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#ifndef _MEO_H_
#define _MEO_H_

typedef struct Token {
    int32_t tok;
    union {
        int32_t value;
        struct {
            char* id_str;
            bool left_value;
        };
    };
} Token;

enum TokenType{
    TokenEof,

    // operator
    TokenPlus,
    TokenMinus,
    TokenMul,
    TokenDiv,
    TokenEQ,
    TokenNE,
    TokenLT,
    TokenLE,
    TokenGT,
    TokenGE,

    // 
    TokenIf,
    TokenElse,
    TokenWhile,
    TokenDo,
    TokenFor,

    // data type
    TokenIntType,
    TokenVoidType,
    TokenLongType,

    TokenNumber,
    TokenLP,
    TokenRP,
    TokenLBracket,
    TokenRBracket,
    TokenAssign,

    TokenPrint,
    TokenReturn,
    TokenEoi,
    TokenSemi,

    TokenIdentifier,
};

#endif // _MEO_H_
