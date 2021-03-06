/*
 * Copyright (c) 2020/07 gravieb <truongptk30a3@gmail.com>
 * This file is released under the GPLv3
 */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#ifndef _MEO_H_
#define _MEO_H_

#define MAX_IDENT_LEN 32
#define MAX_STR_LEN   100 // todo: need dynamic
#define MAX_IDENT_CNT 1000
#define MAX_LABEL_LEN   10

typedef struct Token {
    int32_t tok;
    union {
        int32_t value;
        struct {
            char id_str[MAX_IDENT_LEN];
            bool left_value;
        };
        char str[MAX_STR_LEN];
    };
    int32_t var_id;
} Token;

enum TokenType{
    TokenEof,

    // operator
    TokenPlus,
    TokenMinus,
    TokenMul,
    TokenStar = TokenMul, // for pointer
    TokenDiv,
    TokenEQ,
    TokenNE,
    TokenLT,
    TokenLE,
    TokenGT,
    TokenGE,

    // Bitwise
    TokenBitAnd,
    TokenDereference = TokenBitAnd, // for dereference of pointer
    TokenBitOr,
    TokenBitXor,

    // Logical
    TokenAnd,
    TokenOr,

    // loop
    TokenIf,
    TokenElse,
    TokenWhile,
    TokenDo,
    TokenFor,

    // data type
    TokenIntType,
    TokenVoidType,
    TokenLongType,

    TokenString,

    TokenNumber,
    TokenLP,
    TokenRP,
    TokenLBracket,
    TokenRBracket,
    TokenComma,
    TokenAssign,

    TokenReturn,
    TokenEoi,
    TokenSemi,

    TokenIdentifier,
};

#endif // _MEO_H_
