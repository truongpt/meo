/*
 * Copyright (c) 2020/07 gravieb <truongptk30a3@gmail.com>
 * This file is released under the GPLv3
 */

#ifndef _ERROR_CODE_H_
#define _ERROR_CODE_H_

enum ErrorCode {
    Success,
    InParameterInvalid,
    OpenFileError,
    SyntaxError,
    TokenError,
    LexLimitResource,
    ParseLimitResource,
    GenLimitResource
};
#endif // _ERROR_CODE_H_
