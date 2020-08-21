/*
 * Copyright (c) 2018/12 truong <truongptk30a3@gmail.com>
 * This file is released under the GPLv3
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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
        {TokenRP    ,-1},
        {TokenEoi   ,-1}
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
        if (TokenEoi == T.tok) {
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
        {TokenNumber ,4},
        {TokenEoi   ,-1}
    };

    void* prm = NULL;
    REQUIRE(Success == LexCreate());
    REQUIRE(Success == LexOpen(&prm, (char*)"data/test2"));

    Token T;
    int i = 0;
    while(Success == LexProc(prm, &T)) {
        Token t = expect[i];
        REQUIRE(T.tok == t.tok);
        if (TokenNumber == t.tok) {
            REQUIRE(T.value == t.value);
        }

        i++;
        if (TokenEoi == T.tok) {
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
        {TokenNumber ,4},
        {TokenEoi   ,-1}
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
        if (TokenEoi == T.tok) {
            break;
        }
    }

    REQUIRE(i == expect.size());
    REQUIRE(Success == LexClose(prm));
    REQUIRE(Success == LexDestroy());
}

TEST_CASE("lex test arithmetic expression with special character")
{
    std::ofstream outfile ("data/test5");
    outfile << "1   " << std::endl;
    outfile << "   +" << std::endl;
    outfile << "2 " << std::endl;
    outfile << "* " << std::endl;
    outfile << "3 " << std::endl;
    outfile << " *" << std::endl;
    outfile << " 4" << std::endl;
    outfile << " /" << std::endl;
    outfile << " 5" << std::endl;
    outfile.close();
    vector<Token> expect = vector<Token>{
        {TokenNumber ,1},
        {TokenPlus  ,-1},
        {TokenNumber ,2},
        {TokenMul   ,-1},
        {TokenNumber ,3},
        {TokenMul   ,-1},
        {TokenNumber ,4},
        {TokenDiv   ,-1},
        {TokenNumber ,5},        
        {TokenEoi   ,-1}
    };

    void* prm = NULL;
    REQUIRE(Success == LexCreate());
    REQUIRE(Success == LexOpen(&prm, (char*)"data/test5"));

    Token T;
    int i = 0;
    while(Success == LexProc(prm, &T)) {
        Token t = expect[i];
        REQUIRE(T.tok == t.tok);
        if (TokenNumber == t.tok) {
            REQUIRE(T.value == t.value);
        }
        i++;
        if (TokenEoi == T.tok) {
            break;
        }
    }

    REQUIRE(i == expect.size());
    REQUIRE(Success == LexClose(prm));
    REQUIRE(Success == LexDestroy());
}


TEST_CASE("lex test token: int type, L&R bracket, return")
{
    std::ofstream outfile ("data/test6");
    outfile << "int" << std::endl;
    outfile << "void" << std::endl;
    outfile << "{}" << std::endl;
    outfile << "return" << std::endl;
    outfile.close();
    vector<Token> expect = vector<Token>{
        {TokenIntType  ,-1},
        {TokenVoidType ,-1},
        {TokenLBracket ,-1},
        {TokenRBracket ,-1},
        {TokenReturn   ,-1},
        {TokenEoi      ,-1}
    };

    void* prm = NULL;
    REQUIRE(Success == LexCreate());
    REQUIRE(Success == LexOpen(&prm, (char*)"data/test6"));

    Token T;
    int i = 0;
    while(Success == LexProc(prm, &T)) {
        Token t = expect[i];
        REQUIRE(T.tok == t.tok);
        i++;
        if (TokenEoi == T.tok) {
            break;
        }
    }

    REQUIRE(i == expect.size());
    REQUIRE(Success == LexClose(prm));
    REQUIRE(Success == LexDestroy());
}

TEST_CASE("lex test token: int type, equal, identifier")
{
    std::ofstream outfile ("data/test7");
    outfile << "int cnt = 10;" << std::endl;

    outfile.close();
    vector<Token> expect = vector<Token>{
        {TokenIntType    ,-1},
        {TokenIdentifier ,-1},
        {TokenAssign     ,-1},
        {TokenNumber     ,10},
        {TokenSemi       ,-1},
        {TokenEoi        ,-1}
    };

    expect[1].id_str = strdup("cnt");
    void* prm = NULL;
    REQUIRE(Success == LexCreate());
    REQUIRE(Success == LexOpen(&prm, (char*)"data/test7"));

    Token T;
    int i = 0;
    while(Success == LexProc(prm, &T)) {
        Token t = expect[i];
        REQUIRE(T.tok == t.tok);
        i++;
        if (TokenNumber == T.tok) {
            REQUIRE(T.value == t.value);
        } else if (TokenIdentifier == T.tok) {
            REQUIRE(0 == strncmp(T.id_str, t.id_str, strlen(t.id_str)));
        } else if (TokenEoi == T.tok) {
            break;
        }
    }

    REQUIRE(i == expect.size());
    REQUIRE(Success == LexClose(prm));
    REQUIRE(Success == LexDestroy());
}

