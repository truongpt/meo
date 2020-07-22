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
