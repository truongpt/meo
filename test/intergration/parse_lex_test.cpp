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
int32_t MockGetPrintValue();

TEST_CASE("basic arithmetic op parttern:print 1+2;")
{
    create_folder("data");
    std::ofstream outfile ("data/test1");
    outfile << "print 1+2;" << std::endl;
    outfile.close();

    void* lex_prm = NULL;
    REQUIRE(Success == LexCreate());
    REQUIRE(Success == LexOpen(&lex_prm, (char*)"data/test1"));

    void* gen_prm = NULL;
    REQUIRE(Success == GenCreate());
    REQUIRE(Success == GenOpen(&gen_prm, GenX86_64, (char*)"data/out1"));

    void* parse_prm = NULL;
    REQUIRE(Success == ParseCreate());
    REQUIRE(Success == ParseOpen(&parse_prm, lex_prm, gen_prm, false));
    
    REQUIRE(Success == ParseProc(parse_prm));
    REQUIRE(MockGetPrintValue() == (1+2));

    ParseClose(parse_prm);
    ParseDestroy();

    REQUIRE(Success == GenClose(gen_prm));
    REQUIRE(Success == GenDestroy());

    REQUIRE(Success == LexClose(lex_prm));
    REQUIRE(Success == LexDestroy());

}

TEST_CASE("basic variable parttern: int a; int b;")
{
    std::ofstream outfile ("data/test2");
    outfile << "int a;" << std::endl;
    outfile << "int b;" << std::endl;
    outfile << "a = 2;" << std::endl;
    outfile << "b = 5;" << std::endl;
    outfile << "print a+b;" << std::endl;
    outfile << "a = a*b;" << std::endl;
    outfile << "print a;" << std::endl;
    outfile << "print a*b;" << std::endl;
    outfile.close();

    void* lex_prm = NULL;
    REQUIRE(Success == LexCreate());
    REQUIRE(Success == LexOpen(&lex_prm, (char*)"data/test2"));

    void* gen_prm = NULL;
    REQUIRE(Success == GenCreate());
    REQUIRE(Success == GenOpen(&gen_prm, GenX86_64, (char*)"data/out2"));

    void* parse_prm = NULL;
    REQUIRE(Success == ParseCreate());
    REQUIRE(Success == ParseOpen(&parse_prm, lex_prm, gen_prm, false));
    
    REQUIRE(Success == ParseProc(parse_prm));
    REQUIRE(MockGetPrintValue() == (2+5));
    REQUIRE(MockGetPrintValue() == (2*5));
    REQUIRE(MockGetPrintValue() == (2*5*5));

    ParseClose(parse_prm);
    ParseDestroy();

    REQUIRE(Success == GenClose(gen_prm));
    REQUIRE(Success == GenDestroy());

    REQUIRE(Success == LexClose(lex_prm));
    REQUIRE(Success == LexDestroy());

}

TEST_CASE("basic variable parttern: hw reg resource manager test")
{
    std::ofstream outfile ("data/test3");
    outfile << "int a1;" << std::endl;
    outfile << "int a2;" << std::endl;
    outfile << "int a3;" << std::endl;
    outfile << "int a4;" << std::endl;
    outfile << "int a5;" << std::endl;
    outfile << "int a6;" << std::endl;
    outfile << "int a7;" << std::endl;
    outfile << "a1 = 1;" << std::endl;
    outfile << "a2 = 2;" << std::endl;
    outfile << "a3 = 3;" << std::endl;
    outfile << "a4 = 4;" << std::endl;
    outfile << "a5 = 5;" << std::endl;
    outfile << "a6 = 6;" << std::endl;
    outfile << "print a1+a2+a3+a4+a5+a6;" << std::endl;
    outfile.close();

    void* lex_prm = NULL;
    REQUIRE(Success == LexCreate());
    REQUIRE(Success == LexOpen(&lex_prm, (char*)"data/test3"));

    void* gen_prm = NULL;
    REQUIRE(Success == GenCreate());
    REQUIRE(Success == GenOpen(&gen_prm, GenX86_64, (char*)"data/out3"));

    void* parse_prm = NULL;
    REQUIRE(Success == ParseCreate());
    REQUIRE(Success == ParseOpen(&parse_prm, lex_prm, gen_prm, false));
    
    REQUIRE(Success == ParseProc(parse_prm));
    REQUIRE(MockGetPrintValue() == (1+2+3+4+5+6));

    ParseClose(parse_prm);
    ParseDestroy();

    REQUIRE(Success == GenClose(gen_prm));
    REQUIRE(Success == GenDestroy());

    REQUIRE(Success == LexClose(lex_prm));
    REQUIRE(Success == LexDestroy());

}


TEST_CASE("relational basic pattern")
{
    std::ofstream outfile ("data/test4");
    outfile << "print 1 < 2;" << std::endl;
    outfile << "print 1 <= 1;" << std::endl;
    outfile << "print 1 > 2;" << std::endl;
    outfile << "print 2 >= 2;" << std::endl;
    outfile.close();

    void* lex_prm = NULL;
    REQUIRE(Success == LexCreate());
    REQUIRE(Success == LexOpen(&lex_prm, (char*)"data/test4"));

    void* gen_prm = NULL;
    REQUIRE(Success == GenCreate());
    REQUIRE(Success == GenOpen(&gen_prm, GenX86_64, (char*)"data/out4"));

    void* parse_prm = NULL;
    REQUIRE(Success == ParseCreate());
    REQUIRE(Success == ParseOpen(&parse_prm, lex_prm, gen_prm, false));

    REQUIRE(Success == ParseProc(parse_prm));
    REQUIRE(MockGetPrintValue() == 1);
    REQUIRE(MockGetPrintValue() == 1);
    REQUIRE(MockGetPrintValue() == 0);
    REQUIRE(MockGetPrintValue() == 1);

    ParseClose(parse_prm);
    ParseDestroy();

    REQUIRE(Success == GenClose(gen_prm));
    REQUIRE(Success == GenDestroy());

    REQUIRE(Success == LexClose(lex_prm));
    REQUIRE(Success == LexDestroy());
}

TEST_CASE("relational basic pattern with variable")
{
    std::ofstream outfile ("data/test5");
    outfile << "int a1;" << std::endl;
    outfile << "int a2;" << std::endl;
    outfile << "a1 = 1;" << std::endl;
    outfile << "a2 = 2;" << std::endl;
    outfile << "print a1 > a2;" << std::endl;
    outfile << "print a1 < a2;" << std::endl;
    outfile.close();

    void* lex_prm = NULL;
    REQUIRE(Success == LexCreate());
    REQUIRE(Success == LexOpen(&lex_prm, (char*)"data/test5"));

    void* gen_prm = NULL;
    REQUIRE(Success == GenCreate());
    REQUIRE(Success == GenOpen(&gen_prm, GenX86_64, (char*)"data/out5"));

    void* parse_prm = NULL;
    REQUIRE(Success == ParseCreate());
    REQUIRE(Success == ParseOpen(&parse_prm, lex_prm, gen_prm, false));

    REQUIRE(Success == ParseProc(parse_prm));
    REQUIRE(MockGetPrintValue() == 0);
    REQUIRE(MockGetPrintValue() == 1);

    ParseClose(parse_prm);
    ParseDestroy();

    REQUIRE(Success == GenClose(gen_prm));
    REQUIRE(Success == GenDestroy());

    REQUIRE(Success == LexClose(lex_prm));
    REQUIRE(Success == LexDestroy());

}

TEST_CASE("equal basic pattern with variable")
{
    std::ofstream outfile ("data/test6");
    outfile << "int a1;" << std::endl;
    outfile << "int a2;" << std::endl;
    outfile << "a1 = 2;" << std::endl;
    outfile << "a2 = 2;" << std::endl;
    outfile << "print a1 == a2;" << std::endl;
    outfile << "print a1 != a2;" << std::endl;
    outfile.close();

    void* lex_prm = NULL;
    REQUIRE(Success == LexCreate());
    REQUIRE(Success == LexOpen(&lex_prm, (char*)"data/test6"));

    void* gen_prm = NULL;
    REQUIRE(Success == GenCreate());
    REQUIRE(Success == GenOpen(&gen_prm, GenX86_64, (char*)"data/out6"));

    void* parse_prm = NULL;
    REQUIRE(Success == ParseCreate());
    REQUIRE(Success == ParseOpen(&parse_prm, lex_prm, gen_prm, false));

    REQUIRE(Success == ParseProc(parse_prm));
    REQUIRE(MockGetPrintValue() == 1);
    REQUIRE(MockGetPrintValue() == 0);

    ParseClose(parse_prm);
    ParseDestroy();

    REQUIRE(Success == GenClose(gen_prm));
    REQUIRE(Success == GenDestroy());

    REQUIRE(Success == LexClose(lex_prm));
    REQUIRE(Success == LexDestroy());

}

TEST_CASE("basic parttern with {}")
{
    std::ofstream outfile ("data/test7");
    outfile << "int a1;" << std::endl;
    outfile << "int a2;" << std::endl;
    outfile << "{a1 = 1;}" << std::endl;
    outfile << "a2 = 2;" << std::endl;
    outfile << "{print a1;}" << std::endl;
    outfile << "{{{{print a2;}}}}" << std::endl;
    outfile.close();

    void* lex_prm = NULL;
    REQUIRE(Success == LexCreate());
    REQUIRE(Success == LexOpen(&lex_prm, (char*)"data/test7"));

    void* gen_prm = NULL;
    REQUIRE(Success == GenCreate());
    REQUIRE(Success == GenOpen(&gen_prm, GenX86_64, (char*)"data/out7"));

    void* parse_prm = NULL;
    REQUIRE(Success == ParseCreate());
    REQUIRE(Success == ParseOpen(&parse_prm, lex_prm, gen_prm, false));

    REQUIRE(Success == ParseProc(parse_prm));
    REQUIRE(MockGetPrintValue() == 1);
    REQUIRE(MockGetPrintValue() == 2);

    ParseClose(parse_prm);
    ParseDestroy();

    REQUIRE(Success == GenClose(gen_prm));
    REQUIRE(Success == GenDestroy());

    REQUIRE(Success == LexClose(lex_prm));
    REQUIRE(Success == LexDestroy());

}

#if 0
TEST_CASE("basic if-else pattern")
{
    std::ofstream outfile ("data/test7");
    outfile << "int a1;" << std::endl;
    outfile << "int a2;" << std::endl;
    outfile << "a1 = 1;" << std::endl;
    outfile << "a2 = 2;" << std::endl;
    outfile << "if (a1 > a2)" << std::endl;
    outfile << "{print a1;}" << std::endl;
    outfile << "else" << std::endl;
    outfile << "{ print a2;}" << std::endl;
    outfile.close();

    void* lex_prm = NULL;
    REQUIRE(Success == LexCreate());
    REQUIRE(Success == LexOpen(&lex_prm, (char*)"data/test7"));

    void* gen_prm = NULL;
    REQUIRE(Success == GenCreate());
    REQUIRE(Success == GenOpen(&gen_prm, GenX86_64, (char*)"data/out7"));

    void* parse_prm = NULL;
    REQUIRE(Success == ParseCreate());
    REQUIRE(Success == ParseOpen(&parse_prm, lex_prm, gen_prm, false));

    REQUIRE(Success == ParseProc(parse_prm));
    REQUIRE(MockGetPrintValue() == 2);

    ParseClose(parse_prm);
    ParseDestroy();

    REQUIRE(Success == GenClose(gen_prm));
    REQUIRE(Success == GenDestroy());

    REQUIRE(Success == LexClose(lex_prm));
    REQUIRE(Success == LexDestroy());

}

#endif
