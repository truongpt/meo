/*
 * Copyright (c) 2018/12 truong <truongptk30a3@gmail.com>
 * This file is released under the GPLv3
 */

#include <stdio.h>
#include <string>
#include <iostream>
#include <fstream>
#include "ast.h"
#include "error_code.h"

#include "common.h"
#include "catch.hh"

using namespace std;

TEST_CASE("ast test")
{

    AstNode* node = ast_create_leaf(TokenNumber, 10);
    REQUIRE(NULL != node);
    REQUIRE(10 == node->value);
}
