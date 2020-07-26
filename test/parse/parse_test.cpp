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

