/*
 * Copyright (c) 2020/07 gravieb <truongptk30a3@gmail.com>
 * This file is released under the GPLv3
 */

#include<stdio.h>
#include "lex.h"
#include "parse.h"
#include "gen.h"
#include "meo.h"

typedef struct MeoHandle {
    void* lex_prm;
    void* gen_prm;
    void* parse_prm;
} MeoHandle;

int main(int argc, char*argv[])
{
    if (argc < 2) {
        printf ("input parameter error\n");
        return -1;
    }

    char* out_file = "out.s";
    MeoHandle meo_handle = {NULL, NULL};

    LexCreate();
    LexOpen(&(meo_handle.lex_prm), argv[1]);

    GenCreate();
    GenOpen(&(meo_handle.gen_prm), GenX86_64, out_file);

    ParseCreate();
    ParseOpen(&(meo_handle.parse_prm), meo_handle.lex_prm, meo_handle.gen_prm);

    ParseProc(meo_handle.parse_prm);

    ParseClose(meo_handle.parse_prm);
    LexClose(meo_handle.lex_prm);
    GenClose(meo_handle.gen_prm);

    LexDestroy();
    GenDestroy();
    ParseDestroy();

    // using backend of gcc
    system("cc -o a.out out.s");

    return 0;
}
