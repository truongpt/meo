/*
 * Copyright (c) 2020/07 gravieb <truongptk30a3@gmail.com>
 * This file is released under the GPLv3
 */

#include<stdio.h>
#include "lex.h"
#include "parse.h"
#include "gen.h"
#include "meo.h"

int main(int argc, char*argv[])
{
    if (argc != 2) {
        printf ("input parameter error\n");
        return -1;
    }
    printf("Hello, World\n");

    return 0;
}
