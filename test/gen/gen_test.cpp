/*
 * Copyright (c) 2020/07 gravieb <truongptk30a3@gmail.com>
 * This file is released under the GPLv3
 */

#include <stdio.h>
#include <string>
#include <iostream>
#include <fstream>
#include "gen.h"
#include "error_code.h"

#include "common.h"
#include "catch.hh"

using namespace std;
int32_t MockGetValue(char* r);

TEST_CASE("gen test get single resource")
{
    create_folder("data");
    void* prm = NULL;
    REQUIRE(Success == GenCreate());
    REQUIRE(Success == GenOpen(&prm, GenX86_64, (char*)"data/test1"));

    REQUIRE(Success == GenClose(prm));
    REQUIRE(Success == GenDestroy());
}

TEST_CASE("gen test get multi resource")
{
    int max_rsc = 10;
    vector<void*> prm = vector<void*>(10,NULL);
    REQUIRE(Success == GenCreate());
    for (auto& p : prm) {
        REQUIRE(Success == GenOpen(&p, GenX86_64, (char*)"data/test2"));
        REQUIRE(NULL != p);
    }

    REQUIRE(InParameterInvalid == GenOpen(&prm[0], GenX86_64, (char*)"data/test21"));

    void* a_prm = NULL;
    REQUIRE(GenLimitResource == GenOpen(&a_prm, GenX86_64, (char*)"data/test22"));
    REQUIRE(NULL == a_prm);

    for (auto& p : prm) {
        REQUIRE(Success == GenClose(p));
    }

    REQUIRE(Success == GenDestroy());
}

TEST_CASE("gen test basic parttern + ")
{
    void* prm = NULL;
    REQUIRE(Success == GenCreate());
    REQUIRE(Success == GenOpen(&prm, GenX86_64, (char*)"data/test3"));

    char* r1 = GenLoad(prm, 1);
    REQUIRE(NULL != r1);

    char* r2 = GenLoad(prm, 2);
    REQUIRE(NULL != r2);

    char* r3 = GenPlus(prm, r1,r2);
    int32_t total = MockGetValue(r3);
    REQUIRE(total == (1+2));
    REQUIRE(Success == GenClose(prm));
    REQUIRE(Success == GenDestroy());
}


TEST_CASE("gen test basic parttern -")
{
    void* prm = NULL;
    REQUIRE(Success == GenCreate());
    REQUIRE(Success == GenOpen(&prm, GenX86_64, (char*)"data/test3"));

    char* r1 = GenLoad(prm, 1);
    REQUIRE(NULL != r1);

    char* r2 = GenLoad(prm, 2);
    REQUIRE(NULL != r2);

    char* r3 = GenMinus(prm, r1,r2);

    int32_t total = MockGetValue(r3);
    REQUIRE(total == (1-2));
    REQUIRE(Success == GenClose(prm));
    REQUIRE(Success == GenDestroy());
}


TEST_CASE("gen test basic parttern * ")
{
    void* prm = NULL;
    REQUIRE(Success == GenCreate());
    REQUIRE(Success == GenOpen(&prm, GenX86_64, (char*)"data/test3"));

    char* r1 = GenLoad(prm, 3);
    REQUIRE(NULL != r1);

    char* r2 = GenLoad(prm, 2);
    REQUIRE(NULL != r2);

    char* r3 = GenMul(prm, r1,r2);

    int32_t total = MockGetValue(r3);
    REQUIRE(total == (3*2));
    REQUIRE(Success == GenClose(prm));
    REQUIRE(Success == GenDestroy());
}


TEST_CASE("gen test basic parttern / ")
{
    void* prm = NULL;
    REQUIRE(Success == GenCreate());
    REQUIRE(Success == GenOpen(&prm, GenX86_64, (char*)"data/test3"));

    char* r1 = GenLoad(prm, 10);
    REQUIRE(NULL != r1);

    char* r2 = GenLoad(prm, 2);
    REQUIRE(NULL != r2);

    char* r3 = GenDiv(prm, r1,r2);

    int32_t total = MockGetValue(r3);
    REQUIRE(total == (10/2));
    REQUIRE(Success == GenClose(prm));
    REQUIRE(Success == GenDestroy());
}

TEST_CASE("gen test basic parttern combine arithmetic 10*2+4/2-5")
{
    void* prm = NULL;
    REQUIRE(Success == GenCreate());
    REQUIRE(Success == GenOpen(&prm, GenX86_64, (char*)"data/test3"));


    Token t1 = {TokenNumber, 10};
    Token t2 = {TokenNumber, 2};
    Token t3 = {TokenNumber, 4};
    Token t4 = {TokenNumber, 2};
    Token t5 = {TokenNumber, 5};

    char* r1 = GenLoad(prm, 10);
    REQUIRE(NULL != r1);
    char* r2 = GenLoad(prm, 2);
    REQUIRE(NULL != r2);

    char* r3 = GenMul(prm, r1,r2);
    REQUIRE(NULL != r3);

    char* r4 = GenLoad(prm, 4);
    REQUIRE(NULL != r4);

    char* r5 = GenLoad(prm, 2);
    REQUIRE(NULL != r5);

    char* r6 = GenDiv(prm, r4, r5);

    char* r7 = GenPlus(prm, r3, r6); // 10*2+4/2

    char* r8 = GenLoad(prm, 5);
    REQUIRE(NULL != r8);

    char* r9 = GenMinus(prm, r7, r8); // 10*2+4/2-5

    int32_t total = MockGetValue(r9);
    REQUIRE(total == (10*2+4/2-5));
    REQUIRE(Success == GenClose(prm));
    REQUIRE(Success == GenDestroy());
}
