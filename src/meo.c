/*
 * Copyright (c) 2020/07 gravieb <truongptk30a3@gmail.com>
 * This file is released under the GPLv3
 */

#include<stdio.h>
#include<stdbool.h>
#include<string.h>
#include "lex.h"
#include "parse.h"
#include "gen.h"
#include "meo.h"
#include "log.h"

typedef struct MeoHandle {
    void* lex_prm;
    void* gen_prm;
    void* parse_prm;
} MeoHandle;

int main(int argc, char*argv[])
{
    if (argc < 2) {
        mlog(CLGT,"input parameter error\n");
        return -1;
    }

    char* in_file = NULL;
    bool is_interpreter = false;
    if (!strncmp(argv[1], "-i", sizeof("-i"))) {
        is_interpreter = true;
        in_file = argv[2];
    } else {
        is_interpreter = false;
        in_file = argv[1];
    }

    char* out_file = "out.s";
    MeoHandle meo_handle = {NULL, NULL};

    LexCreate();
    LexOpen(&(meo_handle.lex_prm), in_file);

    GenCreate();
    GenOpen(&(meo_handle.gen_prm), GenX86_64, out_file);

    ParseCreate();
    ParseOpen(&(meo_handle.parse_prm), meo_handle.lex_prm, meo_handle.gen_prm, is_interpreter);

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
