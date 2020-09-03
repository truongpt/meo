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
int32_t MockGetReturnValue();

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

    vector<Token> token_test = {
            {TokenIntType,   -1},
            {TokenIdentifier, -1},
            {TokenLP,         -1},
            {TokenRP,         -1},
            {TokenLBracket,   -1},
            {TokenReturn,     -1},
            {TokenLP,         -1},
            {TokenNumber,      1},
            {TokenPlus,       -1},
            {TokenNumber,      2},
            {TokenRP,         -1},
            {TokenSemi,       -1},
            {TokenRBracket,   -1},
            {TokenEoi,        -1}
    };
    token_test[1].id_str = strdup("main");
    MockLexCreate(token_test);

    void* gen_prm = NULL;
    REQUIRE(Success == GenCreate());
    REQUIRE(Success == GenOpen(&gen_prm, GenX86_64, (char*)"data/out2"));

    void* parse_prm = NULL;
    REQUIRE(Success == ParseCreate());
    REQUIRE(Success == ParseOpen(&parse_prm, (void*)&mock_lex_prm, gen_prm, false));
    
    REQUIRE(Success == ParseProc(parse_prm));
    REQUIRE(MockGetReturnValue() == (1+2));

    ParseClose(parse_prm);
    ParseDestroy();

    MockLexDestroy();
}

TEST_CASE("parse test plus token: (2*3);")
{

    int32_t mock_lex_prm;
    vector<Token> token_test = {
            {TokenIntType,   -1},
            {TokenIdentifier, -1},
            {TokenLP,         -1},
            {TokenRP,         -1},
            {TokenLBracket,   -1},
            {TokenReturn,     -1},
            {TokenLP,         -1},
            {TokenNumber,      2},
            {TokenMul,        -1},
            {TokenNumber,      3},
            {TokenRP,         -1},
            {TokenSemi,       -1},
            {TokenRBracket,   -1},
            {TokenEoi,        -1}
    };
    token_test[1].id_str = strdup("main");
    MockLexCreate(token_test);

    void* gen_prm = NULL;
    REQUIRE(Success == GenCreate());
    REQUIRE(Success == GenOpen(&gen_prm, GenX86_64, (char*)"data/out2"));

    void* parse_prm = NULL;
    REQUIRE(Success == ParseCreate());
    REQUIRE(Success == ParseOpen(&parse_prm, (void*)&mock_lex_prm, gen_prm, false));

    REQUIRE(Success == ParseProc(parse_prm));

    REQUIRE(MockGetReturnValue() == (2*3));

    ParseClose(parse_prm);
    ParseDestroy();

    MockLexDestroy();
}

TEST_CASE("parse test plus token: (1+2*3+4);")
{

    int32_t mock_lex_prm;
    vector<Token> token_test = {
            {TokenIntType,   -1},
            {TokenIdentifier, -1},
            {TokenLP,         -1},
            {TokenRP,         -1},
            {TokenLBracket,   -1},
            {TokenReturn,     -1},
            {TokenLP,         -1},
            {TokenNumber,      1},
            {TokenPlus,       -1},
            {TokenNumber,      2},
            {TokenMul,        -1},
            {TokenNumber,      3},
            {TokenPlus,       -1},
            {TokenNumber,      4},
            {TokenRP,         -1},
            {TokenSemi,       -1},
            {TokenRBracket,   -1},
            {TokenEoi,        -1}
    };
    token_test[1].id_str = strdup("main");
    MockLexCreate(token_test);

    void* gen_prm = NULL;
    REQUIRE(Success == GenCreate());
    REQUIRE(Success == GenOpen(&gen_prm, GenX86_64, (char*)"data/out3"));

    void* parse_prm = NULL;
    REQUIRE(Success == ParseCreate());
    REQUIRE(Success == ParseOpen(&parse_prm, (void*)&mock_lex_prm, gen_prm, false));

    REQUIRE(Success == ParseProc(parse_prm));

    REQUIRE(MockGetReturnValue() == (1+2*3+4));

    ParseClose(parse_prm);
    ParseDestroy();

    MockLexDestroy();
}

TEST_CASE("parse test plus token: (1+2)*(3+4);")
{

    int32_t mock_lex_prm;

    vector<Token> token_test = {
            {TokenIntType,   -1},
            {TokenIdentifier, -1},
            {TokenLP,         -1},
            {TokenRP,         -1},
            {TokenLBracket,   -1},
            {TokenReturn,     -1},
            {TokenLP,         -1},
            {TokenNumber,      1},
            {TokenPlus,       -1},
            {TokenNumber,      2},
            {TokenRP,         -1},
            {TokenMul,        -1},
            {TokenLP,         -1},
            {TokenNumber,      3},
            {TokenPlus,       -1},
            {TokenNumber,      4},
            {TokenRP,         -1},
            {TokenSemi,       -1},
            {TokenRBracket,   -1},
            {TokenEoi,        -1}
    };
    token_test[1].id_str = strdup("main");
    MockLexCreate(token_test);

    void* gen_prm = NULL;
    REQUIRE(Success == GenCreate());
    REQUIRE(Success == GenOpen(&gen_prm, GenX86_64, (char*)"data/out4"));

    void* parse_prm = NULL;
    REQUIRE(Success == ParseCreate());
    REQUIRE(Success == ParseOpen(&parse_prm, (void*)&mock_lex_prm, gen_prm, false));

    REQUIRE(Success == ParseProc(parse_prm));
    REQUIRE(MockGetReturnValue() == ((1+2)*(3+4)));

    ParseClose(parse_prm);
    ParseDestroy();

    MockLexDestroy();
}

TEST_CASE("parse test pattern: int xyz;")
{
    int32_t mock_lex_prm;

    vector<Token> token_test = {
            {TokenIntType,   -1},
            {TokenIdentifier, -1},
            {TokenLP,         -1},
            {TokenRP,         -1},
            {TokenLBracket,   -1},
            {TokenIntType,    -1},
            {TokenIdentifier, -1},
            {TokenSemi,       -1},
            {TokenRBracket,   -1},
            {TokenEoi,        -1}
    };

    token_test[1].id_str = strdup("main");
    token_test[6].id_str = strdup("xyz");

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


TEST_CASE("parse test return token: return (1+2);")
{
    int32_t mock_lex_prm;
    vector<Token> token_test = {
            {TokenIntType,   -1},
            {TokenIdentifier, -1},
            {TokenLP,         -1},
            {TokenRP,         -1},
            {TokenLBracket,   -1},
            {TokenReturn,     -1},
            {TokenLP,         -1},
            {TokenNumber,      1},
            {TokenPlus,       -1},
            {TokenNumber,      2},
            {TokenRP,         -1},
            {TokenSemi,       -1},
            {TokenRBracket,   -1},
            {TokenEoi,        -1}
    };

    token_test[1].id_str = strdup("main");

    MockLexCreate(token_test);

    void* gen_prm = NULL;
    REQUIRE(Success == GenCreate());
    REQUIRE(Success == GenOpen(&gen_prm, GenX86_64, (char*)"data/out2"));

    void* parse_prm = NULL;
    REQUIRE(Success == ParseCreate());
    REQUIRE(Success == ParseOpen(&parse_prm, (void*)&mock_lex_prm, gen_prm, false));
    
    REQUIRE(Success == ParseProc(parse_prm));
    // todo: consider method to confirm

    ParseClose(parse_prm);
    ParseDestroy();

    MockLexDestroy();
}


TEST_CASE("parse test pattern: int a; a = 10; return a;")
{
    int32_t mock_lex_prm;

    vector<Token> token_test = {
        {TokenIntType,   -1},
        {TokenIdentifier, -1},
        {TokenLP,         -1},
        {TokenRP,         -1},
        {TokenLBracket,   -1},
        {TokenIntType,    -1},
        {TokenIdentifier, -1},
        {TokenSemi,       -1},
        {TokenIdentifier, -1},
        {TokenAssign,     -1},
        {TokenNumber,     10},
        {TokenSemi,       -1},
        {TokenReturn,     -1},
        {TokenIdentifier, -1},
        {TokenSemi,       -1},
        {TokenRBracket,   -1},
        {TokenEoi,        -1}
    };
    token_test[1].id_str = strdup("main");
    token_test[6].id_str = strdup("abc");
    token_test[8].id_str = strdup("abc");
    token_test[13].id_str = strdup("abc");

    MockLexCreate(token_test);
    void* gen_prm = NULL;
    REQUIRE(Success == GenCreate());
    REQUIRE(Success == GenOpen(&gen_prm, GenX86_64, (char*)"data/out6"));

    void* parse_prm = NULL;
    REQUIRE(Success == ParseCreate());
    REQUIRE(Success == ParseOpen(&parse_prm, (void*)&mock_lex_prm, gen_prm, false));

    REQUIRE(Success == ParseProc(parse_prm));
    REQUIRE(MockGetReturnValue() == 10);

    ParseClose(parse_prm);
    ParseDestroy();

    MockLexDestroy();
}

TEST_CASE("parse test pattern: a+b;")
{
    int32_t mock_lex_prm;
    vector<Token> token_test = {
        {TokenIntType,   -1},
        {TokenIdentifier,-1},
        {TokenLP,        -1},
        {TokenRP,        -1},
        {TokenLBracket,  -1},
        {TokenIntType,   -1},
        {TokenIdentifier,-1}, //1
        {TokenSemi,      -1}, // int a;
        {TokenIntType,   -1},
        {TokenIdentifier,-1}, //4
        {TokenSemi,      -1}, // int b;/
        {TokenIdentifier,-1}, //6: a = 1
        {TokenAssign,    -1},
        {TokenNumber,     1},
        {TokenSemi,      -1},
        {TokenIdentifier,-1}, //10: b = 2
        {TokenAssign,    -1},
        {TokenNumber,     2},
        {TokenSemi,      -1},
        {TokenReturn,    -1}, // return a+b;
        {TokenIdentifier,-1}, //15 a
        {TokenPlus,      -1},
        {TokenIdentifier,-1}, // 17 b
        {TokenSemi,      -1},
        {TokenRBracket,  -1},
        {TokenEoi,       -1}
    };

    token_test[1].id_str = strdup("main");
    token_test[5+1].id_str = strdup("a");
    token_test[5+4].id_str = strdup("b");
    token_test[5+6].id_str = strdup("a");
    token_test[5+10].id_str = strdup("b");
    token_test[5+15].id_str = strdup("a");
    token_test[5+17].id_str = strdup("b");

    MockLexCreate(token_test);
    void* gen_prm = NULL;
    REQUIRE(Success == GenCreate());
    REQUIRE(Success == GenOpen(&gen_prm, GenX86_64, (char*)"data/out7"));

    void* parse_prm = NULL;
    REQUIRE(Success == ParseCreate());
    REQUIRE(Success == ParseOpen(&parse_prm, (void*)&mock_lex_prm, gen_prm, false));

    REQUIRE(Success == ParseProc(parse_prm));
    REQUIRE(MockGetReturnValue() == (1+2));

    ParseClose(parse_prm);
    ParseDestroy();

    MockLexDestroy();
}
