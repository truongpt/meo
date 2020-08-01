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

TEST_CASE("gen test basic parttern")
{
    void* prm = NULL;
    REQUIRE(Success == GenCreate());
    REQUIRE(Success == GenOpen(&prm, GenX86_64, (char*)"data/test3"));

    REQUIRE(Success == GenClose(prm));
    REQUIRE(Success == GenDestroy());
}
