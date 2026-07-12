/*
 * Copyright (c) 2020/07 gravieb <truongptk30a3@gmail.com>
 * This file is released under the GPLv3
 */

#include <stdio.h>
#include <stdbool.h>
#include "meo.h"

#ifndef _OP_TABLE_H_
#define _OP_TABLE_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef struct OpDesc {
    enum TokenType tok;
    int32_t    ast;
    int        prec;
    bool       right_assoc;
    const char name[4];
} OpDesc;

extern const OpDesc op_table[];
extern const int op_table_count;

OpDesc* op_find(enum TokenType tok);
const char* op_tok2str(int tok);

#ifdef __cplusplus
}
#endif

#endif /* _OP_TABLE_H_ */
