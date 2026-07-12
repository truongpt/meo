/*
 * Copyright (c) 2020/08 gravieb <truongptk30a3@gmail.com>
 * This file is released under the GPLv3
 */

#ifndef _CODEGEN_H_
#define _CODEGEN_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "ast.h"
#include "parse_internal.h"

void codegen_gen(ParseParameter* parse_prm, AstNode* node);
void codegen_gen_all(ParseParameter* parse_prm);
void codegen_map_init(ParseParameter* parse_prm);

#ifdef __cplusplus
}
#endif

#endif /* _CODEGEN_H_ */
