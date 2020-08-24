/*
 * Copyright (c) 2020/08 gravieb <truongptk30a3@gmail.com>
 * This file is released under the GPLv3
 */

#include "log.h"

#define RESET   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */
#define BLUE    "\033[34m"      /* Blue */
#define MAGENTA "\033[35m"      /* Magenta */
#define CYAN    "\033[36m"      /* Cyan */
#define WHITE   "\033[37m"      /* White */
#define BOLDBLACK   "\033[1m\033[30m"      /* Bold Black */
#define BOLDRED     "\033[1m\033[31m"      /* Bold Red */
#define BOLDGREEN   "\033[1m\033[32m"      /* Bold Green */
#define BOLDYELLOW  "\033[1m\033[33m"      /* Bold Yellow */
#define BOLDBLUE    "\033[1m\033[34m"      /* Bold Blue */
#define BOLDMAGENTA "\033[1m\033[35m"      /* Bold Magenta */
#define BOLDCYAN    "\033[1m\033[36m"      /* Bold Cyan */
#define BOLDWHITE   "\033[1m\033[37m"      /* Bold White */

#define LOG_OFF 0xFFFF
static int g_log_level = 2;

static char* g_log_level_msg[] = {
    "TRACE",
    "DEBUG",
    "INFO",
    "WARN",
    "ERROR",
    "CLGT" // fata error
};

static char* g_token_str[] = {
    "TokenEof",
    "TokenPlus",
    "TokenMinus",
    "TokenMul",
    "TokenDiv",
    "TokenEqual",
    "TokenNE",
    "TokenLT",
    "TokenLE",
    "TokenGT",
    "TokenGE",
    "TokenIf",
    "TokenElse",
    "TokenWhile",
    "TokenDo",
    "TokenFor",
    "TokenIntType",
    "TokenVoidType",
    "TokenLongType",
    "TokenNumber",
    "TokenLP",
    "TokenRP",
    "TokenLBracket",
    "TokenRBracket",
    "TokenAssign",
    "TokenPrint",
    "TokenReturn",
    "TokenEoi",
    "TokenSemi",
    "TokenIdentifier"
};

void set_mlog_level(int level)
{
    g_log_level = level;
}

char* tok2str(int tok)
{
    if (tok >= sizeof(g_token_str)/sizeof(g_token_str[0])) {
        return "NULL";
    }
    return g_token_str[tok];
}

void mlog(int level, char* format, ...)
{
    va_list varg;
    char buf[4096];
    if (level >= g_log_level || CLGT == level) {
        va_start(varg, format);
        vsnprintf(buf, sizeof(buf), format, varg);
        va_end(varg);

        fprintf(stdout,BOLDRED"[%s] "RESET,g_log_level_msg[level]);
        fprintf(stdout, "%s", buf);
    }
}

