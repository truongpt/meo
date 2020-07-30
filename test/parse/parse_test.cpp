/*
 * Copyright (c) 2020/07 gravieb <truongptk30a3@gmail.com>
 * This file is released under the GPLv3
 */

#include <stdio.h>
#include <string>
#include <iostream>
#include <fstream>
#include "lex.h"
#include "parse.h"
#include "error_code.h"

#include "common.h"
#include "catch.hh"

using namespace std;
int32_t MockLexCreate(vector<Token> tok_array);
int32_t MockLexDestroy();

TEST_CASE("parse test get single resource")
{
    create_folder("data");
    std::ofstream outfile ("data/testp0");
    outfile << "0" << std::endl;
    outfile.close();

    void* lex_prm = NULL;
    REQUIRE(Success == LexCreate());
    REQUIRE(Success == LexOpen(&lex_prm, (char*)"data/testp0"));

    void* prm = NULL;
    REQUIRE(Success == ParseCreate());
    REQUIRE(Success == ParseOpen(&prm, lex_prm));

    REQUIRE(Success == ParseClose(prm));
    REQUIRE(Success == ParseDestroy());
}

TEST_CASE("parse test get multi resource")
{
    std::ofstream outfile ("data/testp1");
    outfile << "0" << std::endl;
    outfile.close();

    void* lex_prm = NULL;
    REQUIRE(Success == LexCreate());
    REQUIRE(Success == LexOpen(&lex_prm, (char*)"data/testp1"));

    int max_rsc = 10;
    vector<void*> prm = vector<void*>(10,NULL);
    REQUIRE(Success == ParseCreate());
    for (auto& p : prm) {
        REQUIRE(Success == ParseOpen(&p, lex_prm));
        REQUIRE(NULL != p);
    }

    REQUIRE(InParameterInvalid == ParseOpen(&prm[0], lex_prm));

    void* a_prm = NULL;
    REQUIRE(ParseLimitResource == ParseOpen(&a_prm, lex_prm));
    REQUIRE(NULL == a_prm);

    for (auto& p : prm) {
        REQUIRE(Success == ParseClose(p));
    }

    REQUIRE(Success == ParseDestroy());
}

TEST_CASE("parse test plus token: (1+2);")
{

    int32_t mock_lex_prm;

    MockLexCreate(vector<Token> {
            {TokenLP,    -1},
            {TokenNumber, 1},
            {TokenPlus,  -1},
            {TokenNumber, 2},
            {TokenRP,    -1},
            {TokenSemi,  -1},
            {TokenEoi,   -1}
        });

    void* parse_prm = NULL;
    REQUIRE(Success == ParseCreate());
    REQUIRE(Success == ParseOpen(&parse_prm, (void*)&mock_lex_prm));
    
    int32_t res = -1;
    REQUIRE(Success == ParseProc(parse_prm, &res));
    REQUIRE(res == (1+2));

    ParseClose(parse_prm);
    ParseDestroy();

    MockLexDestroy();
}

TEST_CASE("parse test plus token: (2*3);")
{

    int32_t mock_lex_prm;

    MockLexCreate(vector<Token> {
            {TokenLP,    -1},
            {TokenNumber, 2},
            {TokenMul,   -1},
            {TokenNumber, 3},
            {TokenRP,    -1},
            {TokenSemi,  -1},
            {TokenEoi,   -1}
        });

    void* parse_prm = NULL;
    REQUIRE(Success == ParseCreate());
    REQUIRE(Success == ParseOpen(&parse_prm, (void*)&mock_lex_prm));
    
    int32_t res = -1;
    REQUIRE(Success == ParseProc(parse_prm, &res));
    REQUIRE(res == (2*3));

    ParseClose(parse_prm);
    ParseDestroy();

    MockLexDestroy();
}
