/*
 * Copyright (c) 2020/07 gravieb <truongptk30a3@gmail.com>
 * This file is released under the GPLv3
 */

#include "op_table.h"
#include "ast.h"

const OpDesc op_table[] = {
    // logical (lowest precedence)
    { TokenOr,      AstOr,      1, false, "||" },
    { TokenAnd,     AstAnd,     2, false, "&&" },
    // bitwise
    { TokenBitOr,   AstBitOr,   3, false, "|" },
    { TokenBitXor,  AstBitXor,  4, false, "^" },
    { TokenBitAnd,  AstBitAnd,  5, false, "&" },
    // equality
    { TokenEQ,      AstEQ,      6, false, "==" },
    { TokenNE,      AstNE,      6, false, "!=" },
    // relational
    { TokenLT,      AstLT,      7, false, "<" },
    { TokenLE,      AstLE,      7, false, "<=" },
    { TokenGT,      AstGT,      7, false, ">" },
    { TokenGE,      AstGE,      7, false, ">=" },
    // additive
    { TokenPlus,    AstPlus,    8, false, "+" },
    { TokenMinus,   AstMinus,   8, false, "-" },
    // multiplicative (highest precedence)
    { TokenMul,     AstMul,     9, false, "*" },
    { TokenDiv,     AstDiv,     9, false, "/" },
};

const int op_table_count = sizeof(op_table) / sizeof(op_table[0]);

OpDesc* op_find(enum TokenType tok)
{
    for (int i = 0; i < op_table_count; i++) {
        if (op_table[i].tok == tok) {
            return (OpDesc*)&op_table[i];
        }
    }
    return NULL;
}

static const char* g_token_names[] = {
    [TokenEof]       = "TokenEof",
    [TokenPlus]      = "TokenPlus",
    [TokenMinus]     = "TokenMinus",
    [TokenMul]       = "TokenMul",
    [TokenDiv]       = "TokenDiv",
    [TokenEQ]        = "TokenEQ",
    [TokenNE]        = "TokenNE",
    [TokenLT]        = "TokenLT",
    [TokenLE]        = "TokenLE",
    [TokenGT]        = "TokenGT",
    [TokenGE]        = "TokenGE",
    [TokenBitAnd]    = "TokenBitAnd",
    [TokenBitOr]     = "TokenBitOr",
    [TokenBitXor]    = "TokenBitXor",
    [TokenAnd]       = "TokenAnd",
    [TokenOr]        = "TokenOr",
    [TokenIf]        = "TokenIf",
    [TokenElse]      = "TokenElse",
    [TokenWhile]     = "TokenWhile",
    [TokenDo]        = "TokenDo",
    [TokenFor]       = "TokenFor",
    [TokenIntType]   = "TokenIntType",
    [TokenVoidType]  = "TokenVoidType",
    [TokenLongType]  = "TokenLongType",
    [TokenString]    = "TokenString",
    [TokenNumber]    = "TokenNumber",
    [TokenLP]        = "TokenLP",
    [TokenRP]        = "TokenRP",
    [TokenLBracket]  = "TokenLBracket",
    [TokenRBracket]  = "TokenRBracket",
    [TokenComma]     = "TokenComma",
    [TokenAssign]    = "TokenAssign",
    [TokenReturn]    = "TokenReturn",
    [TokenEoi]       = "TokenEoi",
    [TokenSemi]      = "TokenSemi",
    [TokenIdentifier]= "TokenIdentifier",
};

const char* op_tok2str(int tok)
{
    if (tok >= 0 && tok < (int)(sizeof(g_token_names) / sizeof(g_token_names[0]))) {
        if (g_token_names[tok]) {
            return g_token_names[tok];
        }
    }
    return "Unknown";
}
