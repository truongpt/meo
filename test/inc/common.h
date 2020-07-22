/*
 * Copyright (c) 2020/07 gravieb <truongptk30a3@gmail.com>
 * This file is released under the GPLv3
 */
#include <stdio.h>
#include <sys/stat.h>
#include <string>

#ifndef _COMMON_TEST_H_
#define _COMMON_TEST_H_

void create_folder(std::string folder)
{
    struct stat sb;
    if (!(stat(folder.c_str(), &sb) == 0 && S_ISDIR(sb.st_mode))) {
        system(("mkdir " + folder).c_str());
    }
}

#endif
