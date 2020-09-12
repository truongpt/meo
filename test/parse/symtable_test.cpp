/*
 * Copyright (c) 2020/08 gravieb <truongptk30a3@gmail.com>
 * This file is released under the GPLv3
 */
#include "symtable.h"
#include "error_code.h"

#include "catch.hh"

TEST_CASE("symtable add - find work")
{
    SymbolTable st;
    char* s1 = "conchocon";
    char* s2 = "conmeo";
    char* s3 = "conchocanconmeoconmeoduoiconchuot";
    REQUIRE(Success == symtable_init(&st));
    REQUIRE(Success == symtable_add(&st, s1, 0));
    REQUIRE(-1 != symtable_find(&st, s1, 0));
    REQUIRE(-1 == symtable_find(&st, s2, 0));
    REQUIRE(Success == symtable_add(&st, s2, 0));
    REQUIRE(-1 != symtable_find(&st, s2, 0));

    REQUIRE(-1 == symtable_find(&st, s3, 0));
    REQUIRE(Success == symtable_add(&st, s3, 0));
    REQUIRE(-1 != symtable_find(&st, s3, 0));
}

TEST_CASE("symtable add - find valid work")
{
    SymbolTable st;
    char* s1 = "a";
    char* s2 = "b";
    char* s3 = "c";
    REQUIRE(Success == symtable_init(&st));
    REQUIRE(Success == symtable_add(&st, s1, 0));
    REQUIRE(Success == symtable_add(&st, s2, 1));
    REQUIRE(Success == symtable_add(&st, s3, 2));

    REQUIRE(-1 != symtable_find_valid(&st, s1, 100));
    REQUIRE(-1 != symtable_find_valid(&st, s2, 100));
    REQUIRE(-1 != symtable_find_valid(&st, s3, 100));
}
