/*
 * Copyright (c) 2018/12 truong <truongptk30a3@gmail.com>
 * This file is released under the GPLv3
 */

#include <stdio.h>
#include <string>
#include <iostream>
#include <fstream>
#include "lex.h"
#include "error_code.h"

#include "common.h"
#include "catch.hh"

using namespace std;

TEST_CASE("lex test get single resource")
{
    create_folder("data");
    std::ofstream outfile ("data/xyz");
    outfile << "1" << std::endl;
    outfile.close();

    void* prm = NULL;
    REQUIRE(Success == LexCreate());

    REQUIRE(InParameterInvalid == LexOpen(&prm, NULL));

    REQUIRE(Success == LexOpen(&prm, (char*)"data/xyz"));

    REQUIRE(InParameterInvalid == LexOpen(&prm, (char*)"data/xyz"));

    REQUIRE(Success == LexClose(prm));
    REQUIRE(Success == LexDestroy());
}

TEST_CASE("lex test get multi resource")
{
    int max_rsc = 10;
    vector<void*> prm = vector<void*>(10,NULL);
    REQUIRE(Success == LexCreate());
    for (int i = 0; i < max_rsc; i++) {
        std::ofstream outfile ("data/xyz"+ to_string(i));
        outfile.close();

        REQUIRE(Success == LexOpen(&prm[i], ("data/xyz"+ to_string(i)).c_str() ));
        REQUIRE(NULL != prm[i]);
    }

    {
        std::ofstream outfile ("data/xyzk");
        outfile.close();
        REQUIRE(InParameterInvalid == LexOpen(&prm[0], (char*)"data/xyzk"));
    }

    {
        void* a_prm = NULL;
        std::ofstream outfile ("data/xyzf");
        outfile.close();
        REQUIRE(LexLimitResource == LexOpen(&a_prm, (char*)"data/xyzf"));
        REQUIRE(NULL == a_prm);
    }

    for (auto& p : prm) {
        REQUIRE(Success == LexClose(p));
    }

    REQUIRE(Success == LexDestroy());
}

TEST_CASE("lex test operator token")
{
    std::ofstream outfile ("data/test1");
    outfile << "-+*/()" << std::endl;
    outfile.close();
    vector<Token> expect = vector<Token>{
        {TokenMinus ,-1},
        {TokenPlus  ,-1},
        {TokenMul   ,-1},
        {TokenDiv   ,-1},
        {TokenLP    ,-1},
        {TokenRP    ,-1}
    };

    void* prm = NULL;
    REQUIRE(Success == LexCreate());
    REQUIRE(Success == LexOpen(&prm, (char*)"data/test1"));

    Token T;
    int i = 0;
    while(Success == LexProc(prm, &T)) {
        Token t = expect[i];
        REQUIRE(T.tok == t.tok);
        i++;
        if (TokenEof == T.tok) {
            break;
        }
    }

    REQUIRE(i == expect.size());
    REQUIRE(Success == LexClose(prm));
    REQUIRE(Success == LexDestroy());
}

TEST_CASE("lex test numeric token")
{
    std::ofstream outfile ("data/test2");
    outfile << "1 2 3 4" << std::endl;
    outfile.close();
    vector<Token> expect = vector<Token>{
        {TokenNumber ,1},
        {TokenNumber ,2},
        {TokenNumber ,3},
        {TokenNumber ,4}
    };

    void* prm = NULL;
    REQUIRE(Success == LexCreate());
    REQUIRE(Success == LexOpen(&prm, (char*)"data/test2"));

    Token T;
    int i = 0;
    while(Success == LexProc(prm, &T)) {
        Token t = expect[i];
        REQUIRE(T.tok == t.tok);
        REQUIRE(T.value == t.value);
        i++;
        if (TokenEof == T.tok) {
            break;
        }
    }

    REQUIRE(i == expect.size());
    REQUIRE(Success == LexClose(prm));
    REQUIRE(Success == LexDestroy());
}

TEST_CASE("lex test arithmetic expression")
{
    std::ofstream outfile ("data/test4");
    outfile << "1 + 2 * 3 * 4" << std::endl;
    outfile.close();
    vector<Token> expect = vector<Token>{
        {TokenNumber ,1},
        {TokenPlus  ,-1},
        {TokenNumber ,2},
        {TokenMul   ,-1},
        {TokenNumber ,3},
        {TokenMul   ,-1},
        {TokenNumber ,4}
    };

    void* prm = NULL;
    REQUIRE(Success == LexCreate());
    REQUIRE(Success == LexOpen(&prm, (char*)"data/test4"));

    Token T;
    int i = 0;
    while(Success == LexProc(prm, &T)) {
        Token t = expect[i];
        REQUIRE(T.tok == t.tok);
        if (TokenNumber == t.tok) {
            REQUIRE(T.value == t.value);
        }
        i++;
        if (TokenEof == T.tok) {
            break;
        }
    }

    REQUIRE(i == expect.size());
    REQUIRE(Success == LexClose(prm));
    REQUIRE(Success == LexDestroy());
}

