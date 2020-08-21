/*
 * Copyright (c) 2018/12 truong <truongptk30a3@gmail.com>
 * This file is released under the GPLv3
 */

#include <stdio.h>
#include <string>
#include <iostream>
#include <fstream>
#include "ast.h"
#include "gen.h"
#include "parse.h"
#include "error_code.h"

#include "common.h"
#include "catch.hh"

using namespace std;

TEST_CASE("ast test")
{
    create_folder("data");
    Token T = {TokenNumber, 10};
    AstNode* node = ast_create_leaf(T);
    REQUIRE(NULL != node);
    REQUIRE(10 == node->value);
}

TEST_CASE("ast test operator +")
{

    Token T = {TokenNumber, 10};
    AstNode* node = ast_create_leaf(T);
    REQUIRE(NULL != node);
    REQUIRE(10 == node->value);
    REQUIRE(AstNumber == node->type);

    Token T1 = {TokenNumber, 5};
    AstNode* node1 = ast_create_leaf(T1);
    REQUIRE(NULL != node1);
    REQUIRE(5 == node1->value);
    REQUIRE(AstNumber == node1->type);

    Token T2 = {TokenPlus, -1};
    AstNode* node2 = ast_create_node(T2, node, node1);
    REQUIRE(NULL != node2);
    REQUIRE(AstPlus == node2->type);
    // todo
    // REQUIRE(15 == ast_interpret(NULL, node2));
}

TEST_CASE("ast test operator -")
{
    void* gen_prm = NULL;
    REQUIRE(Success == GenCreate());
    REQUIRE(Success == GenOpen(&gen_prm, GenX86_64, (char*)"data/out1"));

    int32_t mock_lex_prm;
    void* parse_prm = NULL;
    REQUIRE(Success == ParseCreate());
    REQUIRE(Success == ParseOpen(&parse_prm, (void*)&mock_lex_prm, gen_prm, false));

    Token T = {TokenNumber, 10};
    AstNode* node = ast_create_leaf(T);
    REQUIRE(NULL != node);
    REQUIRE(10 == node->value);
    REQUIRE(AstNumber == node->type);

    Token T1 = {TokenNumber, 5};
    AstNode* node1 = ast_create_leaf(T1);
    REQUIRE(NULL != node1);
    REQUIRE(5 == node1->value);
    REQUIRE(AstNumber == node1->type);

    Token T2 = {TokenMinus, -1};
    AstNode* node2 = ast_create_node(T2, node, node1);
    REQUIRE(NULL != node2);
    REQUIRE(AstMinus == node2->type);
    ast_gen(parse_prm, node2);

    ParseClose(parse_prm);
    ParseDestroy();
}

