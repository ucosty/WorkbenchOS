// WorkbenchOS
// Copyright (c) 2021 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

[[noreturn]] void panic(Lib::Error error);

#define TRY(expression)                  \
    ({                                   \
        auto response = (expression);    \
        if (response.is_error())         \
            return response.get_error(); \
        response.get();                  \
    })

#define TRY_PANIC(expression)            \
    ({                                   \
        auto response = (expression);    \
        if (response.is_error())         \
            panic(response.get_error()); \
        response.get();                  \
    })
