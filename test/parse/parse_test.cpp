/*
 * Copyright (c) 2020/07 gravieb <truongptk30a3@gmail.com>
 * This file is released under the GPLv3
 */

#include <stdio.h>
#include <string.h>
#include <string>
#include <iostream>
#include <fstream>
#include "lex.h"
#include "gen.h"
#include "parse.h"
#include "error_code.h"

#include "common.h"
#include "catch.hh"

using namespace std;
int32_t MockLexCreate(vector<Token> tok_array);
int32_t MockLexDestroy();
int32_t MockGetLatest();

TEST_CASE("parse test get single resource")
{
    create_folder("data");
    std::ofstream outfile ("data/testp0");
    outfile << "0" << std::endl;
    outfile.close();

    void* lex_prm = NULL;
    REQUIRE(Success == LexCreate());
    REQUIRE(Success == LexOpen(&lex_prm, (char*)"data/in0"));

    void* gen_prm = NULL;
    REQUIRE(Success == GenCreate());
    REQUIRE(Success == GenOpen(&gen_prm, GenX86_64, (char*)"data/out0"));

    void* prm = NULL;
    REQUIRE(Success == ParseCreate());
    REQUIRE(Success == ParseOpen(&prm, lex_prm, gen_prm, false));

    REQUIRE(Success == ParseClose(prm));
    REQUIRE(Success == ParseDestroy());

    REQUIRE(Success == LexClose(lex_prm));
    REQUIRE(Success == LexDestroy());

    REQUIRE(Success == GenClose(gen_prm));
    REQUIRE(Success == GenDestroy());
}

TEST_CASE("parse test get multi resource")
{
    std::ofstream outfile ("data/testp1");
    outfile << "0" << std::endl;
    outfile.close();

    void* lex_prm = NULL;
    REQUIRE(Success == LexCreate());
    REQUIRE(Success == LexOpen(&lex_prm, (char*)"data/in1"));

    void* gen_prm = NULL;
    REQUIRE(Success == GenCreate());
    REQUIRE(Success == GenOpen(&gen_prm, GenX86_64, (char*)"data/out1"));

    int max_rsc = 10;
    vector<void*> prm = vector<void*>(10,NULL);
    REQUIRE(Success == ParseCreate());
    for (auto& p : prm) {
        REQUIRE(Success == ParseOpen(&p, lex_prm, gen_prm, false));
        REQUIRE(NULL != p);
    }

    REQUIRE(InParameterInvalid == ParseOpen(&prm[0], lex_prm, gen_prm, false));

    void* a_prm = NULL;
    REQUIRE(ParseLimitResource == ParseOpen(&a_prm, lex_prm, gen_prm, false));
    REQUIRE(NULL == a_prm);

    for (auto& p : prm) {
        REQUIRE(Success == ParseClose(p));
    }

    REQUIRE(Success == ParseDestroy());

    REQUIRE(Success == LexClose(lex_prm));
    REQUIRE(Success == LexDestroy());

    REQUIRE(Success == GenClose(gen_prm));
    REQUIRE(Success == GenDestroy());
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

    void* gen_prm = NULL;
    REQUIRE(Success == GenCreate());
    REQUIRE(Success == GenOpen(&gen_prm, GenX86_64, (char*)"data/out2"));

    void* parse_prm = NULL;
    REQUIRE(Success == ParseCreate());
    REQUIRE(Success == ParseOpen(&parse_prm, (void*)&mock_lex_prm, gen_prm, false));
    
    REQUIRE(Success == ParseProc(parse_prm));
    REQUIRE(MockGetLatest() == (1+2));

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

    void* gen_prm = NULL;
    REQUIRE(Success == GenCreate());
    REQUIRE(Success == GenOpen(&gen_prm, GenX86_64, (char*)"data/out2"));

    void* parse_prm = NULL;
    REQUIRE(Success == ParseCreate());
    REQUIRE(Success == ParseOpen(&parse_prm, (void*)&mock_lex_prm, gen_prm, false));

    REQUIRE(Success == ParseProc(parse_prm));

    REQUIRE(MockGetLatest() == (2*3));

    ParseClose(parse_prm);
    ParseDestroy();

    MockLexDestroy();
}

TEST_CASE("parse test plus token: (1+2*3+4);")
{

    int32_t mock_lex_prm;

    MockLexCreate(vector<Token> {
            {TokenLP,    -1},
            {TokenNumber, 1},
            {TokenPlus,  -1},
            {TokenNumber, 2},
            {TokenMul,   -1},
            {TokenNumber, 3},
            {TokenPlus,  -1},
            {TokenNumber, 4},
            {TokenRP,    -1},
            {TokenSemi,  -1},
            {TokenEoi,   -1}
        });

    void* gen_prm = NULL;
    REQUIRE(Success == GenCreate());
    REQUIRE(Success == GenOpen(&gen_prm, GenX86_64, (char*)"data/out3"));

    void* parse_prm = NULL;
    REQUIRE(Success == ParseCreate());
    REQUIRE(Success == ParseOpen(&parse_prm, (void*)&mock_lex_prm, gen_prm, false));

    REQUIRE(Success == ParseProc(parse_prm));

    REQUIRE(MockGetLatest() == (1+2*3+4));

    ParseClose(parse_prm);
    ParseDestroy();

    MockLexDestroy();
}

TEST_CASE("parse test plus token: (1+2)*(3+4);")
{

    int32_t mock_lex_prm;

    MockLexCreate(vector<Token> {
            {TokenLP,    -1},
            {TokenNumber, 1},
            {TokenPlus,  -1},
            {TokenNumber, 2},
            {TokenRP,    -1},
            {TokenMul,   -1},
            {TokenLP,    -1},
            {TokenNumber, 3},
            {TokenPlus,  -1},
            {TokenNumber, 4},
            {TokenRP,    -1},
            {TokenSemi,  -1},
            {TokenEoi,   -1}
        });

    void* gen_prm = NULL;
    REQUIRE(Success == GenCreate());
    REQUIRE(Success == GenOpen(&gen_prm, GenX86_64, (char*)"data/out4"));

    void* parse_prm = NULL;
    REQUIRE(Success == ParseCreate());
    REQUIRE(Success == ParseOpen(&parse_prm, (void*)&mock_lex_prm, gen_prm, false));

    REQUIRE(Success == ParseProc(parse_prm));
    REQUIRE(MockGetLatest() == ((1+2)*(3+4)));

    ParseClose(parse_prm);
    ParseDestroy();

    MockLexDestroy();
}

TEST_CASE("parse test pattern: int + identifier;")
{
    int32_t mock_lex_prm;

    vector<Token> token_test = {
        {TokenIntType,   -1},
        {TokenIdentifier,-1},
        {TokenSemi,      -1},
        {TokenEoi,       -1}
    };
    token_test[1].id_str = strdup("abc");

    MockLexCreate(token_test);
    void* gen_prm = NULL;
    REQUIRE(Success == GenCreate());
    REQUIRE(Success == GenOpen(&gen_prm, GenX86_64, (char*)"data/out5"));

    void* parse_prm = NULL;
    REQUIRE(Success == ParseCreate());
    REQUIRE(Success == ParseOpen(&parse_prm, (void*)&mock_lex_prm, gen_prm, false));

    REQUIRE(Success == ParseProc(parse_prm));
    // TODO check result

    ParseClose(parse_prm);
    ParseDestroy();

    MockLexDestroy();
}

TEST_CASE("parse test pattern: Identifier = 10;")
{
    int32_t mock_lex_prm;

    vector<Token> token_test = {
        {TokenIntType,   -1},
        {TokenIdentifier,-1},
        {TokenSemi,      -1},
        {TokenIdentifier,-1},
        {TokenEqual,     -1},
        {TokenNumber,    10},
        {TokenSemi,      -1},
        {TokenEoi,       -1}
    };

    token_test[1].id_str = strdup("abc");
    token_test[3].id_str = strdup("abc");

    MockLexCreate(token_test);
    void* gen_prm = NULL;
    REQUIRE(Success == GenCreate());
    REQUIRE(Success == GenOpen(&gen_prm, GenX86_64, (char*)"data/out5"));

    void* parse_prm = NULL;
    REQUIRE(Success == ParseCreate());
    REQUIRE(Success == ParseOpen(&parse_prm, (void*)&mock_lex_prm, gen_prm, false));

    REQUIRE(Success == ParseProc(parse_prm));
    // TODO check result

    ParseClose(parse_prm);
    ParseDestroy();

    MockLexDestroy();
}

